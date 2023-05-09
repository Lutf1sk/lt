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

	if (!lt_lstr_startswith(stream.str, CLSTR("HTTP/")))
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
	u32 status_code = lt_lstr_uint(LSTR(status_start, it - status_start));

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

	b8 transfer_enc_present = 0;
	b8 content_length_present = 0;
	usz content_length = 0;

	usz entry_count = 0;
	while (it < end) {
		char* line_start = it;
		while (*it != '\r' || *(it + 1) != '\n')
			++it;

		lstr_t line = LSTR(line_start, it - line_start);
		if (!line.len)
			break;

		usz key_len = lt_lstr_split(line, ':');
		if (key_len == line.len)
			fail_to(err = LT_ERR_INVALID_SYNTAX, err2);

		lstr_t key = LSTR(line.str, key_len);
		lstr_t val = LSTR(line.str + (key_len + 1), line.len  - (key_len + 1));
		val = lt_lstr_trim_left(val);

		if (lt_lstr_case_eq(key, CLSTR("transfer-encoding")))
			transfer_enc_present = 1;
		else if (lt_lstr_case_eq(key, CLSTR("content-length"))) {
			content_length = lt_lstr_uint(val);
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
	else if (transfer_enc_present) {
		lt_strstream_t stream;
		if ((err = lt_strstream_create(&stream, alloc)))
			goto err0;

		// Read until "\r\n\r\n" is encountered
		u32 history = 0;
		for (;;) {
			isz res;
			if ((res = callb(usr, &history, 1)) < 0) {
				lt_strstream_destroy(&stream);
				fail_to(err = -res, err2);
			}
			if ((res = lt_strstream_writec(&stream, history)) < 0) {
				lt_strstream_destroy(&stream);
				fail_to(err = -res, err2);
			}
			if (history == X2CRLF)
				break;
			history <<= 8;
		}

		body = stream.str;
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
