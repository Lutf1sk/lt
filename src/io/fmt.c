#include <lt/io.h>
#include <lt/mem.h>
#include <lt/str.h>
#include <lt/math.h>
#include <lt/ctype.h>

#define BUFSZ 1024

static LT_FLATTEN
isz buffered_write(lt_write_fn_t callb, void* usr, char** it, char* end, lstr_t data) {
	isz written = 0;
	if (*it + data.len > end) {
		char* start = end - BUFSZ;
		usz buffered = *it - start;
		*it = start;
		written += callb(usr, start, buffered);
	}

	if (data.len >= BUFSZ) {
		written += callb(usr, data.str, data.len);
	}
	else {
		*it += lt_mcopy(*it, data.str, data.len);
	}
	return written;
}

#define REP16(a...) a a a a a a a a a a a a a a a a
#define REP256(a...) REP16(a) REP16(a) REP16(a) REP16(a) REP16(a) REP16(a) REP16(a) REP16(a) REP16(a) REP16(a) REP16(a) REP16(a) REP16(a) REP16(a) REP16(a) REP16(a)

#define MAX_PAD_W 256
static char padding_chars[MAX_PAD_W] = { REP256(' ',) };

static LT_INLINE
isz pad_to(lt_write_fn_t callb, void* usr, char** it, char* end, usz col_w, usz content_w) {
	usz padding_w = col_w - content_w;
	return buffered_write(callb, usr, it, end, LSTR(padding_chars, padding_w));
}

#define NUM_MAX 64

isz lt_io_dummy_write(void* usr, const void* data, usz len) {
	return len;
}

isz lt_io_dummy_read(void* usr, void* data, usz len) {
	return len;
}

static LT_FLATTEN LT_INLINE
isz lt_io_printuq(lt_write_fn_t callb, void* usr, char** buf_it, char* buf_end, u64 n, usz leftpad) {
	char buf[NUM_MAX], *end = buf + sizeof(buf), *it = end - 1;

	// Fill buffer backwards with the remainder of n/10
	while (n >= 10) {
		*it-- = n % 10 + '0';
		n /= 10;
	}
	*it = n + '0';

	usz len = end - it;
	isz written = pad_to(callb, usr, buf_it, buf_end, leftpad, len);
	return written + buffered_write(callb, usr, buf_it, buf_end, LSTR(buf, len));
}

static LT_FLATTEN LT_INLINE
isz lt_io_printiq(lt_write_fn_t callb, void* usr, char** buf_it, char* buf_end, i64 n, usz leftpad) {
	char buf[NUM_MAX], *end = buf + sizeof(buf), *it = end - 1;

	// Handle signed values
	b8 sign = 0;
	if (n < 0) {
		n = -n;
		sign = 1;
	}

	// Fill buffer backwards with the remainder of n/10
	while (n >= 10) {
		*it-- = n % 10 + '0';
		n /= 10;
	}
	*it = n + '0';

	if (sign) {
		*--it = '-';
	}

	usz len = end - it;
	isz written = pad_to(callb, usr, buf_it, buf_end, leftpad, len);
	return written + buffered_write(callb, usr, buf_it, buf_end, LSTR(buf, len));
}

static char io_hex_char[16] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F' };

static LT_FLATTEN LT_INLINE
isz lt_io_printhq(lt_write_fn_t callb, void* usr, char** buf_it, char* buf_end, u64 n, usz leftpad) {
	char buf[NUM_MAX], *end = buf + sizeof(buf), *it = end - 1;

	// Fill buffer backwards with the lowest nibble of n
	while (n > 0x0F) {
		*it-- = io_hex_char[n & 0x0F];
		n >>= 4;
	}
	*it = io_hex_char[n];

	usz len = end - it;
	isz written = pad_to(callb, usr, buf_it, buf_end, leftpad, len);
	return written + buffered_write(callb, usr, buf_it, buf_end, LSTR(buf, len));
}

static LT_FLATTEN LT_INLINE
isz lt_io_printbq(lt_write_fn_t callb, void* usr, char** buf_it, char* buf_end, u64 n, usz leftpad) {
	char buf[NUM_MAX], *end = buf + sizeof(buf), *it = end - 1;

	while (n > 1) {
		*it-- = '0' + (n & 1);
		n >>= 1;
	}
	*it = '0' + (n & 1);

	usz len = end - it;
	isz written = pad_to(callb, usr, buf_it, buf_end, leftpad, len);
	return buffered_write(callb, usr, buf_it, buf_end, LSTR(buf, len));
}

static LT_FLATTEN LT_INLINE
isz lt_io_printmq(lt_write_fn_t callb, void* usr, char** buf_it, char* buf_end, u64 n, usz pad) {
	char buf[NUM_MAX] = "B";
	usz len = 1;

	isz res, size = 0;

	if LT_UNLIKELY (n >= LT_GB(1)) {
		memcpy(buf, ".0GiB", 5);
		buf[1] += (n % LT_GB(1)) / (LT_GB(1) / 10);
		len = 5;

		n /= LT_GB(1);
	}
	else if LT_UNLIKELY (n >= LT_MB(1)) {
		memcpy(buf, ".0MiB", 5);
		buf[1] += (n % LT_MB(1)) / (LT_MB(1) / 10);
		len = 5;

		n /= LT_MB(1);
	}

	else if LT_UNLIKELY (n >= LT_KB(1)) {
		memcpy(buf, ".0KiB", 5);
		buf[1] += (n % LT_KB(1)) / (LT_KB(1) / 10);
		len = 5;

		n /= LT_KB(1);
	}

	if (buf[1] == ':') { // !! this is a really dumb workaround for rounding errors, should be improved
		buf[1] = '9';
	}

	if LT_UNLIKELY ((res = lt_io_printuq(callb, usr, buf_it, buf_end, n, pad)) < 0) {
		return res;
	}
	size += res;

	if LT_UNLIKELY ((res = buffered_write(callb, usr, buf_it, buf_end, LSTR(buf, len))) < 0) {
		return res;
	}
	return size + res;
}

static LT_INLINE
isz lt_io_printfq(lt_write_fn_t callb, void* usr, char** buf_it, char* buf_end, f64 n, usz leftpad) {
	return -LT_ERR_UNSUPPORTED;
}

isz lt_io_vprintf(lt_write_fn_t callb, void* usr, const char* fmt, va_list argl) {
	char buf[BUFSZ], *buf_it = buf, *buf_end = buf_it + BUFSZ;

	isz written = 0;

	char c;
	while ((c = *fmt) != 0) {
		const char* substr_start = fmt;
		if (c != '%') {
			while ((c = *fmt) != 0 && c != '%') {
				++fmt;
			}

			written += buffered_write(callb, usr, &buf_it, buf_end, lt_lsfrom_range(substr_start, fmt));
			continue;
		}

		int leftpad = 0;
		while (lt_is_digit(c = *++fmt)) {
			leftpad *= 10;
			leftpad += c - '0';
		}

		switch (c) {
		case 0: --fmt; break;

		case '_':
			written += pad_to(callb, usr, &buf_it, buf_end, leftpad, 0);
			break;

		case '%':
			written += pad_to(callb, usr, &buf_it, buf_end, leftpad, 1);
			written += buffered_write(callb, usr, &buf_it, buf_end, CLSTR("%"));
			break;

		case 'c': {
			char c = va_arg(argl, int);
			written += pad_to(callb, usr, &buf_it, buf_end, leftpad, 1);
			written += buffered_write(callb, usr, &buf_it, buf_end, LSTR(&c, 1));
		}	break;

		case 'S': {
			lstr_t str = va_arg(argl, lstr_t);
			written += pad_to(callb, usr, &buf_it, buf_end, leftpad, str.len);
			written += buffered_write(callb, usr, &buf_it, buf_end, str);
		}	break;

		case 's': {
			lstr_t str = lt_lsfroms(va_arg(argl, char*));
			written += pad_to(callb, usr, &buf_it, buf_end, leftpad, str.len);
			written += buffered_write(callb, usr, &buf_it, buf_end, str);
		}	break;

		case 'u': {
			u64 val;
			c = *++fmt;
			switch (c) {
			case 'b': val = va_arg(argl, unsigned int); break;
			case 'w': val = va_arg(argl, unsigned int); break;
			case 'd': val = va_arg(argl, u32); break;
			case 'q': val = va_arg(argl, u64); break;
			case 'z': val = va_arg(argl, usz); break;
			default: val = 0; break;
			}
			written += lt_io_printuq(callb, usr, &buf_it, buf_end, val, leftpad);
		}	break;

		case 'i': {
			i64 val;
			c = *++fmt;
			switch (c) {
			case 'b': val = va_arg(argl, int); break;
			case 'w': val = va_arg(argl, int); break;
			case 'd': val = va_arg(argl, i32); break;
			case 'q': val = va_arg(argl, i64); break;
			case 'z': val = va_arg(argl, isz); break;
			default: val = 0; break;
			}
			written += lt_io_printiq(callb, usr, &buf_it, buf_end, val, leftpad);
		}	break;

		case 'h': {
			u64 val;
			c = *++fmt;
			switch (c) {
			case 'b': val = va_arg(argl, unsigned int); break;
			case 'w': val = va_arg(argl, unsigned int); break;
			case 'd': val = va_arg(argl, u32); break;
			case 'q': val = va_arg(argl, u64); break;
			case 'z': val = va_arg(argl, usz); break;
			default: val = 0; break;
			}
			written += lt_io_printhq(callb, usr, &buf_it, buf_end, val, leftpad);
		}	break;

		case 'b': {
			u64 val;
			c = *++fmt;
			switch (c) {
			case 'b': val = va_arg(argl, unsigned int); break;
			case 'w': val = va_arg(argl, unsigned int); break;
			case 'd': val = va_arg(argl, u32); break;
			case 'q': val = va_arg(argl, u64); break;
			case 'z': val = va_arg(argl, usz); break;
			default: val = 0; break;
			}
			written += lt_io_printbq(callb, usr, &buf_it, buf_end, val, leftpad);
		}	break;

		case 'm': {
			u64 val;
			c = *++fmt;
			switch (c) {
			case 'b': val = va_arg(argl, unsigned int); break;
			case 'w': val = va_arg(argl, unsigned int); break;
			case 'd': val = va_arg(argl, u32); break;
			case 'q': val = va_arg(argl, u64); break;
			case 'z': val = va_arg(argl, usz); break;
			default: val = 0; break;
			}
			written += lt_io_printmq(callb, usr, &buf_it, buf_end, val, leftpad);
		}	break;

		case 'p':
			written += lt_io_printhq(callb, usr, &buf_it, buf_end, va_arg(argl, usz), leftpad);
			break;

		case 'f': {
			c = *++fmt;
			if (c != 'd' && c != 'q') {
				--fmt;
			}
			written += lt_io_printfq(callb, usr, &buf_it, buf_end, va_arg(argl, f64), leftpad);
		}	break;

		case 'r': {
			char val = *++fmt;
			usz repeat = va_arg(argl, usz);
			written += pad_to(callb, usr, &buf_it, buf_end, leftpad, repeat);

			for (;;) {
				usz writable = lt_min(repeat, buf_end - buf_it);
				repeat -= writable;
				buf_it += lt_mset8(buf_it, val, writable);

				if (repeat > 0) {
					written += callb(usr, buf, buf_it - buf);
					buf_it = buf;
				}
				else {
					break;
				}
			}
		}	break;

		default: {
			written += buffered_write(callb, usr, &buf_it, buf_end, LSTR((char*)substr_start, fmt - substr_start));
		}	break;
		}
		++fmt;
	}

	usz buf_rem = buf_it - buf;
	if (buf_rem > 0) {
		written += callb(usr, buf, buf_rem);
	}
	return written;
}

LT_FLATTEN
isz lt_io_printf(lt_write_fn_t callb, void* usr, const char* fmt, ...) {
	va_list argl;
	va_start(argl, fmt);
	isz bytes = lt_io_vprintf(callb, usr, fmt, argl);
	va_end(argl);
	return bytes;
}

