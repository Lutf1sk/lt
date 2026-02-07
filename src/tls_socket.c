#ifdef LT_OPENSSL

#include <lt2/net.h>
#include <lt2/time.h>
#include <lt2/async.h>

#include <openssl/ssl.h>
#include <openssl/err.h>

static SSL_CTX* ssl_client_ctx = NULL;

static b8 openssl_initialized = 0;

static
void initialize_openssl(void) {
	if (openssl_initialized)
		return;
	OPENSSL_init_ssl(OPENSSL_INIT_NO_LOAD_SSL_STRINGS, NULL);
	openssl_initialized = 1;
}

static
void create_tls_client_context(err* err) {
	if (ssl_client_ctx)
		return;
	ssl_client_ctx = SSL_CTX_new(TLS_client_method());
	if (!ssl_client_ctx)
		throw(err, ERR_ANY, "failed to create openssl client context");
}

tls_handle* socket_connect_tls(socket_handle sock, socket_addr* addr, u16 port, ls sni_host, err* err) {
	if (!ssl_client_ctx)
		create_tls_client_context(err);
	if (!ssl_client_ctx)
		return NULL;

	if (!socket_connect_tcp(sock, addr, port, err))
		return NULL;

	char cstr_host[512];
	if (sni_host.size >= sizeof(cstr_host)) {
		throw(err, ERR_BAD_ARGUMENT, "sni hostname exceeds maximum length of 512 bytes");
		return NULL;
	}

	SSL* ssl = SSL_new(ssl_client_ctx);
	if (!ssl) {
		throw(err, ERR_ANY, "SSL_new() failed");
		return NULL;
	}
	SSL_set_fd(ssl, sock);

	if (sni_host.size) {
		memcpy(cstr_host, sni_host.ptr, sni_host.size);
		cstr_host[sni_host.size] = 0;
		if (SSL_set_tlsext_host_name(ssl, cstr_host) != 1) {
			throw(err, ERR_ANY, "SSL_set_tlsext_host_name() failed");
			goto err0;
		}
	}

	if (SSL_connect(ssl) != 1) {
		throw(err, ERR_ANY, "tls connection failed");
		goto err0;
	}
	return (void*)ssl;

err0:	SSL_free(ssl);
		return NULL;
}

void socket_close_tls(tls_handle* tls) {
	SSL_free((void*)tls);
}

tls_context* tls_load_certificates(ls cert_path, ls key_path, ls cert_chain_path, err* err) {
	if (cert_path.size > PATH_MAX || key_path.size > PATH_MAX || cert_chain_path.size > PATH_MAX) {
		throw(err, ERR_ANY, "tls certificate/key/chain path is too long");
		return NULL;
	}

	SSL_CTX* cx = SSL_CTX_new(TLS_server_method());
	if (!cx) {
		throw(err, ERR_ANY, "failed to create openssl server context");
		return NULL;
	}

	char cpath[PATH_MAX + 1];

	memcpy(cpath, cert_path.ptr, cert_path.size);
	cpath[cert_path.size] = 0;
	if (SSL_CTX_use_certificate_file(cx, cpath, SSL_FILETYPE_PEM) <= 0) {
		throw(err, ERR_ANY, "failed to load tls certificate");
		goto err0;
	}

	memcpy(cpath, key_path.ptr, key_path.size);
	cpath[key_path.size] = 0;
	if (SSL_CTX_use_PrivateKey_file(cx, cpath, SSL_FILETYPE_PEM) <= 0) {
		throw(err, ERR_ANY, "failed to load tls private key");
		goto err0;
	}

	if (!cert_chain_path.size)
		return (void*)cx;

	memcpy(cpath, cert_chain_path.ptr, cert_chain_path.size);
	cpath[cert_chain_path.size] = 0;
	if (SSL_CTX_use_certificate_chain_file(cx, cpath) <= 0) {
		throw(err, ERR_ANY, "failed to load tls certificate chain");
		goto err0;
	}
	return (void*)cx;

err0:
	SSL_CTX_free(cx);
	return NULL;
}

tls_handle* socket_accept_tls(socket_handle sock, tls_context* cx, err* err) {
	SSL* ssl = SSL_new((void*)cx);
	SSL_set_fd(ssl, sock);

	int ret = SSL_accept(ssl);
	if (ret >= 0)
		return (void*)ssl;
	throw(err, ERR_ANY, "failed to accept tls connection");
	SSL_free(ssl);
	return NULL;
}

tls_handle* socket_accept_tls_async(task* t, tls_handshake_state* state, err* err) {
	co_reenter(t);

	if (!state->timeout_at_ms)
		state->timeout_at_ms = time_ms() + S_TO_MS(60);

	state->handle = (void*)SSL_new((void*)state->context);
	SSL_set_fd((void*)state->handle, state->socket);

	for (;;) {
		int ret = SSL_accept((void*)state->handle);
		if (ret >= 0)
			return state->handle;

		int ssl_error = SSL_get_error((void*)state->handle, ret);
		if (ssl_error != SSL_ERROR_WANT_READ && ssl_error != SSL_ERROR_WANT_WRITE) {
			throw(err, ERR_ANY, "failed to accept tls connection");
			SSL_free((void*)state->handle);
			return NULL;
		}
		if (time_ms() >= state->timeout_at_ms) {
			throw(err, ERR_TIMED_OUT, "tls handshake timed out");
			SSL_free((void*)state->handle);
			return NULL;
		}
		co_yield(NULL);
	}
}

usz socket_send_tls(tls_handle* ssl, const void* data, usz size, err* err) {
	isz res = SSL_write((void*)ssl, data, size);
	if (!res && size) {
		throw(err, ERR_CLOSED, "cannot send to a closed tls connection");
		return 0;
	}
	if (res < 0) {
		throw(err, ERR_ANY, "SSL_write() failed");
		return 0;
	}
	return res;
}

usz socket_receive_tls(tls_handle* ssl, void* data, usz size, err* err) {
	isz res = SSL_read((void*)ssl, data, size);
	if (!res && size) {
		throw(err, ERR_CLOSED, "cannot receive from a closed tls connection");
		return 0;
	}
	if (res < 0) {
		throw(err, ERR_ANY, "SSL_read() failed");
		return 0;
	}
	return res;
}

#endif

