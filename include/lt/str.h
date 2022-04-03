#ifndef LT_STR_H
#define LT_STR_H

#include <lt/lt.h>

extern int memcmp(const void* ptr1, const void* ptr2, size_t size);

static LT_INLINE
b8 lt_lstr_eq(lstr_t s1, lstr_t s2) {
	return !((s1.len - s2.len) | memcmp(s1.str, s2.str, s1.len));
}

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

f64 lt_lstr_float(lstr_t);
i64 lt_lstr_int(lstr_t);
u64 lt_lstr_uint(lstr_t);
u64 lt_lstr_hex_uint(lstr_t str);

#endif
