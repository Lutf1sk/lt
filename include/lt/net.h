#ifndef LT_NET_H
#define LT_NET_H 1

#include <lt/err.h>
#include <lt/fwd.h>

// TODO: Rewrite socket abstractions

// socket.c
typedef struct lt_socket lt_socket_t;

typedef
enum lt_socktype {
	LT_SOCKTYPE_TCP,
	LT_SOCKTYPE_UDP,
} lt_socktype_t;

typedef
struct lt_sockaddr {
	char impl[128];
} lt_sockaddr_t;

lt_err_t lt_sockaddr_resolve(lstr_t addr, u16 port, lt_socktype_t type, lt_sockaddr_t* out_addr_, lt_alloc_t* alloc);

lt_socket_t* lt_socket_create(lt_socktype_t type, lt_alloc_t* alloc);
void lt_socket_destroy(lt_socket_t* sock, lt_alloc_t* alloc);

lt_err_t lt_socket_connect(lt_socket_t* sock, lt_sockaddr_t* addr);
lt_err_t lt_socket_server(lt_socket_t* sock, u16 port);
lt_socket_t* lt_socket_accept(lt_socket_t* sock, lt_alloc_t* alloc);

isz lt_socket_send(lt_socket_t* sock, void* data, usz size);
isz lt_socket_recv(lt_socket_t* sock, void* data, usz size);

// sockstream.c

typedef
struct lt_sockstream {
	u8* buffer;
	usz buffer_size;
	u8* it;
	usz bytes_avail;
	lt_socket_t* socket;
} lt_sockstream_t;

lt_err_t lt_sockstream_create(lt_sockstream_t* stream, lt_socket_t* sock, usz buffer_size, lt_alloc_t* alloc);
void lt_sockstream_destroy(lt_sockstream_t* stream, lt_alloc_t* alloc);

isz lt_sockstream_read(lt_sockstream_t* stream, void* data, usz size);
isz lt_sockstream_read_fixed(lt_sockstream_t* stream, void* data, usz size);

#endif
