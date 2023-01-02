#ifdef LT_SSL

#include <lt/ssl.h>

#include <openssl/ssl.h>
#include <openssl/err.h>

typedef
struct lt_socket {
	int fd;
} lt_socket_t;

static SSL_CTX* ssl_ctx = NULL;

lt_err_t lt_ssl_init(void) {
	SSL_library_init();
	OpenSSL_add_all_algorithms();
	SSL_load_error_strings();

	ssl_ctx = SSL_CTX_new(TLS_client_method());
	if (!ssl_ctx)
		return LT_ERR_UNKNOWN;
	SSL_CTX_set_options(ssl_ctx, SSL_OP_NO_SSLv2|SSL_OP_NO_SSLv3);
	return LT_SUCCESS;
}

lt_ssl_connection_t* lt_ssl_connect(lt_socket_t* socket) {
	SSL* ssl = SSL_new(ssl_ctx);
	if (!ssl)
		return NULL;
	SSL_set_fd(ssl, socket->fd);

	char err_buf[512];

	if (SSL_connect(ssl) != 1) {
		ERR_print_errors_fp(stderr);
		SSL_free(ssl);
		return NULL;
	}
	return (void*)ssl;
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
