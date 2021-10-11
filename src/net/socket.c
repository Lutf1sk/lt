#include <lt/net.h>
#include <lt/lt.h>
#include <lt/mem.h>

#if defined(LT_UNIX)
#	include <sys/socket.h>
#	include <netdb.h>
#	include <unistd.h>

typedef
struct lt_socket {
	int fd;
} lt_socket_t;

typedef
struct lt_sockaddr_impl {
	ulong addr_len;
	struct sockaddr addr;
} lt_sockaddr_impl_t;

static
int lt_socktype_to_unix(lt_socktype_t type) {
	switch (type) {
	case LT_SOCKTYPE_TCP: return SOCK_STREAM;
	case LT_SOCKTYPE_UDP: return SOCK_DGRAM;
	default: return -1;
	}
}

#elif defined(LT_WINDOWS)

#endif

b8 lt_sockaddr_resolve(char* addr, char* port, lt_socktype_t type, lt_sockaddr_t* out_addr_) {
#if defined(LT_UNIX)
	lt_sockaddr_impl_t* out_addr = (lt_sockaddr_impl_t*)out_addr_;

	struct addrinfo hints;
	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = lt_socktype_to_unix(type);

	struct addrinfo* resolved;
	if (getaddrinfo(addr, port, &hints, &resolved) != 0)
		return 0;
	if (!resolved)
		return 0;

	out_addr->addr_len = resolved->ai_addrlen;
	out_addr->addr = *resolved->ai_addr;
	freeaddrinfo(resolved);
	return 1;
#endif
}

lt_socket_t* lt_socket_create(lt_arena_t* arena, lt_socktype_t type) {
#if defined(LT_UNIX)
	int fd = socket(AF_INET, lt_socktype_to_unix(type), 0);
	if (fd < 0)
		return NULL;

	lt_socket_t* sock = lt_arena_reserve(arena, sizeof(lt_socket_t));
	sock->fd = fd;
	return sock;
#endif
}

void lt_socket_destroy(lt_socket_t* sock) {
#if defined(LT_UNIX)
	close(sock->fd);
#endif
}

b8 lt_socket_connect(lt_socket_t* sock, lt_sockaddr_t* addr_) {
#if defined(LT_UNIX)
	lt_sockaddr_impl_t* addr = (lt_sockaddr_impl_t*)addr_;
	return connect(sock->fd, &addr->addr, addr->addr_len) >= 0;
#endif
}

b8 lt_socket_server(lt_socket_t* sock, u16 port) {
#if defined(LT_UNIX)
	int reuse_addr = 1;
	if (setsockopt(sock->fd, SOL_SOCKET, SO_REUSEADDR, &reuse_addr, sizeof(int)) < 0)
		return 0;

	struct sockaddr_in server_addr;
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = INADDR_ANY;
	server_addr.sin_port = htons(port);

	if (bind(sock->fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0)
		return 0;
	if (listen(sock->fd, 10) < 0)
		return 0;
	return 1;
#endif
}

lt_socket_t* lt_socket_accept(lt_arena_t* arena, lt_socket_t* sock) {
#if defined(LT_UNIX)
	struct sockaddr_in new_addr;
	usz new_size = sizeof(new_addr);
	int new_fd = accept(sock->fd, (struct sockaddr*)&new_addr, (socklen_t*)&new_size);
	if (new_fd < 0)
		return NULL;

	lt_socket_t* new_sock = lt_arena_reserve(arena, sizeof(lt_socket_t));
	new_sock->fd = new_fd;
	return new_sock;
#endif
}

isz lt_socket_send(lt_socket_t* sock, void* data, usz size) {
#if defined(LT_UNIX)
	return send(sock->fd, data, size, MSG_NOSIGNAL);
#endif
}

isz lt_socket_recv(lt_socket_t* sock, void* data, usz size) {
#if defined(LT_UNIX)
	return recv(sock->fd, data, size, 0);
#endif
}


