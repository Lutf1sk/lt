#ifndef LT_STR_H
#define LT_STR_H

#include <lt/err.h>

extern int memcmp(const void* ptr1, const void* ptr2, size_t size);

extern usz strlen(const char* str);
extern usz strnlen(const char* str, usz max);

static LT_INLINE
b8 lt_lstr_eq(lstr_t s1, lstr_t s2) {
	return !((s1.len - s2.len) | memcmp(s1.str, s2.str, s1.len));
}

b8 lt_lstr_case_eq(lstr_t s1, lstr_t s2);

static LT_INLINE
b8 lt_lstr_startswith(lstr_t str, lstr_t substr) {
	if (str.len < substr.len)
		return 0;
	return memcmp(str.str, substr.str, substr.len) == 0;
}

static LT_INLINE
b8 lt_lstr_endswith(lstr_t str, lstr_t substr) {
	if (str.len < substr.len)
		return 0;

	char* end = str.str + str.len;
	return memcmp(end - substr.len, substr.str, substr.len) == 0;
}

static LT_INLINE
usz lt_lstr_split(lstr_t str, char delim) {
	for (usz i = 0; i < str.len; ++i)
		if (str.str[i] == delim)
			return i;
	return str.len;
}

static LT_INLINE
lstr_t lt_lstr_split_bwd(lstr_t str, char delim) {
	for (isz i = str.len - 1; i >= 0; --i)
		if (str.str[i] == delim)
			return LSTR(str.str + i + 1, str.len - i - 1);
	return str;
}

static LT_INLINE
lstr_t lt_lstr_from_cstr(char* cstr) {
	return LSTR(cstr, strlen(cstr));
}

lstr_t lt_lstr_trim_left(lstr_t str);
lstr_t lt_lstr_trim_right(lstr_t str);
lstr_t lt_lstr_trim(lstr_t str);

lt_err_t lt_lstr_float(lstr_t, f64* out);
lt_err_t lt_lstr_int(lstr_t, i64* out);
lt_err_t lt_lstr_uint(lstr_t, u64* out);
lt_err_t lt_lstr_hex_uint(lstr_t str, u64* out);

#endif
