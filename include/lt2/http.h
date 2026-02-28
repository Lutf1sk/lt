#pragma once

#include <lt2/common.h>
#include <lt2/net.h>

#define HTTP_KEEP_ALIVE 1
#define HTTP_CHUNKED    2

typedef struct http_response {
	u8 ver_major;
	u8 ver_minor;

	u16 status_code;
	ls status_msg;
} http_response_t;

typedef struct http_request {
	u8 ver_major;
	u8 ver_minor;

	ls path;
	ls method;
} http_request_t;

typedef struct http_headers {
	u16 status_code;
	ls status_msg;

	ls str;

	usz count;
	ls* keys;
	ls* vals;
} http_headers_t;

typedef struct http_connection {
	tls_handle* tls;
	socket_handle socket;

	u64 timeout_at_ms;

	u8* strbuf;
	u8* strbuf_it;
	u8* strbuf_end;

	ls* header_keys;
	ls* header_vals;
	usz max_header_count;

	ringbuf_t rb;

	b8 keep_alive;
	b8 chunked;

	usz remain;
} http_connection_t;

ls  http_get_header (http_headers_t* headers, ls key, err* error);
ls* http_find_header(http_headers_t* headers, ls key);

http_response_t* http_recv_response_preamble(task* t, http_connection_t* conn, http_response_t* out, err* error);
http_request_t*  http_recv_request_preamble (task* t, http_connection_t* conn, http_request_t*  out, err* error);

http_headers_t* http_recv_headers(task* t, http_connection_t* conn, http_headers_t* out, err* error);

usz http_recv_content_chunk(task* t, http_connection_t* conn, void* data, usz size, err* error);

