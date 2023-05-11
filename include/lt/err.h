#ifndef LT_ERR_H
#define LT_ERR_H 1

#include <lt/lt.h>

#define LT_FOR_EACH_ERR(x) \
	LT_ERR_OP(UNKNOWN) \
	LT_ERR_OP(UNSUPPORTED) \
	LT_ERR_OP(INVALID_FORMAT) \
	LT_ERR_OP(NOT_FOUND) \
	LT_ERR_OP(PERMISSION_DENIED) \
	LT_ERR_OP(CLOSED) \
	LT_ERR_OP(OUT_OF_MEMORY) \
	LT_ERR_OP(OUT_OF_RANGE) \
	LT_ERR_OP(INVALID_TYPE) \
	LT_ERR_OP(INVALID_SYNTAX) \
	LT_ERR_OP(OVERFLOW) \
	LT_ERR_OP(UNEXPECTED_EOF)

typedef
enum lt_errcode {
	LT_SUCCESS = 0,
#define LT_ERR_OP(x) LT_ERR_##x,
	LT_FOR_EACH_ERR()
#undef LT_ERR_OP
	LT_ERR_MAX,
} lt_errcode_t;

typedef u32 lt_err_t;

lstr_t lt_err_str(lt_err_t err);

#endif
