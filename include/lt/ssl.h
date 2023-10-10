#ifndef LT_SSL_H
#define LT_SSL_H 1

#include <lt/err.h>
#include <lt/fwd.h>

typedef struct lt_ssl_connection lt_ssl_connection_t;

lt_err_t lt_ssl_init(void);

lt_ssl_connection_t* lt_ssl_connect(lt_socket_t* socket, lstr_t sni_host);
void lt_ssl_connection_destroy(lt_ssl_connection_t* ssl);

isz lt_ssl_send(lt_ssl_connection_t* ssl, void* data, usz size);
isz lt_ssl_send_fixed(lt_ssl_connection_t* ssl, void* data, usz size);
isz lt_ssl_recv(lt_ssl_connection_t* ssl, void* data, usz size);
isz lt_ssl_recv_fixed(lt_ssl_connection_t* ssl, void* data, usz size);

#endif
