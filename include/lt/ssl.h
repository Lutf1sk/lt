#ifndef LT_SSL_H
#define LT_SSL_H 1

#include <lt/err.h>
#include <lt/fwd.h>
#include <lt/net.h>

#define LT_SSL_CLIENT 0x1
#define LT_SSL_SERVER 0x2

lt_err_t lt_ssl_init(u32 flags);
void lt_ssl_terminate(u32 flags);

lt_ssl_connection_t* lt_ssl_connect(lt_socket_t socket[static 1], lstr_t sni_host);
void lt_ssl_connection_destroy(lt_ssl_connection_t* ssl);

lt_err_t lt_ssl_configure_server(lstr_t cert_path, lstr_t key_path, lstr_t cert_chain_path);

lt_ssl_connection_t* lt_ssl_accept(lt_socket_t socket[static 1]);

isz lt_ssl_send(lt_ssl_connection_t* ssl, const void* data, usz size);
isz lt_ssl_send_fixed(lt_ssl_connection_t* ssl, const void* data, usz size);
isz lt_ssl_recv(lt_ssl_connection_t* ssl, void* data, usz size);
isz lt_ssl_recv_fixed(lt_ssl_connection_t* ssl, void* data, usz size);

#endif
