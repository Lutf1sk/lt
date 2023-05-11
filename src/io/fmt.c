#include <lt/io.h>
// #include <lt/mem.h>

isz lt_io_dummy_callb(void* usr, void* data, usz len) {
	return len;
}

static LT_FLATTEN LT_INLINE
isz lt_io_printuq(lt_io_callback_t callb, void* usr, u64 n) {
	char buf[32], *end = buf + sizeof(buf) - 1, *it = end;

	// Fill buffer backwards with the remainder of n/10
	while (n >= 10) {
		*it-- = n % 10 + '0';
		n /= 10;
	}
	*it = n + '0';

	usz len = end - it;
	return callb(usr, it, len + 1);
}

static LT_FLATTEN LT_INLINE
isz lt_io_printiq(lt_io_callback_t callb, void* usr, i64 n) {
	static char minus = '-';
	usz sign = 0;

	u64 v = n;
	// Handle signed values
	if (n < 0) {
		v = -v;
		callb(usr, &minus, 1);
		sign = 1;
	}

	return lt_io_printuq(callb, usr, v) + sign;
}

static char io_hex_char[16] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F' };

static LT_FLATTEN LT_INLINE
isz lt_io_printhq(lt_io_callback_t callb, void* usr, u64 n) {
	char buf[32], *end = buf + sizeof(buf) - 1, *it = end;

	// Fill buffer backwards with the lowest nibble of n
	while (n > 0x0F) {
		*it-- = io_hex_char[n & 0x0F];
		n >>= 4;
	}
	*it = io_hex_char[n];

	usz len = end - it;
	return callb(usr, it, len + 1);
}

static LT_INLINE
isz lt_io_printfq(lt_io_callback_t callb, void* usr, f64 n) {
	return -LT_ERR_UNSUPPORTED;
}


isz lt_io_vprintf(lt_io_callback_t callb, void* usr, char* fmt, va_list argl) {
	char* it = fmt;

	isz written = 0;

	char c;
	while ((c = *it) != 0) {
		if (c != '%') {
			char* substr_start = it;
			while ((c = *it) != 0 && c != '%')
				++it;
			written += callb(usr, substr_start, it - substr_start);
			continue;
		}

		c = *++it;

		switch (c) {
		case 0: --it; break;

		case '%':
			written += callb(usr, "%", 1);
			break;

		case 'c': {
			char c = va_arg(argl, int);
			written += callb(usr, &c, 1);
		}	break;

		case 'S': {
			lstr_t str = va_arg(argl, lstr_t);
			written += callb(usr, str.str, str.len);
		}	break;

		case 's': {
			char* str = va_arg(argl, char*);
			written += callb(usr, str, strlen(str));
		}	break;

		case 'u': {
			u64 val;
			c = *++it;
			switch (c) {
			case 'b': val = va_arg(argl, unsigned int); break;
			case 'w': val = va_arg(argl, unsigned int); break;
			case 'd': val = va_arg(argl, u32); break;
			case 'q': val = va_arg(argl, u64); break;
			case 'z': val = va_arg(argl, usz); break;
			default: val = 0; break;
			}
			written += lt_io_printuq(callb, usr, val);
		}	break;

		case 'i': {
			i64 val;
			c = *++it;
			switch (c) {
			case 'b': val = va_arg(argl, int); break;
			case 'w': val = va_arg(argl, int); break;
			case 'd': val = va_arg(argl, i32); break;
			case 'q': val = va_arg(argl, i64); break;
			case 'z': val = va_arg(argl, isz); break;
			default: val = 0; break;
			}
			written += lt_io_printiq(callb, usr, val);
		}	break;

		case 'h': {
			u64 val;
			c = *++it;
			switch (c) {
			case 'b': val = va_arg(argl, unsigned int); break;
			case 'w': val = va_arg(argl, unsigned int); break;
			case 'd': val = va_arg(argl, u32); break;
			case 'q': val = va_arg(argl, u64); break;
			case 'z': val = va_arg(argl, usz); break;
			default: val = 0; break;
			}
			written += lt_io_printhq(callb, usr, val);
		}	break;

		case 'p':
			written += lt_io_printhq(callb, usr, va_arg(argl, usz));
			break;

		case 'f': {
			f64 val;
			c = *++it;
			switch (c) {
			case 'd': val = va_arg(argl, double); break;
			case 'q': val = va_arg(argl, f64); break;
			default: val = 0.0f; break;
			}
			written += lt_io_printfq(callb, usr, val);
		}	break;

		case 'r': {
			char val = *++it;
			isz repeat = va_arg(argl, usz);
			for (usz i = 0; i < repeat; ++i)
				written += callb(usr, &val, 1);
		}	break;

		default: {
			char buf[2] = "% ";
			buf[1] = c;
			written += callb(usr, buf, 2);
		}	break;
		}
		++it;
	}

	return written;
}

LT_FLATTEN
isz lt_io_printf(lt_io_callback_t callb, void* usr, char* fmt, ...) {
	va_list argl;
	va_start(argl, fmt);
	isz bytes = lt_io_vprintf(callb, usr, fmt, argl);
	va_end(argl);
	return bytes;
}

