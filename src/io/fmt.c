#include <lt/io.h>
#include <lt/str.h>
#include <lt/mem.h>

isz lt_file_vprintf(lt_file_t* file, char* fmt, va_list args) {
	char* it = fmt;

	isz written = 0;

	char c;
	while ((c = *it) != 0) {
		if (c != '%') {
			char* substr_start = it;
			while ((c = *it) != 0 && c != '%')
				++it;
			written += lt_file_write(file, substr_start, it - substr_start);
			continue;
		}

		c = *++it;

		switch (c) {
		case 0: --it; break;

		case '%':
			written += lt_file_printc(file, '%');
			break;

		case 'c':
			written += lt_file_printc(file, va_arg(args, int));
			break;

		case 'S': {
			lstr_t str = va_arg(args, lstr_t);
			written += lt_file_write(file, str.str, str.len);
		}	break;

		case 's': {
			char* str = va_arg(args, char*);
			written += lt_file_write(file, str, strlen(str));
		}	break;

		case 'u': {
			u64 val;
			c = *++it;
			switch (c) {
			case 'b': val = va_arg(args, int); break;
			case 'w': val = va_arg(args, int); break;
			case 'd': val = va_arg(args, u32); break;
			case 'q': val = va_arg(args, u64); break;
			case 'z': val = va_arg(args, usz); break;
			default: val = 0; break;
			}
			written += lt_file_printuq(file, val);
		}	break;

		case 'i': {
			i64 val;
			c = *++it;
			switch (c) {
			case 'b': val = va_arg(args, int); break;
			case 'w': val = va_arg(args, int); break;
			case 'd': val = va_arg(args, i32); break;
			case 'q': val = va_arg(args, i64); break;
			case 'z': val = va_arg(args, isz); break;
			default: val = 0; break;
			}
			written += lt_file_printiq(file, val);
		}	break;

		case 'h': {
			u64 val;
			c = *++it;
			switch (c) {
			case 'b': val = va_arg(args, int); break;
			case 'w': val = va_arg(args, int); break;
			case 'd': val = va_arg(args, u32); break;
			case 'q': val = va_arg(args, u64); break;
			case 'z': val = va_arg(args, usz); break;
			default: val = 0; break;
			}
			written += lt_file_printuq_hex(file, val);
		}	break;

		case 'p':
			written += lt_file_printuq_hex(file, va_arg(args, usz));
			break;

		default: {
			char buf[2] = "% ";
			buf[1] = c;
			written += lt_file_write(file, buf, 2);
		}	break;
		}
		++it;
	}

	return written;
}

isz lt_str_vprintf(char* str, char* fmt, va_list args) {
	char* it = fmt;
	char* out_it = str;

	char c;
	while ((c = *it) != 0) {
		if (c != '%') {
			char* substr_start = it;
			while ((c = *it) != 0 && c != '%')
				++it;

			usz len = it - substr_start;
			memcpy(out_it, substr_start, len);
			out_it += len;
			continue;
		}

		c = *++it;

		switch (c) {
		case 0: --it; break;

		case '%':
			*(out_it++) = '%';
			break;

		case 'c':
			*(out_it++) = va_arg(args, int);
			break;

		case 'S': {
			lstr_t str = va_arg(args, lstr_t);
			memcpy(out_it, str.str, str.len);
			out_it += str.len;
		}	break;

		case 's': {
			char* str = va_arg(args, char*);
			usz len = strlen(str);
			memcpy(out_it, str, len);
			out_it += len;
		}	break;

		case 'u': {
			u64 val;
			c = *++it;
			switch (c) {
			case 'b': val = va_arg(args, int); break;
			case 'w': val = va_arg(args, int); break;
			case 'd': val = va_arg(args, u32); break;
			case 'q': val = va_arg(args, u64); break;
			case 'z': val = va_arg(args, usz); break;
			default: val = 0; break;
			}
			out_it += lt_str_printuq(out_it, val);
		}	break;

		case 'i': {
			i64 val;
			c = *++it;
			switch (c) {
			case 'b': val = va_arg(args, int); break;
			case 'w': val = va_arg(args, int); break;
			case 'd': val = va_arg(args, i32); break;
			case 'q': val = va_arg(args, i64); break;
			case 'z': val = va_arg(args, isz); break;
			default: val = 0; break;
			}
			out_it += lt_str_printiq(out_it, val);
		}	break;

		case 'h': {
			u64 val;
			c = *++it;
			switch (c) {
			case 'b': val = va_arg(args, int); break;
			case 'w': val = va_arg(args, int); break;
			case 'd': val = va_arg(args, u32); break;
			case 'q': val = va_arg(args, u64); break;
			case 'z': val = va_arg(args, usz); break;
			default: val = 0; break;
			}
			out_it += lt_str_printuq_hex(out_it, val);
		}	break;

		case 'p':
			out_it += lt_str_printuq_hex(out_it, va_arg(args, usz));
			break;

		default: {
			char buf[2] = "% ";
			buf[1] = c;
			memcpy(out_it, buf, 2);
			out_it += 2;
		}	break;
		}
		++it;
	}

	return out_it - str;
}

