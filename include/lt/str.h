#ifndef LT_STR_H
#define LT_STR_H

#include <lt/err.h>
#include <lt/fwd.h>

extern int memcmp(const void* ptr1, const void* ptr2, size_t size);

extern usz strlen(const char* str);
extern usz strnlen(const char* str, usz max);

static LT_INLINE
b8 lt_lseq(lstr_t s1, lstr_t s2) {
	return s1.len == s2.len && memcmp(s1.str, s2.str, s1.len) == 0;
}

static LT_INLINE
i32 lt_lscmp(lstr_t s1, lstr_t s2) {
	isz dlen = s1.len - s2.len;
	if (dlen)
		return (dlen >> (8 * sizeof(isz) - 1)) * 2 + 1;
	return memcmp(s1.str, s2.str, s1.len);
}

b8 lt_lseq_nocase(lstr_t s1, lstr_t s2);

static LT_INLINE
b8 lt_lsprefix(lstr_t str, lstr_t substr) {
	if (str.len < substr.len)
		return 0;
	return memcmp(str.str, substr.str, substr.len) == 0;
}

static LT_INLINE
b8 lt_lssuffix(lstr_t str, lstr_t substr) {
	if (str.len < substr.len)
		return 0;

	char* end = str.str + str.len;
	return memcmp(end - substr.len, substr.str, substr.len) == 0;
}

static LT_INLINE
lstr_t lt_lssplit(lstr_t str, char delim) {
	for (usz i = 0; i < str.len; ++i)
		if (str.str[i] == delim)
			return LSTR(str.str, i);
	return str;
}

static LT_INLINE
lstr_t lt_lssplit_bwd(lstr_t str, char delim) {
	for (isz i = str.len - 1; i >= 0; --i)
		if (str.str[i] == delim)
			return LSTR(str.str + i + 1, str.len - i - 1);
	return str;
}

static LT_INLINE
lstr_t lt_lsfroms(const char* cstr) {
	return LSTR((char*)cstr, strlen(cstr));
}

char* lt_lstos(lstr_t lstr, lt_alloc_t alloc[static 1]);

static LT_INLINE
lstr_t lt_lsfrom_range(const char* start, const char* end) {
	return LSTR((char*)start, end - start);
}

static LT_INLINE
lstr_t lt_lstake(lstr_t str, usz bytes) {
	return LSTR(str.str, bytes);
}

static LT_INLINE
lstr_t lt_lsdrop(lstr_t str, usz bytes) {
	return LSTR(str.str + bytes, str.len - bytes);
}

lstr_t lt_lstrim_left(lstr_t str);
lstr_t lt_lstrim_right(lstr_t str);
lstr_t lt_lstrim(lstr_t str);

lt_err_t lt_lstof(lstr_t, f64 out[static 1]);
lt_err_t lt_lstoi(lstr_t, i64 out[static 1]);
lt_err_t lt_lstou(lstr_t, u64 out[static 1]);
lt_err_t lt_lshextou(lstr_t str, u64 out[static 1]);

lstr_t lt_lsbuild(lt_alloc_t alloc[static 1], const char* fmt, ...);
char* lt_sbuild(lt_alloc_t alloc[static 1], const char* fmt, ...);

lstr_t lt_lsdirname(lstr_t path);
lstr_t lt_lsbasename(lstr_t path);

lstr_t lt_lstrim_trailing_slash(lstr_t path);

#endif
