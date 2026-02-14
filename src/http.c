#include <lt2/http.h>
#include <lt2/async.h>
#include <lt2/time.h>
#include <lt2/str.h>

#ifndef ON_WASI
#	include <ctype.h>

ls get_http_header(http_request_state* state, ls key, err* error) {
	for (usz i = 0; i < state->header_count; ++i) {
		if (lseq_nocase(key, state->header_keys[i]))
			return state->header_values[i];
	}
	throw(error, ERR_NOT_FOUND, "http header not found");
	return ls("");
}

ls* find_http_header(http_request_state* state, ls key) {
	for (usz i = 0; i < state->header_count; ++i) {
		if (lseq_nocase(key, state->header_keys[i]))
			return &state->header_values[i];
	}
	return NULL;
}

#ifdef LT_OPENSSL
#	define read_socket(state, buf, size, error) \
		((state)->tls \
			? socket_receive_tls((state)->tls, buf, size, error) \
			: socket_receive((state)->socket,  buf, size, error))
#else
#	define read_socket(state, buf, size, error) \
	socket_receive((state)->socket, buf, size, error)
#endif

b8 receive_http_header_data(task* t, http_request_state* state, err* error) {
	co_reenter(t);

	for (;;) {
		if UNLIKELY (state->buffer_it >= state->buffer_end) {
			throw(error, ERR_LIMIT_EXCEEDED, "http headers exceed maximum length");
			return 0;
		}

		while (!poll_handle(state->socket, R, 0)) {
			if (time_ms() >= state->timeout_at_ms) {
				throw(error, ERR_TIMED_OUT, "http connection timed out");
				return 0;
			}
			co_set_awaiting(state->socket, R);
			co_yield(0);
		}

		usz res = read_socket(state, state->buffer_it, state->buffer_end - state->buffer_it, error);
		if UNLIKELY (!res)
			return 0;

		u8* search_from = state->buffer_it - 3;
		if (search_from < state->buffer_start)
			search_from = state->buffer_start;
		state->buffer_it += res;

		u8* end = lssubstr(lsrange(search_from, state->buffer_it), ls("\r\n\r\n"));
		if (!end)
			continue;
		state->headers_end = end + 4;
		state->trailing_content = lsrange(state->headers_end, state->buffer_it);
		return 1;
	}
}

b8 parse_http_headers(http_request_state* state, err* error) {
	u8* it = state->headers_start;
	for (;;) {
		if (it >= state->headers_end)
			return 1;

		u8* line_start = it;
		for (;;) {
			if UNLIKELY (it + 1 >= state->headers_end) {
				throw(error, ERR_BAD_SYNTAX, "expected '\\r\\n' after http header");
				return 0;
			}
			if (it[0] == '\r' && it[1] == '\n')
				break;
			++it;
		}
		ls line = lsrange(line_start, it);
		if (!line.size)
			return 1;
		it += 2;

		if UNLIKELY (state->header_count >= state->max_header_count) {
			throw(error, ERR_LIMIT_EXCEEDED, "maximum http header limit exceeded");
			return 0;
		}
		usz header_index = state->header_count++;

		ls key = lssplit(line, ':');
		if UNLIKELY (key.size == line.size) {
			throw(error, ERR_BAD_SYNTAX, "expected ':' before '\\r\\n' in http header");
			return 0;
		}
		state->header_keys[header_index] = key;
		u8* val_start = line.ptr + key.size + 1;
		ls value = lstrim(lsrange(val_start, it));
		state->header_values[header_index] = value;

		if (lseq_upper(key, ls("CONTENT-LENGTH")))
			state->content_length = lstou(value, error);
		else if (lseq_upper(key, ls("CONNECTION")) && lseq_upper(value, ls("KEEP-ALIVE")))
			state->flags |= HTTP_KEEP_ALIVE;
		else if (lseq_upper(key, ls("TRANSFER-ENCODING")) && lseq_upper(value, ls("CHUNKED")))
			state->flags |= HTTP_CHUNKED;
		else if (lseq_upper(key, ls("HOST")))
			state->host = value;
		else if (lseq_upper(key, ls("AUTHORIZATION")))
			state->authorization = value;
	}
}

static
usz receive_content(task* t, http_request_state* state, void* data, usz size, err* error) {
	co_reenter(t);

	state->chunk_size = state->content_length - state->trailing_content.size;
	// this can happen if more than one request is sent over a single connection
	if (state->content_length < state->trailing_content.size)
		state->chunk_size = 0;

	while (state->trailing_content.size) {
		if (size > state->trailing_content.size)
			size = state->trailing_content.size;
		memcpy(data, state->trailing_content.ptr, size);
		state->trailing_content = lsdrop(state->trailing_content, size);
		co_yield(size);
	}

	while (state->chunk_size) {
		while (!poll_handle(state->socket, R, 0)) {
			if (time_ms() >= state->timeout_at_ms) {
				throw(error, ERR_TIMED_OUT, "http connection timed out");
				return 0;
			}
			co_set_awaiting(state->socket, R);
			co_yield(0);
		}

		if (size > state->chunk_size)
			size = state->chunk_size;
		usz res = read_socket(state, data, size, error);
		if UNLIKELY (!res)
			return 0;
		state->chunk_size -= res;
		co_yield(res);
	}
	return 0;
}

static
usz receive_chunked(task* t, http_request_state* state, void* data, usz size, err* error) {
	co_reenter(t);

	throw(error, ERR_NOT_IMPLEMENTED, "streaming not implemented for chunked content encoding");
	return 0;

	state->chunk_size_str = lls(state->chunk_size_buf, 0);

	for (;;) {
		u8* size_end;
		for (;;) {
			size_end = lssubstr(state->chunk_size_str, ls("\r\n"));
			if (size_end)
				break;

			if (state->chunk_size_str.size >= sizeof(state->chunk_size_buf)) {
				throw(error, ERR_BAD_SYNTAX, "invalid content chunk size");
				return 0;
			}

			while (!poll_handle(state->socket, R, 0)) {
				if (time_ms() >= state->timeout_at_ms) {
					throw(error, ERR_TIMED_OUT, "http connection timed out");
					return 0;
				}
				co_set_awaiting(state->socket, R);
				co_yield(0);
			}

			u8* chunk_size_str_end = state->chunk_size_buf + state->chunk_size_str.size;
			usz buf_remain = sizeof(state->chunk_size_buf) - state->chunk_size_str.size;
			usz res = read_socket(state, chunk_size_str_end, buf_remain, error);
			if UNLIKELY (!res)
				return 0;
			state->chunk_size_str.size += res;
		}

		ls size_str = lsrange(state->processed_it, size_end);
		state->chunk_size = hexlstou(size_str, error);
		if (!state->chunk_size)
			return 0;

		// !! could split very small chunks incorrectly
		state->chunk_size_str = lsdrop(state->chunk_size_str, size_str.size + 2);
		while (state->chunk_size_str.size) {
			usz res = state->chunk_size_str.size;
			if (res > size)
				res = size;
			memcpy(data, state->chunk_size_str.ptr, res);
			state->chunk_size_str = lsdrop(state->chunk_size_str, res);
			co_yield(res);
		}

		while (state->chunk_size) {
			while (!poll_handle(state->socket, R, 0)) {
				if (time_ms() >= state->timeout_at_ms) {
					throw(error, ERR_TIMED_OUT, "http connection timed out");
					return 0;
				}
				co_set_awaiting(state->socket, R);
				co_yield(0);
			}

			usz res = read_socket(state, data, size, error);
			if UNLIKELY (!res)
				return 0;
			state->chunk_size -= res;
			co_yield(res);
		}
	}
}

usz receive_http_content_async(task* t, http_request_state* state, void* data, usz size, err* err) {
	if (state->flags & HTTP_CHUNKED)
		return receive_chunked(t, state, data, size, err);
	else
		return receive_content(t, state, data, size, err);
}

b8 receive_http_request_async(task* t, http_request_state* state, err* error) {
	co_reenter(t);

	if (!state->timeout_at_ms)
		state->timeout_at_ms = time_ms() + S_TO_MS(60);
	state->buffer_it = state->buffer_start;

	co_await(b8 success = receive_http_header_data(co_subtask, state, error), 0);
	if UNLIKELY (!success)
		return 0;

	state->headers_start = lssubstr(lsrange(state->buffer_start, state->headers_end), ls("\r\n"));
	if UNLIKELY (!state->headers_start) {
		throw(error, ERR_BAD_SYNTAX, "expected '\\r\\n' after http version");
		return 0;
	}
	state->headers_start += 2; // skip \r\n

	u8* it = state->buffer_start;
	while (it < state->headers_start && !isspace(*it))
		++it;
	state->method = lsrange(state->buffer_start, it++);

	u8* path_start = it;
	while (it < state->headers_start && !isspace(*it))
		++it;
	state->path = lsrange(path_start, it++);

	u8* version_start = it;
	while (it < state->headers_start && !isspace(*it))
		++it;
	ls version = lsrange(version_start, it++);

	if UNLIKELY (!lseq(version, ls("HTTP/1.1"))) {
		throw(error, ERR_UNSUPPORTED, "unsupported HTTP version");
		return 0;
	}
	state->version_major = 1;
	state->version_minor = 1;

	return parse_http_headers(state, error);
}

b8 receive_http_response_async(task* t, http_request_state* state, err* error) {
	co_reenter(t);

	if (!state->timeout_at_ms)
		state->timeout_at_ms = time_ms() + S_TO_MS(60);
	state->buffer_it = state->buffer_start;

	co_await(b8 success = receive_http_header_data(co_subtask, state, error), 0);
	if UNLIKELY (!success)
		return 0;

	state->headers_start = lssubstr(lsrange(state->buffer_start, state->headers_end), ls("\r\n"));
	if UNLIKELY (!state->headers_start) {
		throw(error, ERR_BAD_SYNTAX, "expected '\\r\\n' after http response status");
		return 0;
	}
	state->headers_start += 2; // skip \r\n

	u8* it = state->buffer_start;
	while (it < state->headers_start && !isspace(*it))
		++it;
	ls version = lsrange(state->buffer_start, it++);

	u8* status_code_start = it;
	while (it < state->headers_start && !isspace(*it))
		++it;
	state->status_code = lstou(lsrange(status_code_start, it++), error);

	state->status_msg = lsrange(it, state->headers_start - 2);

	if UNLIKELY (!lseq(version, ls("HTTP/1.1"))) {
		throw(error, ERR_UNSUPPORTED, "unsupported HTTP version");
		return 0;
	}
	state->version_major = 1;
	state->version_minor = 1;

	return parse_http_headers(state, error);
}

#endif // !ON_WASI

