#include <lt2/common.h>
#include <lt2/log.h>

#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>

#define err_store ((struct err*)3)

void throw_errno_val(err* err, int posix_err) {
	u32 code = ERR_ANY;
	switch (posix_err) {
	case 0:            code = SUCCESS; break;
	case EACCES:       code = ERR_DENIED; break;
	case EBADF:        code = ERR_BAD_HANDLE; break;
	case ECONNABORTED: code = ERR_ABORTED; break;
	case ECONNREFUSED: code = ERR_REFUSED; break;
	case ECONNRESET:   code = ERR_RESET; break;
	case EEXIST:       code = ERR_EXISTS; break;
	case EFAULT:       code = ERR_BAD_ADDRESS; break;
	case EINTR:        code = ERR_INTERRUPTED; break;
	case EINVAL:       code = ERR_BAD_ARGUMENT; break;
	case EISDIR:       code = ERR_IS_DIRECTORY; break;
	case ENOENT:       code = ERR_NOT_FOUND; break;
	case ENOTCONN:     code = ERR_NOT_CONNECTED; break;
	case ENOTDIR:      code = ERR_NOT_DIRECTORY; break;
	case ENOTEMPTY:    code = ERR_NOT_EMPTY; break;
	case ENOTSUP:      code = ERR_UNSUPPORTED; break;
	case EOVERFLOW:    code = ERR_OVERFLOW; break;
	case EPERM:        code = ERR_DENIED; break;
	case EPIPE:        code = ERR_BROKEN_PIPE; break;
	case ETIMEDOUT:    code = ERR_TIMED_OUT; break;
	case EINPROGRESS:  code = ERR_RUNNING; break;
	case EAGAIN:       code = ERR_RUNNING; break;
	case ENETUNREACH:  code = ERR_UNREACHABLE; break;
	case EHOSTUNREACH: code = ERR_UNREACHABLE; break;
	}
	throw(err, code, strerror(posix_err));
}

void throw_errno(err* err) {
	throw_errno_val(err, errno);
}

void throw(err* err, u8 code, const char* fmt, ...) {
	if (err == err_ignore)
		return;

	if (err >= err_store) {
		*err = err(code);
		return;
	}

	if (err == err_fail) {
		va_list args;
		va_start(args, fmt);
		vlogf(NULL, LOG_ERR, fmt, args);
		va_end(args);
		exit(1);
	}
	else if (err == err_warn) {
		va_list args;
		va_start(args, fmt);
		vlogf(NULL, LOG_WARN, fmt, args);
		va_end(args);
	}
}

