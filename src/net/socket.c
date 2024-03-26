#include <lt/net.h>
#include <lt/lt.h>
#include <lt/str.h>
#include <lt/io.h>
#include <lt/mem.h>

#if defined(LT_UNIX)
#	include <sys/socket.h>
#	include <netdb.h>
#	include <unistd.h>

#	define SOCKET int
#	define INIT_IF_NECESSARY(x)
#elif defined(LT_WINDOWS)
#	define WIN32_LEAN_AND_MEAN 1
#	include <windows.h>
#	include <winsock2.h>
#	include <ws2tcpip.h>

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
struct lt_socket {
	SOCKET fd;
} lt_socket_t;

typedef
struct lt_sockaddr_impl {
	ulong addr_len;
	struct sockaddr addr;
} lt_sockaddr_impl_t;

static
int lt_socktype_to_native(lt_socktype_t type) {
	switch (type) {
	case LT_SOCKTYPE_TCP: return SOCK_STREAM;
	case LT_SOCKTYPE_UDP: return SOCK_DGRAM;
	default: return -1;
	}
}

lt_err_t lt_sockaddr_resolve(lstr_t addr, u16 port, lt_socktype_t type, lt_sockaddr_t* out_addr_, lt_alloc_t* alloc) {
	INIT_IF_NECESSARY(LT_ERR_UNKNOWN);
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

lt_socket_t* lt_socket_create(lt_socktype_t type, lt_alloc_t* alloc) {
	INIT_IF_NECESSARY(NULL);
	SOCKET fd = socket(AF_INET, lt_socktype_to_native(type), 0);
	if (fd < 0)
		return NULL;

	lt_socket_t* sock = lt_malloc(alloc, sizeof(lt_socket_t));
	sock->fd = fd;
	return sock;
}

void lt_socket_destroy(lt_socket_t* sock, lt_alloc_t* alloc) {
#if defined(LT_UNIX)
	close(sock->fd);
#elif defined(LT_WINDOWS)
	closesocket(sock->fd);
#endif
	lt_mfree(alloc, sock);
}

lt_err_t lt_socket_connect(lt_socket_t* sock, lt_sockaddr_t* addr_) {
	lt_sockaddr_impl_t* addr = (lt_sockaddr_impl_t*)addr_;
	if (connect(sock->fd, &addr->addr, addr->addr_len) < 0)
		return LT_ERR_UNKNOWN; // !!
	return LT_SUCCESS;
}

lt_err_t lt_socket_server(lt_socket_t* sock, u16 port) {
	int reuse_addr = 1;
	if (setsockopt(sock->fd, SOL_SOCKET, SO_REUSEADDR, (void*)&reuse_addr, sizeof(int)) < 0)
		return LT_ERR_UNKNOWN; // !!

	struct sockaddr_in server_addr;
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = INADDR_ANY;
	server_addr.sin_port = htons(port);

	if (bind(sock->fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0)
		return LT_ERR_UNKNOWN; // !!
	if (listen(sock->fd, 10) < 0)
		return LT_ERR_UNKNOWN; // !!
	return LT_SUCCESS;
}

lt_socket_t* lt_socket_accept(lt_socket_t* sock, lt_alloc_t* alloc) {
	struct sockaddr_in new_addr;
	usz new_size = sizeof(new_addr);
	SOCKET new_fd = accept(sock->fd, (struct sockaddr*)&new_addr, (socklen_t*)&new_size);
	if (new_fd < 0)
		return NULL;

	lt_socket_t* new_sock = lt_malloc(alloc, sizeof(lt_socket_t));
	if (!new_sock)
		return NULL;

	new_sock->fd = new_fd;
	return new_sock;
}

isz lt_socket_send(lt_socket_t* sock, void* data, usz size) {
	isz res = send(sock->fd, data, size, MSG_NOSIGNAL);
	if (res == 0)
		return -LT_ERR_CLOSED;
	if (res < 0)
		return -LT_ERR_UNKNOWN; // !!
	return res;
}

isz lt_socket_recv(lt_socket_t* sock, void* data, usz size) {
	isz res = recv(sock->fd, data, size, 0);
	if (res == 0)
		return -LT_ERR_CLOSED;
	if (res < 0)
		return -LT_ERR_UNKNOWN; // !!
	return res;
}

