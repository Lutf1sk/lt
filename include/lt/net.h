#ifndef LT_NET_H
#define LT_NET_H 1

#include <lt/err.h>
#include <lt/fwd.h>

// socket.c

typedef
struct lt_socket {
	int fd;
} lt_socket_t;

typedef
enum lt_socktype {
	LT_SOCKTYPE_TCP,
	LT_SOCKTYPE_UDP,
} lt_socktype_t;

typedef
struct lt_sockaddr {
	char impl[128];
} lt_sockaddr_t;

lt_err_t lt_sockaddr_resolve(lstr_t addr, u16 port, lt_socktype_t type, lt_sockaddr_t out_addr_[static 1], lt_alloc_t alloc[static 1]);

lt_socket_t* lt_socket_create(lt_socktype_t type, lt_alloc_t alloc[static 1]);
void lt_socket_close(const lt_socket_t sock[static 1]);
void lt_socket_destroy(const lt_socket_t sock[static 1], lt_alloc_t alloc[static 1]);

lt_err_t lt_socket_connect(const lt_socket_t sock[static 1], const lt_sockaddr_t addr[static 1]);
lt_err_t lt_socket_server(const lt_socket_t sock[static 1], u16 port);
lt_socket_t* lt_socket_accept(const lt_socket_t sock[static 1], lt_sockaddr_t out_addr_[static 1], lt_alloc_t alloc[static 1]);

u32 lt_sockaddr_ipv4_addr(const lt_sockaddr_t addr_[static 1]);
u16 lt_sockaddr_ipv4_port(const lt_sockaddr_t addr_[static 1]);

isz lt_socket_send(const lt_socket_t sock[static 1], const void* data, usz size);
isz lt_socket_recv(const lt_socket_t sock[static 1], void* out_data, usz size);

// sockstream.c

typedef
struct lt_sockstream {
	u8* buffer;
	usz buffer_size;
	u8* it;
	usz bytes_avail;
	lt_socket_t* socket;
} lt_sockstream_t;

lt_err_t lt_sockstream_create(lt_sockstream_t out_stream[static 1], lt_socket_t sock[static 1], usz buffer_size, lt_alloc_t alloc[static 1]);
void lt_sockstream_destroy(const lt_sockstream_t stream[static 1], lt_alloc_t alloc[static 1]);

isz lt_sockstream_read(lt_sockstream_t stream[static 1], void* out_data, usz size);
isz lt_sockstream_read_fixed(lt_sockstream_t stream[static 1], void* out_data, usz size);

#endif
