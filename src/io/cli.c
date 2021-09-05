#include <lt/io.h>

isz lt_printc(char c) {
	return lt_file_write(lt_stdout, &c, 1);
}

isz lt_printls(lstr_t str) {
	return lt_file_write(lt_stdout, str.str, str.len);
}

isz lt_printf(char* fmt, ...) {
	va_list list;
	va_start(list, fmt);
	isz bytes = lt_file_vprintf(lt_stdout, fmt, list);
	va_end(list);
	return bytes;
}

