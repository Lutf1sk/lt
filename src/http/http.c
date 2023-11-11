#include <lt/http.h>
#include <lt/mem.h>
#include <lt/darr.h>
#include <lt/strstream.h>
#include <lt/internal.h>
#include <lt/str.h>
#include <lt/ctype.h>

lstr_t lt_http_method_str(lt_http_method_t method) {
	switch (method) {
#define LT_HTTP_METHOD_OP(x) case LT_HTTP_##x: return CLSTR(#x);
	LT_FOR_EACH_HTTP_METHOD()
#undef LT_HTTP_METHOD_OP
	default: return CLSTR("INVALID_METHOD");
	}
}

lt_err_t lt_http_parse_response(lt_http_response_t* response, lt_io_callback_t callb, void* usr, lt_alloc_t* alloc) {
	lt_err_t err;

	lt_strstream_t stream;
	if ((err = lt_strstream_create(&stream, alloc)))
		goto err0;

	#define X2CRLF (((u32)'\r' << 24) | ((u32)'\n' << 16) | ((u32)'\r' << 8) | (u32)'\n')
	#define X1CRLF (((u32)'\r' << 8) | (u32)'\n')

	// Read until "\r\n\r\n" is encountered
	u32 history = 0;
	for (;;) {
		isz res;
		if ((res = callb(usr, &history, 1)) < 0)
			fail_to(err = -res, err1);
		if ((res = lt_strstream_writec(&stream, history)) < 0)
			fail_to(err = -res, err1);
		if (history == X2CRLF)
			break;
		history <<= 8;
	}

	// Parse first line
	char *it = stream.str.str, *end = it + stream.str.len;

	if (!lt_lsprefix(stream.str, CLSTR("HTTP/")))
		return LT_ERR_INVALID_SYNTAX;
	it += 5;

	// Parse version number
	u8 vmajor = 0, vminor = 0;
	if (!lt_is_digit(*it))
		fail_to(err = LT_ERR_INVALID_SYNTAX, err1);
	vmajor = *it++ - '0';

	if (*it == '.') {
		++it;
		if (!lt_is_digit(*it))
			fail_to(err = LT_ERR_INVALID_SYNTAX, err1);
		vminor = *it++ - '0';
	}

	// Skip whitespace
	if (*it != ' ')
		fail_to(err = LT_ERR_INVALID_SYNTAX, err1);
	while (*it == ' ')
		++it;

	// Parse status code
	char* status_start = it;
	while (lt_is_digit(*it))
		++it;
	if (status_start == it)
		fail_to(err = LT_ERR_INVALID_SYNTAX, err1);
	u64 status_code;
	if (lt_lstou(LSTR(status_start, it - status_start), &status_code) != LT_SUCCESS || status_code > LT_U32_MAX)
		return LT_ERR_INVALID_SYNTAX;

	// Parse status message
	if (*it != ' ')
		fail_to(err = LT_ERR_INVALID_SYNTAX, err1);
	++it;

	char* status_msg_start = it;
	while (*it != '\r' || *(it + 1) != '\n')
		++it;
	lstr_t status_msg = LSTR(status_msg_start, it - status_msg_start);
	it += 2; // Skip "\r\n"

	// Parse entries
	lt_darr(lstr_t) entries = lt_darr_create(lstr_t, 16, alloc);
	if (!entries)
		fail_to(err = LT_ERR_OUT_OF_MEMORY, err1);


#define ENC_NONE 0
#define ENC_UNKNOWN 1
#define ENC_CHUNKED 2
	b8 transfer_enc = ENC_NONE;

	b8 content_length_present = 0;
	u64 content_length = 0;

	usz entry_count = 0;
	while (it < end) {
		char* line_start = it;
		while (*it != '\r' || *(it + 1) != '\n')
			++it;

		lstr_t line = LSTR(line_start, it - line_start);
		if (!line.len)
			break;

		lstr_t key = lt_lssplit(line, ':');
		if (key.len == line.len)
			fail_to(err = LT_ERR_INVALID_SYNTAX, err2);

		lstr_t val = LSTR(line.str + (key.len + 1), line.len  - (key.len + 1));
		val = lt_lstrim_left(val);

		if (lt_lseq_nocase(key, CLSTR("transfer-encoding"))) {
			transfer_enc = ENC_UNKNOWN;
			if (lt_lseq_nocase(val, CLSTR("chunked")))
				transfer_enc = ENC_CHUNKED;
			else
				lt_werrf("unsupported transfer encoding '%S'\n", val);
		}
		else if (lt_lseq_nocase(key, CLSTR("content-length"))) {
			if (lt_lstou(val, &content_length) != LT_SUCCESS)
				return LT_ERR_INVALID_SYNTAX;
			content_length_present = 1;
		}

		lt_darr_insert(entries, entry_count++, &key, 1);
		lt_darr_push(entries, val);

		it += 2; // Skip "\r\n"
	}

	lstr_t body = NLSTR();
	if (content_length_present) {
		if (!(body.str = lt_malloc(alloc, content_length)))
			fail_to(err = LT_ERR_OUT_OF_MEMORY, err2);
		isz res = callb(usr, body.str, content_length);
		if (res < 0) {
			lt_mfree(alloc, body.str);
			fail_to(err = -res, err2);
		}
		body.len = content_length;
	}
	else if (transfer_enc == ENC_CHUNKED) {
		lt_strstream_t body_stream;
		if ((err = lt_strstream_create(&body_stream, alloc)))
			goto err0;
		lt_strstream_t size_stream;
		if ((err = lt_strstream_create(&size_stream, alloc)))
			goto chunked_err0;

		for (;;) {
			isz res;

			// Read until "\r\n" is encountered
			usz size_start_len = size_stream.str.len;
			u16 history = 0;
			for (;;) {
				if ((res = callb(usr, &history, 1)) < 0)
					fail_to(err = -res, chunked_err1);
				if ((res = lt_strstream_writec(&size_stream, history)) < 0)
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
						goto err2;
		}

		lt_strstream_destroy(&size_stream);
		body = body_stream.str;
	}

	response->str = stream.str;
	response->version = LT_HTTP_VERSION(vmajor, vminor);
	response->status_code = status_code;
	response->status_msg = status_msg;
	response->keys = entries;
	response->vals = entries + entry_count;
	response->entry_count = entry_count;
	response->body = body;
	return LT_SUCCESS;

err2:	lt_darr_destroy(entries);
err1:	lt_strstream_destroy(&stream);
err0:	return err;
}

void lt_http_response_destroy(lt_http_response_t* response, lt_alloc_t* alloc) {
	if (response->body.str)
		lt_mfree(alloc, response->body.str);
	lt_darr_destroy(response->keys);
	lt_mfree(alloc, response->str.str);
}
