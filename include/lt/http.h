#ifndef LT_HTTP_H
#define LT_HTTP_H 1

#include <lt/err.h>
#include <lt/fwd.h>

#define LT_HTTP_VERSION(major, minor) (((u16)major << 8) | (u16)minor)
#define LT_HTTP_1 LT_HTTP_VERSION(1, 0)
#define LT_HTTP_1_1 LT_HTTP_VERSION(1, 1)
#define LT_HTTP_2 LT_HTTP_VERSION(2, 0)
#define LT_HTTP_3 LT_HTTP_VERSION(3, 0)

#define LT_HTTP_VERSION_MINOR(ver) (ver & 0xFFFF)
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

typedef
struct lt_http_response {
	lstr_t str;

	u16 version;
	u16 status_code;
	lstr_t status_msg;

	lt_darr(lstr_t) keys;
	lstr_t* vals;
	usz entry_count;

	lstr_t body;
} lt_http_response_t;

lstr_t lt_http_method_str(lt_http_method_t method);

lt_err_t lt_http_parse_response(lt_http_response_t* response, void* usr, lt_io_callback_t callb, lt_alloc_t* alloc);
void lt_http_response_destroy(lt_http_response_t* response, lt_alloc_t* alloc);

#endif
