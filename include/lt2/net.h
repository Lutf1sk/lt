#pragma once

#include <lt2/common.h>

typedef enum socket_addr_type {
	SOCKADDR_IPV4,
	SOCKADDR_IPV6,
} socket_addr_type;

typedef struct socket_addr {
	u8 type;
	u8 flags;
	u8 ip_addr[16];
} socket_addr;

socket_addr resolve_host(ls host, err* err);

typedef int socket_handle;

typedef enum socket_type {
	SOCKET_TCP   = 0x01,
	SOCKET_UDP   = 0x02,
	SOCKET_ASYNC = 0x80,
} socket_type;

typedef struct tls_handle  tls_handle;
typedef struct tls_context tls_context;

socket_handle socket_open(socket_type type, err* err);
void socket_close(socket_handle sock, err* err);
void socket_close_tls(tls_handle* tls);

tls_context* tls_load_certificates(ls cert_path, ls key_path, ls cert_chain_path, err* err);

b8 socket_connect_tcp(socket_handle sock, socket_addr* addr, u16 port, err* err);
tls_handle* socket_connect_tls(socket_handle sock, socket_addr* addr, u16 port, ls hostname, err* err);

b8 socket_bind(socket_handle sock, u16 port, err* err);

typedef struct tls_handshake_state {
	socket_handle socket;
	tls_context* context;
	u64 timeout_at_ms;

	tls_handle* handle;
} tls_handshake_state;

socket_handle socket_accept(socket_handle sock, socket_addr* out_addr, socket_type flags, err* err);
tls_handle* socket_accept_tls(socket_handle sock, tls_context* cx, err* err);
tls_handle* socket_accept_tls_async(task* t, tls_handshake_state* state, err* err);

usz socket_send(socket_handle sock, const void* data, usz size, err* err);
usz socket_receive(socket_handle sock, void* data, usz size, err* err);

usz socket_send_tls(tls_handle* ssl, const void* data, usz size, err* err);
usz socket_receive_tls(tls_handle* ssl, void* data, usz size, err* err);

