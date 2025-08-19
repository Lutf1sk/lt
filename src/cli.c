#include <lt2/common.h>

#include <unistd.h>

static
isz write_out(void* usr, const void* data, usz size) {
	return write((int)(isz)usr, data, size);
}

usz lprintf(const char* fmt, ...) {
	va_list arg_list;
	va_start(arg_list, fmt);
	usz res = vlprintf((write_fn)write_out, (void*)STDOUT_FILENO, fmt, arg_list);
	va_end(arg_list);
	return res;
}

