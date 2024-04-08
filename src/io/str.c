#include <lt/io.h>
#include <lt/mem.h>

isz lt_write_str(char* str[static 1], const void* data, usz len) {
	memcpy(*str, data, len);
	*str += len;
	return len;
}

isz lt_vsprintf(char* str, const char* fmt, va_list argl) {
	char* it = str;
	return lt_io_vprintf((lt_write_fn_t)lt_write_str, &it, fmt, argl);
}

isz lt_sprintf(char* str, const char* fmt, ...) {
	char* it = str;

	va_list argl;
	va_start(argl, fmt);
	isz bytes = lt_io_vprintf((lt_write_fn_t)lt_write_str, &it, fmt, argl);
	va_end(argl);
	return bytes;
}
