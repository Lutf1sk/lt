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

#define wait_readable(state) \
	while (!socket_readable((state)->socket, 0)) { \
		if (time_ms() >= (state)->timeout_at_ms) { \
			throw(error, ERR_TIMED_OUT, "http connection timed out"); \
			return 0; \
		} \
		co_yield(0); \
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

		wait_readable(state);
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
		state->content_start = end + 4;
		return 1;
	}
}

b8 parse_http_headers(http_request_state* state, err* error) {
	u8* it = state->headers_start;
	for (;;) {
		if (it >= state->content_start)
			return 1;

		u8* line_start = it;
		for (;;) {
			if UNLIKELY (it + 1 >= state->content_start) {
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

b8 receive_http_content(task* t, http_request_state* state, err* error) {
	co_reenter(t);

	// ----- read raw content
	if (!(state->flags & HTTP_CHUNKED)) {
		state->content_end = state->content_start + state->content_length;
		if UNLIKELY (state->content_end > state->buffer_end) {
			throw(error, ERR_LIMIT_EXCEEDED, "http content exceeds maximum size");
			return 0;
		}

		while (state->buffer_it < state->content_end) {
			wait_readable(state);
			usz res = read_socket(state, state->buffer_it, state->content_end - state->buffer_it, error);
			if UNLIKELY (!res)
				return 0;
			state->buffer_it += res;
		}
		return 1;
	}

	// ----- read chunked content
	state->processed_it = state->content_start;
	while LIKELY (state->buffer_it < state->buffer_end) {
		u8* size_end = lssubstr(lsrange(state->processed_it, state->buffer_it), ls("\r\n"));
		if (!size_end) {
			// !! this can still end up receiving only a partial size if the packets align in just the right (wrong) way
			wait_readable(state);
			usz res = read_socket(state, state->buffer_it, state->buffer_end - state->buffer_it, error);
			if UNLIKELY (!res)
				return 0;
			state->buffer_it += res;
			size_end = lssubstr(lsrange(state->processed_it, state->buffer_it), ls("\r\n"));
		}

		if UNLIKELY (!size_end) {
			throw(error, ERR_PROTOCOL, "invalid chunk size");
			return 0;
		}

		ls size_str = lsrange(state->processed_it, size_end);
		size_end += 2;
		state->chunk_size = hexlstou(size_str, error);
		if (!state->chunk_size) {
			state->content_length = state->content_end - state->content_start;
			return 1;
		}
		if (state->processed_it + state->chunk_size + 2 > state->buffer_end)
			break;

		usz trail_bytes = state->buffer_it - size_end;
		memmove(state->processed_it, size_end, trail_bytes);
		state->buffer_it -= size_end - state->processed_it;
		state->processed_it += trail_bytes;
		if (state->processed_it < state->buffer_it)
			continue;

		state->content_end = state->buffer_it + state->chunk_size - trail_bytes + 2;
		while (state->buffer_it < state->content_end) {
			wait_readable(state);
			usz res = read_socket(state, state->buffer_it, state->content_end - state->buffer_it, error);
			if UNLIKELY (!res)
				return 0;
			state->buffer_it += res;
		}
		state->buffer_it -= 2;
		state->content_end = state->processed_it = state->buffer_it;
	}
	throw(error, ERR_LIMIT_EXCEEDED, "http content exceeds maximum length");
	return 0;
}

b8 receive_http_request_async(task* t, http_request_state* state, err* error) {
	co_reenter(t);

	if (!state->timeout_at_ms)
		state->timeout_at_ms = time_ms() + S_TO_MS(60);
	state->buffer_it = state->buffer_start;

	co_await(state->subtask_response = receive_http_header_data(co_subtask, state, error), 0);
	if UNLIKELY (!state->subtask_response)
		return 0;

	state->headers_start = lssubstr(lsrange(state->buffer_start, state->content_start), ls("\r\n"));
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

	if UNLIKELY (!parse_http_headers(state, error))
		return 0;

	co_await(state->subtask_response = receive_http_content(co_subtask, state, error), 0);
	return state->subtask_response;
}

b8 receive_http_response_async(task* t, http_request_state* state, err* error) {
	co_reenter(t);

	if (!state->timeout_at_ms)
		state->timeout_at_ms = time_ms() + S_TO_MS(60);
	state->buffer_it = state->buffer_start;

	co_await(state->subtask_response = receive_http_header_data(co_subtask, state, error), 0);
	if UNLIKELY (!state->subtask_response)
		return 0;

	state->headers_start = lssubstr(lsrange(state->buffer_start, state->content_start), ls("\r\n"));
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

	if UNLIKELY (!parse_http_headers(state, error))
		return 0;

	co_await(state->subtask_response = receive_http_content(co_subtask, state, error), 0);
	return state->subtask_response;
}

#endif // !ON_WASI

