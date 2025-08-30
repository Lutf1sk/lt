#include <lt2/common.h>
#include <lt2/str.h>

#define NUM_MAX 64

INLINE
usz printu64(write_fn fn, void* usr, u64 n) {
	char buf[NUM_MAX], *end = buf + sizeof(buf), *it = end - 1;

	// Fill buffer backwards with the remainder of n/10
	while (n >= 10) {
		*it-- = n % 10 + '0';
		n /= 10;
	}
	*it = n + '0';

	usz len = end - it;
	return fn(usr, it, len);
}

INLINE
usz printi64(write_fn fn, void* usr, i64 n) {
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

	if (sign)
		*--it = '-';

	usz len = end - it;
	return fn(usr, it, len);
}

#include <time.h>

usz printdt64(write_fn fn, void* usr, time_t n) {
	struct tm* plocal = localtime(&n);
	u8 buf[128];
	strftime(buf, sizeof(buf), "%x %X", plocal);
	return fn(usr, buf, strlen(buf));
}

isz vlprintf(write_fn fn, void* usr, const char* fmt, va_list args) {
	usz size;
	usz written = 0;
	const char* start;

	for (;;) {
		start = fmt;
		while (*fmt != '{') {
			if (!*fmt)
				goto end;
			++fmt;
		}

		size = fmt - start;
		if (size)
			written += fn(usr, start, size);

		if (*++fmt == '{') {
			written += fn(usr, fmt, 1);
			continue;
		}

		const char* start = fmt;
		while (*fmt != '}') {
			if (!*fmt)
				goto end;
			++fmt;
		}
		ls spec = lsrange(start, fmt++);

		if      (lseq(spec, ls("u8")))  written += printu64(fn, usr, va_arg(args, u32));
		else if (lseq(spec, ls("u16"))) written += printu64(fn, usr, va_arg(args, u32));
		else if (lseq(spec, ls("u32"))) written += printu64(fn, usr, va_arg(args, u32));
		else if (lseq(spec, ls("u64"))) written += printu64(fn, usr, va_arg(args, u64));
		else if (lseq(spec, ls("usz"))) written += printu64(fn, usr, va_arg(args, usz));

		else if (lseq(spec, ls("i8")))  written += printi64(fn, usr, va_arg(args, i32));
		else if (lseq(spec, ls("i16"))) written += printi64(fn, usr, va_arg(args, i32));
		else if (lseq(spec, ls("i32"))) written += printi64(fn, usr, va_arg(args, i32));
		else if (lseq(spec, ls("i64"))) written += printi64(fn, usr, va_arg(args, i64));
		else if (lseq(spec, ls("isz"))) written += printi64(fn, usr, va_arg(args, isz));

		else if (lseq(spec, ls("b8"))) written += printi64(fn, usr, va_arg(args, u32));

		else if (lseq(spec, ls("dt64"))) written += printdt64(fn, usr, va_arg(args, time_t));

		else if (lseq(spec, ls("char"))) {
			char cval = va_arg(args, int);
			written += fn(usr, &cval, 1);
		}

		else if (lseq(spec, ls("char*"))) {
			char* cstr = va_arg(args, char*);
			usz len = strlen(cstr);
			if (len)
				written += fn(usr, cstr, len);
		}

		else if (lseq(spec, ls("ls"))) {
			ls str = va_arg(args, ls);
			if (str.size)
				written += fn(usr, str.ptr, str.size);
		}
	}

end:;
	size = fmt - start;
	if (size)
		written += fn(usr, start, size);
	return written;
}

static
isz return_size(void* usr, const void* data, usz size) {
	return size;
}

FLATTEN
isz llenf(const char* fmt, ...) {
	va_list arg_list;
	va_start(arg_list, fmt);
	usz res = vlprintf(return_size, NULL, fmt, arg_list);
	va_end(arg_list);
	return res;
}

FLATTEN
isz lvlenf(const char* fmt, va_list args) {
	return vlprintf(return_size, NULL, fmt, args);
}

