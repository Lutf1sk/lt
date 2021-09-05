#ifndef LT_NET_H
#define LT_NET_H 1

#include <lt/primitives.h>
#include <lt/fwd.h>

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

b8 lt_sockaddr_resolve(char* addr, char* port, lt_socktype_t type, lt_sockaddr_t* out_addr_);

lt_socket_t* lt_socket_create(lt_arena_t* arena, lt_socktype_t type);
void lt_socket_destroy(lt_socket_t* sock);

b8 lt_socket_connect(lt_socket_t* sock, lt_sockaddr_t* addr);
b8 lt_socket_server(lt_socket_t* sock, u16 port);
lt_socket_t* lt_socket_accept(lt_arena_t* arena, lt_socket_t* sock);

isz lt_socket_send(lt_socket_t* sock, void* data, usz size);
isz lt_socket_recv(lt_socket_t* sock, void* data, usz size);

#endif
