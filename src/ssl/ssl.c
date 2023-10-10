#include <lt/ssl.h>
#include <lt/debug.h>

#ifdef LT_SSL

#include <openssl/ssl.h>
#include <openssl/err.h>

typedef
struct lt_socket {
	int fd;
} lt_socket_t;

static SSL_CTX* ssl_ctx = NULL;

lt_err_t lt_ssl_init(void) {
	SSL_library_init();
	SSL_load_error_strings();
	OpenSSL_add_all_algorithms();

	const SSL_METHOD* method = TLS_client_method();
	if (!method)
		return LT_ERR_UNKNOWN;

	ssl_ctx = SSL_CTX_new(method);
	if (!ssl_ctx)
		return LT_ERR_UNKNOWN;

	return LT_SUCCESS;
}

lt_ssl_connection_t* lt_ssl_connect(lt_socket_t* socket, lstr_t sni_host) {
	SSL* ssl = SSL_new(ssl_ctx);
	if (!ssl)
		return NULL;
	SSL_set_fd(ssl, socket->fd);

	char cstr_host[512];
	if (sni_host.len >= sizeof(cstr_host))
		return NULL;

	if (sni_host.str) {
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

void lt_ssl_connection_destroy(lt_ssl_connection_t* ssl) {
	SSL_free((void*)ssl);
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
