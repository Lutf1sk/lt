#ifndef LT_HTTP_H
#define LT_HTTP_H 1

#include <lt/err.h>
#include <lt/fwd.h>

#define LT_HTTP_VERSION(major, minor) (((u16)major << 8) | (u16)minor)
#define LT_HTTP_1 LT_HTTP_VERSION(1, 0)
#define LT_HTTP_1_1 LT_HTTP_VERSION(1, 1)
#define LT_HTTP_2 LT_HTTP_VERSION(2, 0)
#define LT_HTTP_3 LT_HTTP_VERSION(3, 0)

#define LT_HTTP_VERSION_MINOR(ver) (ver & 0xFF)
#define LT_HTTP_VERSION_MAJOR(ver) ((u16)ver >> 8)

#define LT_FOR_EACH_HTTP_METHOD() \
	LT_HTTP_METHOD_OP(GET) \
	LT_HTTP_METHOD_OP(HEAD) \
	LT_HTTP_METHOD_OP(POST) \
	LT_HTTP_METHOD_OP(PUT) \
	LT_HTTP_METHOD_OP(DELETE) \
	LT_HTTP_METHOD_OP(CONNECT) \
	LT_HTTP_METHOD_OP(OPTIONS) \
	LT_HTTP_METHOD_OP(TRACE) \
	LT_HTTP_METHOD_OP(PATCH)

typedef
enum lt_http_method {
	LT_HTTP_INVALID_METHOD = 0,
#define LT_HTTP_METHOD_OP(x) LT_HTTP_##x,
	LT_FOR_EACH_HTTP_METHOD()
#undef LT_HTTP_METHOD_OP
} lt_http_method_t;

#define LT_HTTP_ENC_NONE 0
#define LT_HTTP_ENC_UNKNOWN 1
#define LT_HTTP_ENC_CHUNKED 2

typedef
struct lt_http_msg {
	lstr_t str;

	u16 version;
	u16 transfer_enc;

	u64 response_status_code;
	lstr_t response_status_msg;

	u64 request_method;
	lstr_t request_file;

	lstr_t* headers;
	lstr_t* header_vals;
	usz header_count;

	lstr_t body;
} lt_http_msg_t;

lstr_t lt_http_method_str(lt_http_method_t method);

lt_err_t lt_http_msg_create(lt_http_msg_t out_msg[static 1], lt_alloc_t alloc[static 1]);
void lt_http_msg_destroy(const lt_http_msg_t msg[static 1], lt_alloc_t alloc[static 1]);

lt_err_t lt_http_add_header(lt_http_msg_t msg[static 1], lstr_t key, lstr_t val);
lstr_t* lt_http_find_header(const lt_http_msg_t msg[static 1], lstr_t key);

lt_err_t lt_http_parse_request(lt_http_msg_t out_request[static 1], lt_read_fn_t callb, void* usr, lt_alloc_t alloc[static 1]);
lt_err_t lt_http_parse_response(lt_http_msg_t out_response[static 1], lt_read_fn_t callb, void* usr, lt_alloc_t alloc[static 1]);

lt_err_t lt_http_write_request(const lt_http_msg_t request[static 1], lt_write_fn_t callb, void* usr);
lt_err_t lt_http_write_response(const lt_http_msg_t response[static 1], lt_write_fn_t callb, void* usr);

#endif
