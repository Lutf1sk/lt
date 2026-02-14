#include <lt2/net.h>

#ifdef ON_UNIX
#	include <sys/socket.h>
#	include <poll.h>
#	include <netinet/in.h>
#	include <unistd.h>
#	include <netdb.h>
#	include <errno.h>
#	include <fcntl.h>

socket_addr resolve_host(ls host, err* err) {
	char cstr[512];
	memcpy(cstr, host.ptr, host.size);
	cstr[host.size] = 0;

	struct addrinfo hints = {
		.ai_family = AF_UNSPEC,
		.ai_flags  = AI_PASSIVE,
	};

	struct addrinfo* res;
	if (getaddrinfo(cstr, NULL, &hints, &res) < 0) {
		throw(err, ERR_NOT_FOUND, "name resolution failed"); // !!
		return (struct socket_addr){0};
	}

	for (struct addrinfo* it = res; it; it = it->ai_next) {
		if (it->ai_addr->sa_family == AF_INET) {
			struct socket_addr addr = { SOCKADDR_IPV4 };
			memcpy(addr.ip_addr, &((struct sockaddr_in*)it->ai_addr)->sin_addr, 4);
			return addr;
		}
		if (it->ai_addr->sa_family == AF_INET6) {
			struct socket_addr addr = { SOCKADDR_IPV6 };
			memcpy(addr.ip_addr, &((struct sockaddr_in6*)it->ai_addr)->sin6_addr, 16);
			return addr;
		}
	}
	throw(err, ERR_NOT_FOUND, "name resolution failed"); // !!
	return (struct socket_addr){0};
}

socket_handle socket_open(socket_type type, err* err) {
	int posix_type = 0;
	if (type & SOCKET_ASYNC) {
		posix_type |= SOCK_NONBLOCK;
		type &= ~SOCKET_ASYNC;
	}

	if (type == SOCKET_TCP)
		posix_type |= SOCK_STREAM;
	else if (type == SOCKET_UDP)
		posix_type |= SOCK_DGRAM;
	else {
		throw(err, ERR_BAD_ARGUMENT, "invalid socket type"); // !!
		return -1;
	}

	int sock = socket(AF_INET, posix_type, 0);
	if (sock < 0) {
		throw_errno(err);
		return -1;
	}
	return sock;
}

void socket_close(socket_handle sock, err* err) {
	if (close(sock) < 0)
		throw_errno(err);
}

b8 socket_connect_tcp(socket_handle sock, socket_addr* addr, u16 port, err* err) {
	struct sockaddr_in ipv4;
	struct sockaddr_in6 ipv6;
	struct sockaddr* posix_addr;
	socklen_t socklen;

	if (addr->type == SOCKADDR_IPV4) {
		ipv4.sin_family = AF_INET;
		ipv4.sin_port = htons(port);
		memcpy(&ipv4.sin_addr, addr->ip_addr, 4);
		posix_addr = (struct sockaddr*)&ipv4;
		socklen = sizeof(ipv4);
	}
	else if (addr->type == SOCKADDR_IPV6) {
		ipv6.sin6_family = AF_INET;
		ipv6.sin6_port = htons(port);
		ipv6.sin6_flowinfo = 0;
		memcpy(&ipv6.sin6_addr, addr->ip_addr, 16);
		ipv6.sin6_scope_id = 0;
		posix_addr = (struct sockaddr*)&ipv6;
		socklen = sizeof(ipv6);
	}
	else {
		throw(err, ERR_BAD_ARGUMENT, "invalid socket type");
		return 0;
	}

	if (connect(sock, posix_addr, socklen) < 0) {
		throw_errno(err);
		return 0;
	}
	return 1;
}

b8 socket_bind(socket_handle sock, u16 port, err* err) {
	int reuse_addr = 1;
	if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, (void*)&reuse_addr, sizeof(int)) < 0) {
		throw_errno(err);
		return 0;
	}

	struct sockaddr_in server_addr;
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = INADDR_ANY;
	server_addr.sin_port = htons(port);

	if (bind(sock, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
		throw_errno(err);
		return 0;
	}
	if (listen(sock, 16) < 0) {
		throw_errno(err);
		return 0;
	}
	return 1;
}

socket_handle socket_accept(socket_handle sock, socket_addr* out_addr, socket_type flags, err* err) {
	struct sockaddr posix_addr;
	socklen_t addrsize = sizeof(posix_addr);

	socket_handle new_fd = accept(sock, (struct sockaddr*)&posix_addr, (socklen_t*)&addrsize);
	if (new_fd < 0) {
		throw_errno(err);
		return -1;
	}

	if (flags & SOCKET_ASYNC) {
		int posix_flags = fcntl(new_fd, F_GETFL, 0);
		if (posix_flags < 0 || fcntl(new_fd, F_SETFL, posix_flags | O_NONBLOCK)) {
			throw_errno(err);
			goto err0;
		}
	}

	if (!out_addr)
		return new_fd;

	if (posix_addr.sa_family == AF_INET) {
		out_addr->type = SOCKADDR_IPV4;
		memcpy(out_addr->ip_addr, &((struct sockaddr_in*)&posix_addr)->sin_addr, 4);
	}
	else if (posix_addr.sa_family == AF_INET6) {
		// TODO
		//out_addr->type = SOCKADDR_IPV6;
		//memcpy(out_addr->ip_addr, &((struct sockaddr_in6*)&posix_addr)->sin6_addr, 16);
	}
	return new_fd;

err0:	close(new_fd);
	return -1;
}

usz socket_send(socket_handle sock, const void* data, usz size, err* err) {
	isz res = send(sock, data, size, 0);
	if (res < 0) {
		throw_errno(err);
		return 0;
	}
	if (!res && size)
		throw(err, ERR_CLOSED, "cannot send data to a closed socket");
	return res;
}

usz socket_receive(socket_handle sock, void* data, usz size, err* err) {
	isz res = recv(sock, data, size, 0);
	if (res < 0) {
		throw_errno(err);
		return 0;
	}
	if (!res && size)
		throw(err, ERR_CLOSED, "cannot receive data from a closed socket");
	return res;
}

#endif // ON_UNIX

