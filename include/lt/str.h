#ifndef LT_STR_H
#define LT_STR_H

#include <lt/lt.h>

extern int memcmp(const void* ptr1, const void* ptr2, size_t size);

static LT_INLINE
b8 lt_lstr_eq(lstr_t s1, lstr_t s2) {
	return !((s1.len - s2.len) | memcmp(s1.str, s2.str, s1.len));
}

f64 lt_lstr_float(lstr_t);
i64 lt_lstr_int(lstr_t);
u64 lt_lstr_uint(lstr_t);

#endif
