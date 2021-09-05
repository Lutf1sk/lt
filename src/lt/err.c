#include <lt/lt.h>
#include <lt/io.h>

#if defined(LT_UNIX)
#	include <errno.h>
#	define LAST_ERROR_STR() strerror(errno)

char* strerror(int err);
void LT_NORETURN exit(int code);

#elif defined(LT_WINDOWS)
#	define WIN32_LEAN_AND_MEAN 1
#	include <windows.h>
#	define LAST_ERROR_STR() strerror(GetLastError())
#endif

// strerror wrapper
char* lt_os_err_str(void) {
	return LAST_ERROR_STR();
}

// Fatal errors
void LT_NORETURN lt_ferr(lstr_t str) {
	lt_file_printf(lt_stderr, "FERR: %S", str);
	exit(1);
}

void LT_NORETURN lt_ferrf(char* fmt, ...) {
	lt_file_printls(lt_stderr, CLSTR("FERR: "));

	va_list list;
	va_start(list, fmt);
	lt_file_vprintf(lt_stderr, fmt, list);
	va_end(list);
	exit(1);
}

void LT_NORETURN lt_ferrb(lstr_t str) {
	lt_file_printf(lt_stderr, "FERR: %S", str);
	lt_backtrace(NULL);
	exit(1);
}

void LT_NORETURN lt_ferrbf(char* fmt, ...) {
	lt_file_printls(lt_stderr, CLSTR("FERR: "));

	va_list list;
	va_start(list, fmt);
	lt_file_vprintf(lt_stderr, fmt, list);
	va_end(list);

	lt_backtrace(NULL);
	exit(1);
}

// Warnings
void lt_werr(lstr_t str) {
	lt_file_printf(lt_stderr, "WARN: %S", str);
}

void lt_werrf(char* fmt, ...) {
	lt_file_printls(lt_stderr, CLSTR("WARN: "));

	va_list list;
	va_start(list, fmt);
	lt_file_vprintf(lt_stderr, fmt, list);
	va_end(list);
}

void lt_werrb(lstr_t str) {
	lt_file_printf(lt_stderr, "WARN: %S", str);
	lt_backtrace(NULL);
}

void lt_werrbf(char* fmt, ...) {
	lt_file_printls(lt_stderr, CLSTR("WARN: "));

	va_list list;
	va_start(list, fmt);
	lt_file_vprintf(lt_stderr, fmt, list);
	va_end(list);

	lt_backtrace(NULL);
}

