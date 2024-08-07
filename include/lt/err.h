#ifndef LT_ERR_H
#define LT_ERR_H 1

#include <lt/lt.h>

#define LT_FOR_EACH_ERR(x) \
	LT_ERR_OP(UNKNOWN) \
	LT_ERR_OP(UNSUPPORTED) \
	LT_ERR_OP(NOT_IMPLEMENTED) \
	LT_ERR_OP(INVALID_FORMAT) \
	LT_ERR_OP(NOT_FOUND) \
	LT_ERR_OP(PERMISSION_DENIED) \
	LT_ERR_OP(CLOSED) \
	LT_ERR_OP(OUT_OF_MEMORY) \
	LT_ERR_OP(OUT_OF_RANGE) \
	LT_ERR_OP(INVALID_TYPE) \
	LT_ERR_OP(INVALID_SYNTAX) \
	LT_ERR_OP(OVERFLOW) \
	LT_ERR_OP(UNDERFLOW) \
	LT_ERR_OP(INTERRUPTED) \
	LT_ERR_OP(UNEXPECTED_EOF) \
	LT_ERR_OP(REDEFINED) \
	LT_ERR_OP(PATH_TOO_LONG) \
	LT_ERR_OP(IS_DIRECTORY) \
	LT_ERR_OP(IS_NOT_DIRECTORY) \
	LT_ERR_OP(IS_FILE) \
	LT_ERR_OP(EXISTS) \
	LT_ERR_OP(INVALID_HANDLE) \
	LT_ERR_OP(REFUSED) \
	LT_ERR_OP(ABORTED) \
	LT_ERR_OP(RESET) \
	LT_ERR_OP(INVALID_ADDRESS) \
	LT_ERR_OP(INVALID_ARGUMENT) \
	LT_ERR_OP(NOT_CONNECTED) \
	LT_ERR_OP(NOT_EMPTY) \
	LT_ERR_OP(IS_EMPTY) \
	LT_ERR_OP(BROKEN_PIPE) \
	LT_ERR_OP(TIMED_OUT) \
	LT_ERR_OP(END_OF_STREAM) \
	LT_ERR_OP(LIMIT_EXCEEDED) \
	LT_ERR_OP(IN_PROGRESS) \
	LT_ERR_OP(UNREACHABLE)

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

#if defined(LT_UNIX)
lt_err_t lt_errno_to_err(int e);
lt_err_t lt_errno(void);
#endif

#endif
