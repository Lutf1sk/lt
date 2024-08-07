#include <lt/net.h>
#include <lt/lt.h>
#include <lt/str.h>
#include <lt/io.h>
#include <lt/mem.h>

#ifdef LT_UNIX
#	include <sys/socket.h>
#	include <netdb.h>
#	include <unistd.h>
#	include <poll.h>

#	define SOCKET int
#	define INIT_IF_NECESSARY(x)
#	define LAST_ERROR() lt_errno()
#elifdef LT_WINDOWS
#	define WIN32_LEAN_AND_MEAN 1
#	include <windows.h>
#	include <winsock2.h>
#	include <ws2tcpip.h>

#	define LAST_ERROR() LT_ERR_UNKNOWN

#	define MSG_NOSIGNAL 0

static b8 ws_initialized = 0;
static WSADATA wsadata;

#	define INIT_IF_NECESSARY(onerr) { \
	if (!ws_initialized) { \
		int res = WSAStartup(MAKEWORD(2, 2), &wsadata); \
		if (res != 0) \
			return onerr; /* // !! */\
		ws_initialized = 1; \
	} \
}
#endif

typedef
struct lt_sockaddr_impl {
	socklen_t addr_len;
	struct sockaddr addr;
} lt_sockaddr_impl_t;

static
int lt_socktype_to_native(lt_socktype_t type) {
	int flags = 0;

	if (type & LT_SOCKTYPE_NONBLOCK) flags |= SOCK_NONBLOCK;

	switch (type & LT_SOCKTYPE_PROTOCOL_MASK) {
	case LT_SOCKTYPE_TCP: return flags | SOCK_STREAM;
	case LT_SOCKTYPE_UDP: return flags | SOCK_DGRAM;
	case LT_SOCKTYPE_RDM: return flags | SOCK_RDM;
	default: return -1;
	}
}

lt_err_t lt_sockaddr_resolve(lstr_t addr, u16 port, lt_socktype_t type, lt_sockaddr_t out_addr_[static 1], lt_alloc_t alloc[static 1]) {
	INIT_IF_NECESSARY(LAST_ERROR());
	lt_sockaddr_impl_t* out_addr = (lt_sockaddr_impl_t*)out_addr_;

	char* caddr = lt_lstos(addr, alloc);
	if (!caddr)
		return LT_ERR_OUT_OF_MEMORY;

	char cport[16];
	cport[lt_sprintf(cport, "%uw", port)] = 0;

	struct addrinfo hints;
	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = lt_socktype_to_native(type);

	struct addrinfo* resolved;
	isz res = getaddrinfo(caddr, cport, &hints, &resolved);

	lt_mfree(alloc, caddr);
	if (res != 0 || !resolved)
		return LT_ERR_NOT_FOUND;

	out_addr->addr_len = resolved->ai_addrlen;
	out_addr->addr = *resolved->ai_addr;
	freeaddrinfo(resolved);
	return LT_SUCCESS;
}

lt_socket_t* lt_socket_create(lt_socktype_t type, lt_alloc_t alloc[static 1]) {
	INIT_IF_NECESSARY(NULL);
	SOCKET fd = socket(AF_INET, lt_socktype_to_native(type), 0);
	if (fd < 0)
		return NULL;

	lt_socket_t* sock = lt_malloc(alloc, sizeof(lt_socket_t));
	sock->fd = fd;
	return sock;
}

void lt_socket_close(const lt_socket_t sock[static 1]) {
#ifdef LT_UNIX
	close(sock->fd);
#elifdef LT_WINDOWS
	closesocket(sock->fd);
#endif
}

void lt_socket_destroy(const lt_socket_t sock[static 1], lt_alloc_t alloc[static 1]) {
	lt_socket_close(sock);
	lt_mfree(alloc, sock);
}

lt_err_t lt_socket_connect(const lt_socket_t sock[static 1], const lt_sockaddr_t addr_[static 1]) {
	const lt_sockaddr_impl_t* addr = (const lt_sockaddr_impl_t*)addr_;
	if (connect(sock->fd, &addr->addr, addr->addr_len) < 0) {
		return LAST_ERROR();
	}
	return LT_SUCCESS;
}

lt_err_t lt_socket_server(const lt_socket_t sock[static 1], u16 port) {
	int reuse_addr = 1;
	if (setsockopt(sock->fd, SOL_SOCKET, SO_REUSEADDR, (void*)&reuse_addr, sizeof(int)) < 0)
		return LAST_ERROR();

	struct sockaddr_in server_addr;
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = INADDR_ANY;
	server_addr.sin_port = htons(port);

	if (bind(sock->fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0)
		return LAST_ERROR();
	if (listen(sock->fd, 10) < 0)
		return LAST_ERROR();
	return LT_SUCCESS;
}

lt_socket_t* lt_socket_accept(const lt_socket_t sock[static 1], lt_sockaddr_t out_addr_[static 1], lt_alloc_t alloc[static 1]) {
	struct sockaddr new_addr, *new_addr_p = &new_addr;
	socklen_t new_size = sizeof(new_addr), *new_size_p = &new_size;

	lt_sockaddr_impl_t* out_addr = (lt_sockaddr_impl_t*)out_addr_;
	if (out_addr) {
		out_addr->addr_len = sizeof(out_addr->addr);
		new_addr_p = &out_addr->addr;
		new_size_p = &out_addr->addr_len;
	}

	SOCKET new_fd = accept(sock->fd, (struct sockaddr*)new_addr_p, (socklen_t*)new_size_p);
	if (new_fd < 0) return NULL;

	lt_socket_t* new_sock = lt_malloc(alloc, sizeof(lt_socket_t));
	if (!new_sock) return NULL;

	new_sock->fd = new_fd;
	return new_sock;
}

lt_err_t lt_socket_pollw(const lt_socket_t sock[static 1], u64 timeout_usec) {
	struct pollfd pfd = { .fd = sock->fd, .events = POLLOUT | POLLERR };

	if (poll(&pfd, 1, timeout_usec / 1000) < 0) return lt_errno();

	if (pfd.revents) {
		int so_err = 1;
		socklen_t optlen = sizeof(so_err);
		if (getsockopt(sock->fd, SOL_SOCKET, SO_ERROR, &so_err, &optlen) < 0) return lt_errno();
		if (so_err) return lt_errno_to_err(so_err);
	}

	if (pfd.revents & POLLOUT) return LT_SUCCESS;

	return LT_ERR_TIMED_OUT;
}

lt_err_t lt_socket_pollr(const lt_socket_t sock[static 1], u64 timeout_usec) {
	struct pollfd pfd = { .fd = sock->fd, .events = POLLIN | POLLERR };

	if (poll(&pfd, 1, timeout_usec / 1000) < 0) return lt_errno();

	if (pfd.revents) {
		int so_err = 1;
		socklen_t optlen = sizeof(so_err);
		if (getsockopt(sock->fd, SOL_SOCKET, SO_ERROR, &so_err, &optlen) < 0) return lt_errno();
		if (so_err) return lt_errno_to_err(so_err);
	}

	if (pfd.revents & POLLIN) return LT_SUCCESS;

	return LT_ERR_TIMED_OUT;
}

u32 lt_sockaddr_ipv4_addr(const lt_sockaddr_t addr_[static 1]) {
	const lt_sockaddr_impl_t* addr = (const lt_sockaddr_impl_t*) addr_;
	const struct sockaddr_in* addr_in = (const struct sockaddr_in*)&addr->addr;
	return htonl(addr_in->sin_addr.s_addr);
}

u16 lt_sockaddr_ipv4_port(const lt_sockaddr_t addr_[static 1]) {
	const lt_sockaddr_impl_t* addr = (const lt_sockaddr_impl_t*) addr_;
	const struct sockaddr_in* addr_in = (const struct sockaddr_in*)&addr->addr;
	return ntohs(addr_in->sin_port);
}

isz lt_socket_send(const lt_socket_t sock[static 1], const void* data, usz size) {
	isz res = send(sock->fd, data, size, MSG_NOSIGNAL);
	if (res == 0)
		return -LT_ERR_CLOSED;
	if (res < 0)
		return -LAST_ERROR();
	return res;
}

isz lt_socket_recv(const lt_socket_t sock[static 1], void* data, usz size) {
	isz res = recv(sock->fd, data, size, 0);
	if (res == 0)
		return -LT_ERR_CLOSED;
	if (res < 0)
		return -LAST_ERROR();
	return res;
}

