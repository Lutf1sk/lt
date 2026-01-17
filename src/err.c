#include <lt2/common.h>

#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>

#define err_store ((struct err*)3)

static
isz write_out(void* usr, const void* data, usz size) {
	return write((int)(isz)usr, data, size);
}

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
		lfwrite(STDERR_FILENO, "\x1b[91merror\x1b[0m: ", 17, err_ignore);

		va_list arg_list;
		va_start(arg_list, fmt);
		vlprintf(write_out, (void*)STDERR_FILENO, fmt, arg_list);
		va_end(arg_list);

		lfwrite(STDERR_FILENO, "\n", 1, err_ignore);
		exit(1);
	}
	else if (err == err_warn) {
		lfwrite(STDERR_FILENO, "\x1b[95mwarning\x1b[0m: ", 19, err_ignore);

		va_list arg_list;
		va_start(arg_list, fmt);
		vlprintf(write_out, (void*)STDERR_FILENO, fmt, arg_list);
		va_end(arg_list);

		lfwrite(STDERR_FILENO, "\n", 1, err_ignore);
	}
}

