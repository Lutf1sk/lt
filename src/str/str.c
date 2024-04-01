#include <lt/str.h>
#include <lt/ctype.h>
#include <lt/mem.h>
#include <lt/io.h>

b8 lt_lseq_nocase(lstr_t s1, lstr_t s2) {
	if (s1.len != s2.len)
		return 0;
	for (usz i = 0; i < s1.len; ++i)
		if (lt_to_upper(s1.str[i]) != lt_to_upper(s2.str[i]))
			return 0;
	return 1;
}

lstr_t lt_lstrim_left(lstr_t str) {
	char *it, *end;
	for (it = str.str, end = it + str.len; it < end && lt_is_space(*it); ++it)
		;
	return LSTR(it, end - it);
}

lstr_t lt_lstrim_right(lstr_t str) {
	char *it;
	for (it = str.str + str.len; it > str.str && lt_is_space(*(it - 1)); --it)
		;
	return LSTR(str.str, it - str.str);
}

LT_FLATTEN
lstr_t lt_lstrim(lstr_t str) {
	return lt_lstrim_left(lt_lstrim_right(str));
}

char* lt_lstos(lstr_t lstr, lt_alloc_t* alloc) {
	char* cstr = lt_malloc(alloc, lstr.len + 1);
	if (!cstr)
		return NULL;
	memcpy(cstr, lstr.str, lstr.len);
	cstr[lstr.len] = 0;
	return cstr;
}

lt_err_t lt_lstof(lstr_t str, f64* out) {
	if (!str.len)
		return LT_ERR_IS_EMPTY;

	usz it = 0;

	b8 sign = str.str[0] == '-';
	if (sign)
		++it;

	u64 val = 0;
	f64 float_val = 0;

	while (it < str.len) {
		if (str.str[it] == '.') {
			++it;
			u64 decimal = 0;
			u64 decimal_mult = 1;
			while (it < str.len) {
				decimal_mult *= 10;
				decimal *= 10;
				char c = str.str[it++];
				if (!lt_is_digit(c))
					return LT_ERR_INVALID_FORMAT;
				decimal += c - '0';
			}
			float_val = ((f64)val + (f64)decimal / (f64)decimal_mult);
			goto done;
		}
		val *= 10;
		char c = str.str[it++];
		if (!lt_is_digit(c))
			return LT_ERR_INVALID_FORMAT;
		val += c - '0';
	}
	float_val = (f64)val;
done:
	if (sign)
		float_val = -float_val;
	*out = float_val;
	return LT_SUCCESS;
}

lt_err_t lt_lstoi(lstr_t str, i64* out) {
	if (!str.len) {
		return LT_ERR_IS_EMPTY;
	}

	b8 sign = str.str[0] == '-';
	if (sign) {
		++str.str;
		--str.len;
	}

	u64 v;
	lt_err_t err = lt_lstou(str, &v);
	if (err)
		return err;

	// !! off by one for underflows
	if (v > LT_I64_MAX)
		return LT_ERR_OVERFLOW;

	if (sign)
		*out = -v;
	else
		*out = v;
	return LT_SUCCESS;
}

lt_err_t lt_lstou(lstr_t str, u64* out) {
	if (!str.len) {
		return LT_ERR_IS_EMPTY;
	}

	u64 val = 0;

	char* it = str.str, *end = it + str.len;
	while (it < end) {
		char c = *it++;

		if (!lt_is_digit(c))
			return LT_ERR_INVALID_FORMAT;
		if (val > LT_U64_MAX/10)
			return LT_ERR_OVERFLOW;

		val *= 10;
		val += c - '0';
	}

	*out = val;
	return LT_SUCCESS;
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

lt_err_t lt_lshextou(lstr_t str, u64* out) {
	if (!str.len) {
		return LT_ERR_IS_EMPTY;
	}

	u64 val = 0;

	char* it = str.str, *end = it + str.len;
	while (it < end) {
		u8 c = *it++;
		u8 hex = hex_conv_tab[c];

		if (!hex && c != '0')
			return LT_ERR_INVALID_FORMAT;
		if (val & 0xF000000000000000)
			return LT_ERR_OVERFLOW;

		val <<= 4;
		val |= hex;
	}

	*out = val;
	return LT_SUCCESS;
}

lstr_t lt_lsdirname(lstr_t path) {
	if (path.len == 0)
		return CLSTR(".");

	isz last_idx = path.len - 1;
	isz last_nonslash = last_idx;

	if (path.str[last_idx] == '/') {
		while (last_nonslash >= 0 && path.str[last_nonslash] == '/')
			--last_nonslash;

		if (last_nonslash == -1)
			return CLSTR("/");
	}

	isz sep_slash = last_nonslash;
	while (sep_slash >= 0 && path.str[sep_slash] != '/')
		--sep_slash;
	if (sep_slash == -1)
		return CLSTR(".");

	isz first_sep_slash = sep_slash;
	while (first_sep_slash > 0 && path.str[first_sep_slash - 1] == '/')
		--first_sep_slash;
	if (first_sep_slash == 0)
		return CLSTR("/");

	return LSTR(path.str, first_sep_slash);
}

lstr_t lt_lsbasename(lstr_t path) {
	if (path.len == 0)
		return CLSTR(".");

	isz last_idx = path.len - 1;
	isz last_nonslash = last_idx;

	if (path.str[last_idx] == '/') {
		while (last_nonslash >= 0 && path.str[last_nonslash] == '/')
			--last_nonslash;

		if (last_nonslash == -1)
			return CLSTR("/");
	}

	isz sep_slash = last_nonslash;
	while (sep_slash >= 0 && path.str[sep_slash] != '/')
		--sep_slash;

	return LSTR(path.str + sep_slash + 1, last_nonslash - sep_slash);
}

lstr_t lt_lstrim_trailing_slash(lstr_t path) {
	if (path.len <= 1) {
		return path;
	}

	char* end = path.str, *it = end + path.len - 1;
	for (; it > end; --it) {
		if (*it != '/') {
			return lt_lsfrom_range(end, it + 1);
		}
	}
	return lt_lsfrom_range(end, it + 1);
}

lstr_t lt_lsbuild(lt_alloc_t* alloc, char* fmt, ...) {
	va_list argl;
	va_start(argl, fmt);
	lstr_t str;
	isz res = lt_vaprintf(&str, alloc, fmt, argl);
	if (res < 0) {
		lt_werrbf("allocation failed\n");
		return NLSTR();
	}
	va_end(argl);
	return str;
}
