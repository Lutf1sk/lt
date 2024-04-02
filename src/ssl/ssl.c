#include <lt/ssl.h>
#include <lt/debug.h>
#include <lt/io.h>

#ifdef LT_SSL

#include <openssl/ssl.h>
#include <openssl/err.h>

typedef
struct lt_socket {
	int fd;
} lt_socket_t;

static SSL_CTX* ssl_client_ctx = NULL;
static SSL_CTX* ssl_server_ctx = NULL;

static b8 library_initialized = 0;

static
void library_init(void) {
	if (library_initialized) {
		return;
	}
	OPENSSL_init_ssl(OPENSSL_INIT_NO_LOAD_SSL_STRINGS, NULL);
	library_initialized = 1;
}

lt_err_t lt_ssl_init(u32 flags) {
	if ((flags & LT_SSL_CLIENT) && !ssl_client_ctx) {
		ssl_client_ctx = SSL_CTX_new(TLS_client_method());
		if (!ssl_client_ctx) {
			return LT_ERR_UNKNOWN;
		}
	}

	if ((flags & LT_SSL_SERVER) && !ssl_server_ctx) {
		ssl_server_ctx = SSL_CTX_new(TLS_server_method());
		if (!ssl_server_ctx) {
			return LT_ERR_UNKNOWN;
		}
	}
	return LT_SUCCESS;
}

void lt_ssl_terminate(u32 flags) {
	if ((flags & LT_SSL_CLIENT) && ssl_client_ctx) {
		SSL_CTX_free(ssl_client_ctx);
		ssl_client_ctx = NULL;
	}
	if ((flags & LT_SSL_SERVER) && ssl_server_ctx) {
		SSL_CTX_free(ssl_server_ctx);
		ssl_server_ctx = NULL;
	}
}

lt_ssl_connection_t* lt_ssl_connect(lt_socket_t* socket, lstr_t sni_host) {
	SSL* ssl = SSL_new(ssl_client_ctx);
	if (!ssl)
		return NULL;
	SSL_set_fd(ssl, socket->fd);

	char cstr_host[512];
	if (sni_host.len >= sizeof(cstr_host))
		return NULL;

	if (sni_host.len) {
		memcpy(cstr_host, sni_host.str, sni_host.len);
		cstr_host[sni_host.len] = 0;
		if (SSL_set_tlsext_host_name(ssl, cstr_host) != 1)
			goto err0;
	}
	if (SSL_connect(ssl) != 1)
		goto err0;
	return (void*)ssl;

err0:	SSL_free(ssl);
		return NULL;
}

void lt_ssl_connection_shutdown(lt_ssl_connection_t* ssl) {
	SSL_shutdown((void*)ssl);
}

void lt_ssl_connection_destroy(lt_ssl_connection_t* ssl) {
	SSL_shutdown((void*)ssl);
	SSL_free((void*)ssl);
}

lt_err_t lt_ssl_configure_server(lstr_t cert_path, lstr_t key_path, lstr_t cert_chain_path) {
	if (cert_path.len > LT_PATH_MAX || key_path.len > LT_PATH_MAX) {
		return LT_ERR_PATH_TOO_LONG;
	}

	char cpath[LT_PATH_MAX + 1];
	memcpy(cpath, cert_path.str, cert_path.len);
	cpath[cert_path.len] = 0;
	if (SSL_CTX_use_certificate_file(ssl_server_ctx, cpath, SSL_FILETYPE_PEM) <= 0) {
		return LT_ERR_UNKNOWN;
	}

	memcpy(cpath, key_path.str, key_path.len);
	cpath[key_path.len] = 0;
	if (SSL_CTX_use_PrivateKey_file(ssl_server_ctx, cpath, SSL_FILETYPE_PEM) <= 0) {
		return LT_ERR_UNKNOWN;
	}

	memcpy(cpath, cert_chain_path.str, cert_chain_path.len);
	cpath[cert_chain_path.len] = 0;
	if (SSL_CTX_use_certificate_chain_file(ssl_server_ctx, cpath) <= 0) {
		return LT_ERR_UNKNOWN;
	}

	return LT_SUCCESS;
}

lt_ssl_connection_t* lt_ssl_accept(lt_socket_t* socket) {
	SSL* ssl = SSL_new(ssl_server_ctx);
	SSL_set_fd(ssl, socket->fd);

	if (SSL_accept(ssl) <= 0) {
		SSL_shutdown(ssl);
		SSL_free(ssl);
		return NULL;
	}
	return (void*)ssl;
}

isz lt_ssl_send(lt_ssl_connection_t* ssl, void* data, usz size) {
	isz res = SSL_write((void*)ssl, data, size);
	if (res == 0)
		return -LT_ERR_CLOSED;
	if (res < 0)
		return -LT_ERR_UNKNOWN;
	return res;
}

isz lt_ssl_send_fixed(lt_ssl_connection_t* ssl, void* data, usz size) {
	u8 *it = data, *end = it + size;
	while (it < end) {
		isz res = lt_ssl_send(ssl, it, end - it);
		if (res < 0)
			return res;
		it += res;
	}
	return size;
}

isz lt_ssl_recv(lt_ssl_connection_t* ssl, void* data, usz size) {
	isz res = SSL_read((void*)ssl, data, size);
	if (res == 0)
		return -LT_ERR_CLOSED;
	if (res < 0)
		return -LT_ERR_UNKNOWN;
	return res;
}

isz lt_ssl_recv_fixed(lt_ssl_connection_t* ssl, void* data, usz size) {
	u8 *it = data, *end = it + size;
	while (it < end) {
		isz res = lt_ssl_recv(ssl, it, end - it);
		if (res < 0)
			return res;
		it += res;
	}
	return size;
}

#endif
