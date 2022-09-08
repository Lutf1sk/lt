#include <lt/io.h>
#include <lt/mem.h>

static
isz lt_str_io_callb(char* str, void* data, usz len) {
	memcpy(str, data, len);
	return len;
}

isz lt_str_vprintf(char* str, char* fmt, va_list argl) {
	return lt_io_vprintf((lt_io_callback_t)lt_str_io_callb, str, fmt, argl);
}

isz lt_str_printf(char* str, char* fmt, ...) {
	va_list argl;
	va_start(argl, fmt);
	isz bytes = lt_io_vprintf((lt_io_callback_t)lt_str_io_callb, str, fmt, argl);
	va_end(argl);
	return bytes;
}

