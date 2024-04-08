#include <lt/lt.h>
#include <lt/io.h>
#define LT_ANSI_SHORTEN_NAMES
#include <lt/ansi.h>
#include <lt/debug.h>

#if defined(LT_UNIX)
#	include <errno.h>
#	define LAST_ERROR_STR() strerror(errno)

const char* strerror(int err);
void LT_NORETURN exit(int code);

#elif defined(LT_WINDOWS)
#	define WIN32_LEAN_AND_MEAN 1
#	include <windows.h>
#	define LAST_ERROR_STR() strerror(GetLastError())
#endif

// strerror wrapper
const char* lt_os_err_str(void) {
	return LAST_ERROR_STR();
}

// Fatal errors
void LT_NORETURN lt_ferrf(const char* fmt, ...) {
	lt_fprintf(lt_stderr, FG_BRED"error"RESET": ");

	va_list list;
	va_start(list, fmt);
	lt_vfprintf(lt_stderr, fmt, list);
	va_end(list);
	exit(1);
}

void LT_NORETURN lt_ferrbf(const char* fmt, ...) {
	lt_fprintf(lt_stderr, FG_BRED"error"RESET": ");

	va_list list;
	va_start(list, fmt);
	lt_vfprintf(lt_stderr, fmt, list);
	va_end(list);

	lt_stack_trace(1);
	exit(1);
}

// Warnings
void lt_werrf(const char* fmt, ...) {
	lt_fprintf(lt_stderr, FG_BMAGENTA"warning"RESET": ");

	va_list list;
	va_start(list, fmt);
	lt_vfprintf(lt_stderr, fmt, list);
	va_end(list);
}

void lt_werrbf(const char* fmt, ...) {
	lt_fprintf(lt_stderr, FG_BMAGENTA"warning"RESET": ");

	va_list list;
	va_start(list, fmt);
	lt_vfprintf(lt_stderr, fmt, list);
	va_end(list);

	lt_stack_trace(1);
}

// Debug info
void lt_ierrf(const char* fmt, ...) {
	lt_fprintf(lt_stderr, FG_BCYAN"info"RESET": ");

	va_list list;
	va_start(list, fmt);
	lt_vfprintf(lt_stderr, fmt, list);
	va_end(list);
}

void lt_ierrbf(const char* fmt, ...) {
	lt_fprintf(lt_stderr, FG_BCYAN"info"RESET": ");

	va_list list;
	va_start(list, fmt);
	lt_vfprintf(lt_stderr, fmt, list);
	va_end(list);

	lt_stack_trace(1);
}

