#include <lt/err.h>
#include <lt/debug.h>

static
lstr_t strtab[LT_ERR_MAX] = {
	CLSTRI("SUCCESS"),
#define LT_ERR_OP(x) CLSTRI(#x),
	LT_FOR_EACH_ERR()
#undef LT_ERR_OP
};

lstr_t lt_err_str(lt_err_t err) {
	LT_ASSERT(err < LT_ERR_MAX);
	return strtab[err];
}

#if defined(LT_UNIX)

#include <errno.h>

lt_err_t lt_errno_to_err(int e) {
	switch (e) {
	case EACCES:		return LT_ERR_PERMISSION_DENIED;
	case EBADF:			return LT_ERR_INVALID_HANDLE;
	case ECONNABORTED:	return LT_ERR_ABORTED;
	case ECONNREFUSED:	return LT_ERR_REFUSED;
	case ECONNRESET:	return LT_ERR_RESET;
	case EEXIST:		return LT_ERR_EXISTS;
	case EFAULT:		return LT_ERR_INVALID_ADDRESS;
	case EINTR:			return LT_ERR_INTERRUPTED;
	case EINVAL:		return LT_ERR_INVALID_ARGUMENT;
	case EISDIR:		return LT_ERR_IS_DIRECTORY;
	case ENOENT:		return LT_ERR_NOT_FOUND;
	case ENOTCONN:		return LT_ERR_NOT_CONNECTED;
	case ENOTDIR:		return LT_ERR_IS_NOT_DIRECTORY;
	case ENOTEMPTY:		return LT_ERR_NOT_EMPTY;
	case ENOTSUP:		return LT_ERR_UNSUPPORTED;
	case EOVERFLOW:		return LT_ERR_OVERFLOW;
	case EPERM:			return LT_ERR_PERMISSION_DENIED;
	case EPIPE:			return LT_ERR_BROKEN_PIPE;
	case ETIMEDOUT:		return LT_ERR_TIMED_OUT;
	default:			return LT_ERR_UNKNOWN;
	}
}

LT_FLATTEN
lt_err_t lt_errno(void) {
	return lt_errno_to_err(errno);
}

#endif