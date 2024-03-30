#include <lt/http.h>
#include <lt/mem.h>
#include <lt/darr.h>
#include <lt/strstream.h>
#include <lt/internal.h>
#include <lt/str.h>
#include <lt/ctype.h>
#include <lt/io.h>

lstr_t lt_http_method_str(lt_http_method_t method) {
	switch (method) {
#define LT_HTTP_METHOD_OP(x) case LT_HTTP_##x: return CLSTR(#x);
	LT_FOR_EACH_HTTP_METHOD()
#undef LT_HTTP_METHOD_OP
	default: return CLSTR("INVALID_METHOD");
	}
}

lt_http_method_t lt_http_method(lstr_t str) {
#define LT_HTTP_METHOD_OP(x) if (lt_lseq_nocase(CLSTR(#x), str)) { return LT_HTTP_##x; }
	LT_FOR_EACH_HTTP_METHOD()
#undef LT_HTTP_METHOD_OP
	return LT_HTTP_INVALID_METHOD;
}

lt_err_t lt_http_msg_create(lt_http_msg_t* msg, lt_alloc_t* alloc) {
	*msg = (lt_http_msg_t){ .headers = lt_darr_create(lstr_t, 32, alloc) };
	if (!msg->headers) {
		return LT_ERR_OUT_OF_MEMORY;
	}
	return LT_SUCCESS;
}

void lt_http_msg_destroy(const lt_http_msg_t* msg, lt_alloc_t* alloc) {
	lt_darr_destroy(msg->headers);
	if (msg->body.str) {
		lt_mfree(alloc, msg->body.str);
	}
	if (msg->str.str) {
		lt_mfree(alloc, msg->str.str);
	}
}

lt_err_t lt_http_add_header(lt_http_msg_t* msg, lstr_t key, lstr_t val) {
	lt_darr_insert(msg->headers, msg->header_count++, &key, 1);
	lt_darr_push(msg->headers, val);
	msg->header_vals = msg->headers + msg->header_count;
	return LT_SUCCESS;
}

lstr_t* lt_http_find_header(const lt_http_msg_t* msg, lstr_t key) {
	for (usz i = 0; i < msg->header_count; ++i) {
		if (lt_lseq_nocase(msg->headers[i], key)) {
			return &msg->header_vals[i];
		}
	}
	return NULL;
}

#define X2CRLF (((u32)'\r' << 24) | ((u32)'\n' << 16) | ((u32)'\r' << 8) | (u32)'\n')
#define X1CRLF (((u32)'\r' << 8) | (u32)'\n')

#define HEADER_MAX_SIZE LT_MB(1)

static
lt_err_t read_headers(lstr_t* out_headers, lt_io_callback_t callb, void* usr, lt_alloc_t* alloc) {
	lt_err_t err;

	lt_strstream_t stream;
	if ((err = lt_strstream_create(&stream, alloc))) {
		return err;
	}

	// Read until "\r\n\r\n" is encountered
	u32 history = 0;
	for (;;) {
		isz res;
		if ((res = callb(usr, &history, 1)) < 0) {
			lt_strstream_destroy(&stream);
			return -res;
		}
		if ((res = lt_strstream_writec(&stream, history & 0xFF)) < 0) {
			lt_strstream_destroy(&stream);
			return -res;
		}
		if (history == X2CRLF) {
			break;
		}

		if (stream.str.len >= HEADER_MAX_SIZE) {
			return LT_ERR_OUT_OF_RANGE;
		}
		history <<= 8;
	}

	*out_headers = stream.str;
	return LT_SUCCESS;
}


static
lt_err_t parse_headers(lt_http_msg_t* msg, lstr_t header_data) {
	for (char* it = header_data.str, *end = it + header_data.len; it < end; it += 2) {
		char* line_start = it;
		for (;;) {
			if (it + 1 >= end) {
				return LT_ERR_INVALID_SYNTAX;
			}
			if (it[0] == '\r' && it[1] == '\n') {
				break;
			}
			++it;
		}
		lstr_t line = lt_lsfrom_range(line_start, it);
		if (!line.len) {
			break;
		}

		lstr_t key = lt_lssplit(line, ':');
		if (key.len == line.len) {
			return LT_ERR_INVALID_SYNTAX;
		}

		char* val_start = line.str + key.len + 1;
		lstr_t val = lt_lstrim(lt_lsfrom_range(val_start, it));
		lt_http_add_header(msg, key, val);
	}

	return LT_SUCCESS;
}

#define MAX_BODY_SIZE LT_MB(4)

static
lt_err_t read_content(lt_http_msg_t* msg, lt_io_callback_t callb, void* usr, lt_alloc_t* alloc) {
	lt_err_t err;

	lstr_t* ptransfer_enc = lt_http_find_header(msg, CLSTR("transfer-encoding"));
	u16 transfer_enc = LT_HTTP_ENC_NONE;
	if (ptransfer_enc) {
		if (lt_lseq_nocase(*ptransfer_enc, CLSTR("chunked"))) {
			transfer_enc = LT_HTTP_ENC_CHUNKED;
		}
		else {
			transfer_enc = LT_HTTP_ENC_UNKNOWN;
			lt_werrf("unsupported transfer encoding '%S'\n", *ptransfer_enc);
		}
	}

	lstr_t* pcontent_length = lt_http_find_header(msg, CLSTR("content-length"));
	if (pcontent_length) {
		usz content_length = 0;
		if ((err = lt_lstou(*pcontent_length, &content_length))) {
			return err;
		}

		if (content_length > MAX_BODY_SIZE) {
			return LT_ERR_OUT_OF_RANGE;
		}

		char* content = lt_malloc(alloc, content_length);
		if (!content) {
			return LT_ERR_OUT_OF_MEMORY;
		}

		isz res = callb(usr, content, content_length);
		if (res < 0) {
			lt_mfree(alloc, content);
			return -res;
		}

		msg->body = LSTR(content, content_length);
		return LT_SUCCESS;
	}

	if (transfer_enc != LT_HTTP_ENC_CHUNKED) {
		return LT_SUCCESS;
	}

	lt_werrf("'transfer-encoding: chunked' is disabled for the moment\n");
	return LT_ERR_UNSUPPORTED;

	lt_strstream_t body_stream;
	if ((err = lt_strstream_create(&body_stream, alloc))) {
		return err;
	}
	lt_strstream_t size_stream;
	if ((err = lt_strstream_create(&size_stream, alloc))) {
		goto chunked_err0;
	}

	for (;;) { // !! UNSAFE
		isz res;

		// Read until "\r\n" is encountered
		usz size_start_len = size_stream.str.len;
		u16 history = 0;
		for (;;) {
			if ((res = callb(usr, &history, 1)) < 0)
				fail_to(err = -res, chunked_err1);
			if ((res = lt_strstream_writec(&size_stream, history & 0xFF)) < 0)
				fail_to(err = -res, chunked_err1);
			if (history == X1CRLF)
				break;
			history <<= 8;
		}
		lstr_t size_str = LSTR(size_stream.str.str + size_start_len, size_stream.str.len - size_start_len - 2);
		size_str = lt_lstrim(size_str);
		if (lt_lseq(size_str, CLSTR("0")))
			break;

		usz size;
		if ((err = lt_lshextou(size_str, &size)))
			goto chunked_err1;

		lt_strstream_clear(&size_stream);

		void* tmp_chunk = lt_malloc(alloc, size);
		if (!tmp_chunk)
			fail_to(err = LT_ERR_OUT_OF_MEMORY, chunked_err1);
		if ((res = callb(usr, tmp_chunk, size)) < 0)
			fail_to(err = -res, chunked_err1);
		if ((res = lt_strstream_write(&body_stream, tmp_chunk, size)) < 0)
			fail_to(err = -res, chunked_err1);
		lt_mfree(alloc, tmp_chunk);

		char crlf_buf[2];
		if ((res = callb(usr, crlf_buf, 2)) < 0)
			fail_to(err = -res, chunked_err1);
		if (memcmp(crlf_buf, "\r\n", 2) != 0)
			fail_to(err = LT_ERR_INVALID_FORMAT, chunked_err1);

		continue;
	chunked_err1:	lt_strstream_destroy(&size_stream);
	chunked_err0:	lt_strstream_destroy(&body_stream);
					return err;
	}

	lt_strstream_destroy(&size_stream);
	msg->body = body_stream.str;
	return LT_SUCCESS;
}

lt_err_t parse_nonspace(char** it, char* end, lstr_t* out_str) {
	char* start = *it;
	if (*it >= end || **it == ' ') {
		return LT_ERR_INVALID_SYNTAX;
	}
	while (*it < end && **it != ' ') {
		++*it;
	}
	*out_str = lt_lsfrom_range(start, *it);
	return LT_SUCCESS;
}

lt_err_t skip_space(char** it, char* end) {
	if (*it >= end || **it != ' ') {
		return LT_ERR_INVALID_SYNTAX;
	}
	while (*it < end && **it == ' ') {
		++*it;
	}
	return LT_SUCCESS;
}

lt_err_t parse_version(char** it, char* end, u16* out_ver) {
	lstr_t http = CLSTR("HTTP/");
	if (!lt_lsprefix(lt_lsfrom_range(*it, end), http)) {
		return LT_ERR_INVALID_SYNTAX;
	}
	*it += http.len;

	// Parse version number
	u8 vmajor = 0, vminor = 0;
	if (*it >= end || !lt_is_digit(**it)) {
		return LT_ERR_INVALID_SYNTAX;
	}
	vmajor = *(*it)++ - '0';
	if (*it < end && **it == '.') {
		++*it;
		if (*it == end || !lt_is_digit(**it)) {
			return LT_ERR_INVALID_SYNTAX;
		}
		vminor = *(*it)++ - '0';
	}
	*out_ver = LT_HTTP_VERSION(vmajor, vminor);
	return LT_SUCCESS;
}

lt_err_t lt_http_parse_request(lt_http_msg_t* out_request, lt_io_callback_t callb, void* usr, lt_alloc_t* alloc) {
	lt_err_t err;

	lstr_t header_data;
	if ((err = read_headers(&header_data, callb, usr, alloc))) {
		return err;
	}

	char *it = header_data.str, *end = it + header_data.len;

	lstr_t method;
	if ((err = parse_nonspace(&it, end, &method)) || (err = skip_space(&it, end))) {
		goto err0;
	}

	lstr_t file;
	if ((err = parse_nonspace(&it, end, &file)) || (err = skip_space(&it, end))) {
		goto err0;
	}

	u16 version;
	if ((err = parse_version(&it, end, &version))) {
		goto err0;
	}

	it += 2;
	if (it >= end) {
		fail_to(err = LT_ERR_INVALID_SYNTAX, err0);
	}

	lt_http_msg_t msg;
	if ((err = lt_http_msg_create(&msg, alloc))) {
		goto err0;
	}

	msg.str = header_data;
	msg.version = version;
	msg.request_method = lt_http_method(method);
	msg.request_file = file;

	if ((err = parse_headers(&msg, lt_lsfrom_range(it, end)))) {
		goto err1;
	}

	if ((err = read_content(&msg, callb, usr, alloc))) {
		goto err1;
	}

	*out_request = msg;
	return LT_SUCCESS;

err1:	lt_http_msg_destroy(&msg, alloc);
		return err;
err0:	lt_mfree(header_data.str, alloc);
		return err;
}

lt_err_t lt_http_parse_response(lt_http_msg_t* out_response, lt_io_callback_t callb, void* usr, lt_alloc_t* alloc) {
	lt_err_t err;

	lstr_t header_data;
	if ((err = read_headers(&header_data, callb, usr, alloc))) {
		return err;
	}

	char* it = header_data.str, *end = it + header_data.len;

	u16 version;
	if ((err = parse_version(&it, end, &version)) || (err = skip_space(&it, end))) {
		goto err0;
	}

	lstr_t status_code_str;
	if ((err = parse_nonspace(&it, end, &status_code_str))) {
		goto err0;
	}
	u64 status_code;
	if ((err = lt_lstou(status_code_str, &status_code)) || (err = skip_space(&it, end))) {
		goto err0;
	}

	char* status_msg_start = it;
	for (;;) {
		if (it + 1 >= end) {
			fail_to(err = LT_ERR_INVALID_SYNTAX, err0);
		}
		if (it[0] == '\r' && it[1] == '\n') {
			break;
		}
		++it;
	}
	lstr_t status_msg = lt_lsfrom_range(status_msg_start, it);
	it += 2;

	lt_http_msg_t msg;
	if ((err = lt_http_msg_create(&msg, alloc))) {
		goto err0;
	}
	msg.str = header_data;
	msg.version = version;
	msg.response_status_code = status_code;
	msg.response_status_msg = status_msg;

	if ((err = parse_headers(&msg, lt_lsfrom_range(it, end)))) {
		goto err1;
	}

	if ((err = read_content(&msg, callb, usr, alloc))) {
		goto err1;
	}

	*out_response = msg;
	return LT_SUCCESS;

err1:	lt_http_msg_destroy(&msg, alloc);
		return err;
err0:	lt_mfree(header_data.str, alloc);
		return err;
}

static
lt_err_t write_common(const lt_http_msg_t* msg, lt_io_callback_t callb, void* usr) {
	for (usz i = 0; i < msg->header_count; ++i) {
		isz res = lt_io_printf(callb, usr, "%S: %S\r\n", msg->headers[i], msg->header_vals[i]);
		if (res < 0) {
			return -res;
		}
	}

	if (!msg->body.len) {
		return LT_SUCCESS;
	}

	isz res = lt_io_printf(callb, usr, "Content-Length: %uz\r\n\r\n%S", msg->body.len, msg->body);
	if (res < 0) {
		return -res;
	}
	return LT_SUCCESS;
}

lt_err_t lt_http_write_request(const lt_http_msg_t* request, lt_io_callback_t callb, void* usr) {
	u8 vmajor = LT_HTTP_VERSION_MAJOR(request->version);
	u8 vminor = LT_HTTP_VERSION_MINOR(request->version);
	isz res = lt_io_printf(callb, usr, "%S %S HTTP/%uw.%uw\r\n", vmajor, vminor, lt_http_method_str(request->request_method), request->request_file);
	if (res < 0) {
		return -res;
	}

	return write_common(request, callb, usr);
}

lt_err_t lt_http_write_response(const lt_http_msg_t* response, lt_io_callback_t callb, void* usr) {
	u8 vmajor = LT_HTTP_VERSION_MAJOR(response->version);
	u8 vminor = LT_HTTP_VERSION_MINOR(response->version);
	isz res = lt_io_printf(callb, usr, "HTTP/%uw.%uw %uq %S\r\n", vmajor, vminor, response->response_status_code, response->response_status_msg);
	if (res < 0) {
		return -res;
	}

	return write_common(response, callb, usr);
}
