#include <lt/io.h>
#include <lt/mem.h>

isz lt_str_printuq_hex(char* str, usz n) {
	char buf[32];
	char* it = buf + sizeof(buf) - 1;

	for (;;) {
		usz rem = n & 0xF;
		*(it--) = rem >= 10 ? (rem - 10) + 'A' : rem + '0';
		n >>= 4;
		if (n == 0)
			break;
	}

	isz len = (buf + sizeof(buf)) - it;
	memcpy(str, it, len);
	return len;
}

isz lt_str_printuq(char* str, u64 n) {
	char buf[32];
	char* it = buf + sizeof(buf) - 1;

	for (;;) {
		*(it--) = (n % 10) + '0';
		n /= 10;
		if (n == 0)
			break;
	}
	++it;

	isz len = (buf + sizeof(buf)) - it;
	memcpy(str, it, len);

	return len;
}

isz lt_str_printiq(char* str, i64 n) {
	if (n < 0) {
		*(str++) = '-';
		n = -n;
	}
	return lt_str_printuq(str, n) + 1;
}

isz lt_str_printfq(char* str, f64 n) {
	char buf[32];
	char* it = buf + sizeof(buf) - 1;

	if (n < 0) {
		*(it++) = '-';
		n = -n;
	}

// 	for (;;) {
// 		*(it++) = '0';
// 	}

	isz len = (buf + sizeof(buf)) - it;
	memcpy(str, it, len);

	return len;
}

#include <stdarg.h>

isz lt_str_printf(char* str, char* fmt, ...) {
	va_list list;
	va_start(list, fmt);
	isz bytes = lt_str_vprintf(str, fmt, list);
	va_end(list);
	return bytes;
}

