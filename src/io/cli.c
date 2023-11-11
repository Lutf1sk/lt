#include <lt/io.h>

isz lt_printf(char* fmt, ...) {
	va_list list;
	va_start(list, fmt);
	isz bytes = lt_vfprintf(lt_stdout, fmt, list);
	va_end(list);
	return bytes;
}
