#include <lt2/http.h>
#include <lt2/async.h>
#include <lt2/time.h>
#include <lt2/str.h>


ls http_get_header(http_headers_t* headers, ls key, err* error) {
	for (usz i = 0; i < headers->count; ++i) {
		if (lseq_nocase(key, headers->keys[i])) {
			return headers->vals[i];
		}
	}
	throw(error, ERR_NOT_FOUND, "missing required http header");
	return ls("");
}

ls* http_find_header(http_headers_t* headers, ls key) {
	for (usz i = 0; i < headers->count; ++i) {
		if (lseq_nocase(key, headers->keys[i])) {
			return &headers->vals[i];
		}
	}
	return NULL;
}

#ifndef ON_WASI

const ls proto_pfx = ls("HTTP/1.1");

INLINE
usz http_recv(http_connection_t* conn, void* data, usz size, err* error) {
	if (conn->tls)
		return socket_receive_tls(conn->tls, data, size, error);
	else
		return socket_receive(conn->socket, data, size, error);
}

http_response_t* http_recv_response_preamble(task* t, http_connection_t* conn, http_response_t* out, err* error) {
	co_reenter(t);

	usz len;
	for (;;) {
		u8* start = conn->rb.first;
		u8* end   = start + conn->rb.used;
		ls avail = lsrange(start, end);

		u8* preamble_end = lssubstr(avail, ls("\r\n"));

		// SHOULD skip empty lines
		if (preamble_end == start) {
			rb_skip(&conn->rb, 2);
			continue;
		}

		if (preamble_end) {
			len = preamble_end - start;
			break;
		}

		if (conn->rb.size == conn->rb.used) {
			throw(error, ERR_LIMIT_EXCEEDED, "http preamble length exceeds buffer size");
			return NULL;
		}

		for (;;) {
			if (time_ms() > conn->timeout_at_ms) {
				throw(error, ERR_TIMED_OUT, "http request timed out while receiving preamble");
				return NULL;
			}
			if (poll_handle(conn->socket, R, conn->timeout_at_ms))
				break;
			co_yield(NULL);
		}

		usz avail_size = rb_free_space(&conn->rb);
		u8* avail_from = rb_free_from(&conn->rb);
		usz read = http_recv(conn, avail_from, avail_size, error);
		if (!read)
			return NULL;
		conn->rb.used += read;

		co_yield(NULL);
	}

	ls preamble = lls(conn->rb.first, len);

	ls proto = lssplit(preamble, ' ');
	if (!lseq(proto, proto_pfx)) {
		throw(error, ERR_BAD_SYNTAX, "invalid protocol string");
		return NULL;
	}
	preamble = lstrim_left(lsdrop(preamble, proto.size));

	ls code_str = lssplit(preamble, ' ');
	u16 code = lstou(code_str, error);
	if (!code || code > 999) {
		throw(error, ERR_BAD_SYNTAX, "invalid status code");
		return NULL;
	}
	preamble = lstrim_left(lsdrop(preamble, code_str.size));

	ls msg = preamble;
	if (!msg.size) {
		throw(error, ERR_BAD_SYNTAX, "invalid status message");
		return NULL;
	}

	if (conn->strbuf_it + msg.size > conn->strbuf_end) {
		throw(error, ERR_NO_MEMORY, "not enough buffer space available for http status string");
		return NULL;
	}

	memcpy(conn->strbuf_it, msg.ptr, msg.size);
	out->status_msg = lls(conn->strbuf_it, msg.size);
	conn->strbuf_it += msg.size;

	out->status_code = code;

	out->ver_minor = 1;
	out->ver_major = 1;

	rb_skip(&conn->rb, len + 2);
	return out;
}

http_request_t* http_recv_request_preamble(task* t, http_connection_t* conn, http_request_t* out, err* error) {
	co_reenter(t);

	usz len;
	for (;;) {
		u8* start = conn->rb.first;
		u8* end   = start + conn->rb.used;
		ls avail = lsrange(start, end);

		u8* preamble_end = lssubstr(avail, ls("\r\n"));

		// SHOULD skip empty lines
		if (preamble_end == start) {
			rb_skip(&conn->rb, 2);
			continue;
		}

		if (preamble_end) {
			len = preamble_end - start;
			break;
		}

		if (conn->rb.size == conn->rb.used) {
			throw(error, ERR_LIMIT_EXCEEDED, "http preamble length exceeds buffer size");
			return NULL;
		}

		for (;;) {
			if (time_ms() > conn->timeout_at_ms) {
				throw(error, ERR_TIMED_OUT, "http request timed out while receiving preamble");
				return NULL;
			}
			if (poll_handle(conn->socket, R, conn->timeout_at_ms))
				break;
			co_yield(NULL);
		}

		usz avail_size = rb_free_space(&conn->rb);
		u8* avail_from = rb_free_from(&conn->rb);
		usz read = http_recv(conn, avail_from, avail_size, error);
		if (!read)
			return NULL;
		conn->rb.used += read;

		co_yield(NULL);
	}

	ls preamble = lls(conn->rb.first, len);

	ls method = lssplit(preamble, ' ');
	if (!method.size) {
		throw(error, ERR_BAD_SYNTAX, "invalid request method");
		return NULL;
	}
	preamble = lstrim_left(lsdrop(preamble, method.size));

	ls path = lssplit(preamble, ' ');
	if (!path.size) {
		throw(error, ERR_BAD_SYNTAX, "invalid request path");
		return NULL;
	}
	preamble = lstrim_left(lsdrop(preamble, path.size));

	if (!lseq(preamble, proto_pfx)) {
		throw(error, ERR_BAD_SYNTAX, "invalid protocol string");
		return NULL;
	}

	if (conn->strbuf_it + method.size + path.size > conn->strbuf_end) {
		throw(error, ERR_NO_MEMORY, "not enough buffer space available for http method and/or path strings");
		return NULL;
	}

	memcpy(conn->strbuf_it, method.ptr, method.size);
	out->method = lls(conn->strbuf_it, method.size);
	conn->strbuf_it += method.size;

	memcpy(conn->strbuf_it, path.ptr, path.size);
	out->path = lls(conn->strbuf_it, path.size);
	conn->strbuf_it += path.size;

	out->ver_minor = 1;
	out->ver_major = 1;

	rb_skip(&conn->rb, len + 2);
	return out;
}

http_headers_t* http_recv_headers(task* t, http_connection_t* conn, http_headers_t* out, err* error) {
	co_reenter(t);

	usz len;
	for (;;) {
		u8* start = conn->rb.first;
		u8* end   = start + conn->rb.used;
		ls avail = lsrange(start, end);

		u8* headers_end = lssubstr(avail, ls("\r\n\r\n"));

		if (headers_end) {
			len = headers_end - start + 2;
			break;
		}

		if (conn->rb.size == conn->rb.used) {
			throw(error, ERR_LIMIT_EXCEEDED, "total http header lengths exceed buffer size");
			return NULL;
		}

		for (;;) {
			if (time_ms() > conn->timeout_at_ms) {
				throw(error, ERR_TIMED_OUT, "http request timed out while receiving headers");
				return NULL;
			}
			if (poll_handle(conn->socket, R, conn->timeout_at_ms))
				break;
			co_yield(NULL);
		}

		usz avail_size = rb_free_space(&conn->rb);
		u8* avail_from = rb_free_from(&conn->rb);
		usz read = http_recv(conn, avail_from, avail_size, error);
		if (!read)
			return NULL;
		conn->rb.used += read;

		co_yield(NULL);
	}

	usz count = 0;

	b8 chunked = 0;
	usz content_length = 0;

	ls rem = lls(conn->rb.first, len);
	while (rem.size) {
		u8* line_end = lssubstr(rem, ls("\r\n"));
		if (!line_end) {
			throw(error, ERR_BAD_SYNTAX, "missing crlf after http header");
			return NULL;
		}
		ls line = lsrange(rem.ptr, line_end);

		ls key = lssplit(line, ':');
		if (key.size == line.size) {
			throw(error, ERR_BAD_SYNTAX, "missing ':' in http header");
			return NULL;
		}
		ls val = lstrim(lsdrop(line, key.size + 1));
		key = lstrim(key);

		if (count >= conn->max_header_count) {
			throw(error, ERR_LIMIT_EXCEEDED, "http header count exceeded max limit");
			return NULL;
		}

		conn->header_vals[count] = val;
		conn->header_keys[count] = key;
		++count;

		if (lseq_nocase(key, ls("Content-Length")))
			content_length = lstou(val, err_ignore); // !!
		else if (lseq_nocase(key, ls("Transfer-Encoding")) && lseq_nocase(val, ls("chunked")))
			chunked = 1;

		rem = lsdrop(rem, line.size + 2);
	}

	if (conn->strbuf_it + len > conn->strbuf_end) {
		throw(error, ERR_NO_MEMORY, "not enough buffer space available for header strings");
		return NULL;
	}

	memcpy(conn->strbuf_it, conn->rb.first, len);
	ls str = lls(conn->strbuf_it, len);
	conn->strbuf_it += len;
	rb_skip(&conn->rb, len + 2);
	out->str = str;

	out->count = count;
	out->keys  = conn->header_keys;
	out->vals  = conn->header_vals;

	conn->remain  = content_length;
	conn->chunked = chunked;

	return out;
}

usz http_recv_content_chunk(task* t, http_connection_t* conn, void* data, usz size, err* error) {
	co_reenter(t);

	if (conn->chunked) {
		throw(error, ERR_NOT_IMPLEMENTED, "chunked transfer encoding is not implemented");
		return 0;
	}

	while (conn->remain) {
		if (size > conn->remain)
			size = conn->remain;

		if (time_ms() > conn->timeout_at_ms) {
			throw(error, ERR_TIMED_OUT, "http request timed out while receiving content");
			return 0;
		}

		if (conn->rb.used < conn->rb.size && poll_handle(conn->socket, R, conn->timeout_at_ms)) {
			u8* avail_from = rb_free_from(&conn->rb);
			usz avail_size = rb_free_space(&conn->rb);

			usz res = http_recv(conn, avail_from, avail_size, error);
			if (!res) {
				return 0;
			}
			conn->rb.used += res;
		}

		usz res = rb_read(&conn->rb, data, size);
		conn->remain -= res;
		co_yield(res);
	}

	return 0;
}

#endif // !ON_WASI

