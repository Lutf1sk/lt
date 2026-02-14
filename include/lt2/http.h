#pragma once

#include <lt2/common.h>
#include <lt2/net.h>

#define HTTP_KEEP_ALIVE 1
#define HTTP_CHUNKED    2

typedef struct http_request_state {
	socket_handle socket;
	tls_handle* tls;

	ls path;
	ls method;

	u8 version_major;
	u8 version_minor;
	u8 flags;
	u8 pad[1];

	u16 header_count;
	u16 max_header_count;
	ls* header_keys;
	ls* header_values;

	usz status_code;
	ls status_msg;

	u64 timeout_at_ms;

	u8* buffer_start;
	u8* buffer_end;
	u8* buffer_it;
	u8* processed_it;

	u8* headers_start;
	u8* headers_end;
	ls  trailing_content;

	usz chunk_size;
	u8  chunk_size_buf[24];
	ls  chunk_size_str;

	usz content_length;
	ls host;
	ls authorization;
} http_request_state;

ls  get_http_header (http_request_state* state, ls key, err* error);
ls* find_http_header(http_request_state* state, ls key);

b8 receive_http_response_async(task* t, http_request_state* state, err* error);
b8 receive_http_request_async(task* t, http_request_state* state, err* error);
usz receive_http_content_async(task* t, http_request_state* state, void* data, usz size, err* error);

