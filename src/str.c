#include <lt2/str.h>

#include <ctype.h>

b8 lseq_nocase(ls s1, ls s2) {
	if (s1.size != s2.size)
		return 0;
	for (u8* it = s1.ptr, *it2 = s2.ptr, *end = it + s1.size; it < end; ++it, ++it2) {
		if (toupper(*it) != toupper(*it2))
			return 0;
	}
	return 1;
}

b8 lseq_upper(ls str, ls upper) {
	if (str.size != upper.size)
		return 0;
	for (u8* it = str.ptr, *upper_it = upper.ptr, *end = it + str.size; it < end; ++it, ++upper_it) {
		if (toupper(*it) != *upper_it)
			return 0;
	}
	return 1;
}

ls lstrim_left(ls str) {
	u8* it, *end;
	for (it = str.ptr, end = it + str.size; it < end && isspace(*it); ++it)
		;
	return lls(it, end - it);
}

ls lstrim_right(ls str) {
	u8* it;
	for (it = str.ptr + str.size; it > str.ptr && isspace(*(it - 1)); --it)
		;
	return lls(str.ptr, it - str.ptr);
}

FLATTEN
ls lstrim(ls str) {
	return lstrim_left(lstrim_right(str));
}

f64 lstof(ls str, err* err) {
	if UNLIKELY (!str.size) {
		throw(err, ERR_EMPTY, "cannot convert empty string to floating-point number");
		return 0.0;
	}

	usz it = 0;

	b8 sign = str.ptr[0] == '-';
	if (sign)
		++it;

	u64 val = 0;
	f64 float_val = 0;

	while (it < str.size) {
		if (str.ptr[it] == '.') {
			++it;
			u64 decimal = 0;
			u64 decimal_mult = 1;
			while (it < str.size) {
				decimal_mult *= 10;
				decimal *= 10;
				char c = str.ptr[it++];
				if UNLIKELY (!isdigit(c)) {
					throw(err, ERR_BAD_SYNTAX, "invalid character in floating-point number");
					return 0.0;
				}
				decimal += c - '0';
			}
			float_val = ((f64)val + (f64)decimal / (f64)decimal_mult);
			goto done;
		}
		val *= 10;
		char c = str.ptr[it++];
		if UNLIKELY (!isdigit(c)) {
			throw(err, ERR_BAD_SYNTAX, "invalid character in floating-point number");
			return 0.0;
		}
		val += c - '0';
	}
	float_val = (f64)val;
done:
	if (sign)
		return -float_val;
	return float_val;
}

i64 lstoi(ls str, err* err) {
	if UNLIKELY (!str.size) {
		throw(err, ERR_EMPTY, "cannot convert empty string to signed integer");
		return 0;
	}

	b8 sign = str.ptr[0] == '-';
	if (sign) {
		++str.ptr;
		--str.size;
	}

	u64 v = lstou(str, err);

	// !! off by one for underflows
	if (v > INT64_MAX) {
		throw(err, ERR_OVERFLOW, "value exceeds 64 bit signed integer limit");
		return 0;
	}

	if (sign)
		return -v;
	return v;
}

u64 lstou(ls str, err* err) {
	if UNLIKELY (!str.size) {
		throw(err, ERR_EMPTY, "cannot convert empty string to unsigned integer");
		return 0;
	}

	u64 val = 0;

	char* it = str.ptr, *end = it + str.size;
	while (it < end) {
		char c = *it++;

		if UNLIKELY (!isdigit(c)) {
			throw(err, ERR_BAD_SYNTAX, "invalid character in unsigned integer");
			return 0;
		}
		if UNLIKELY (val > UINT64_MAX/10) {
			throw(err, ERR_OVERFLOW, "value exceeds 64 bit unsigned integer limit");
			return 0;
		}

		val *= 10;
		val += c - '0';
	}
	return val;
}

static u8 hex_conv_tab[256] = {
	['0'] = 0x0,
	['1'] = 0x1,
	['2'] = 0x2,
	['3'] = 0x3,
	['4'] = 0x4,
	['5'] = 0x5,
	['6'] = 0x6,
	['7'] = 0x7,
	['8'] = 0x8,
	['9'] = 0x9,

	['A'] = 0xA, ['a'] = 0xA,
	['B'] = 0xB, ['b'] = 0xB,
	['C'] = 0xC, ['c'] = 0xC,
	['D'] = 0xD, ['d'] = 0xD,
	['E'] = 0xE, ['e'] = 0xE,
	['F'] = 0xF, ['f'] = 0xF,
 };

u64 hexlstou(ls str, err* err) {
	if UNLIKELY (!str.size) {
		throw(err, ERR_EMPTY, "cannot convert empty string to hexadecimal integer");
		return 0;
	}

	u64 val = 0;

	u8* it = str.ptr, *end = it + str.size;
	while (it < end) {
		u8 c = *it++;
		u8 hex = hex_conv_tab[c];

		if UNLIKELY (!hex && c != '0') {
			throw(err, ERR_BAD_SYNTAX, "invalid character in hexadecimal integer");
			return 0;
		}
		if UNLIKELY (val & 0xF000000000000000) {
			throw(err, ERR_OVERFLOW, "value exceeds 64 bit unsigned integer limit");
			return 0;
		}

		val <<= 4;
		val |= hex;
	}

	return val;
}


u8* lssubstr(ls str, ls substr) {
	u8* end = str.ptr + str.size - substr.size;
	for (u8* it = str.ptr; it <= end; ++it) {
		if (memcmp(it, substr.ptr, substr.size) == 0) {
			return it;
		}
	}
	return NULL;
}

static
void fill_ls(ls* buf, void* data, usz size) {
	if (size > buf->size)
		size = buf->size;
	memcpy(buf->ptr, data, size);
	buf->ptr += size;
	buf->size -= size;
}

ls vlsprintf(ls buf, const char* fmt, va_list args) {
	u8* start = buf.ptr;
	vlprintf((write_fn)fill_ls, &buf, fmt, args);
	return lsrange(start, buf.ptr);
}

ls lsprintf(ls buf, const char* fmt, ...) {
	u8* start = buf.ptr;
	va_list args;
	va_start(args, fmt);
	vlprintf((write_fn)fill_ls, &buf, fmt, args);
	va_end(args);
	return lsrange(start, buf.ptr);
}

