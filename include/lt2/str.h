#pragma once

#include <lt2/common.h>

INLINE
b8 lseq(ls s1, ls s2) {
	return s1.size == s2.size && memcmp(s1.ptr, s2.ptr, s1.size) == 0;
}

INLINE
i32 lscmp(ls s1, ls s2) {
	isz dlen = s1.size - s2.size;
	if (dlen < 0)
		return -1;
	if (dlen > 0)
		return 1;
	return memcmp(s1.ptr, s2.ptr, s1.size);
}

b8 lseq_nocase(ls s1, ls s2);

INLINE
b8 lsprefix(ls str, ls substr) {
	if (str.size < substr.size)
		return 0;
	return memcmp(str.ptr, substr.ptr, substr.size) == 0;
}

INLINE
b8 lssuffix(ls str, ls substr) {
	if (str.size < substr.size)
		return 0;

	char* end = str.ptr + str.size;
	return memcmp(end - substr.size, substr.ptr, substr.size) == 0;
}

INLINE
ls lssplit(ls str, char delim) {
	for (usz i = 0; i < str.size; ++i)
		if (str.ptr[i] == delim)
			return lls(str.ptr, i);
	return str;
}

INLINE
ls lssplit_bwd(ls str, char delim) {
	for (isz i = str.size - 1; i >= 0; --i)
		if (str.ptr[i] == delim)
			return lls(str.ptr + i + 1, str.size - i - 1);
	return str;
}

INLINE
ls stols(const char* cstr) {
	return lls(cstr, strlen(cstr));
}

INLINE
ls lsrange(const void* start, const void* end) {
	return lls(start, (u8*)end - (u8*)start);
}

INLINE
ls lstake(ls str, usz bytes) {
	return lls(str.ptr, bytes);
}

INLINE
ls lsdrop(ls str, usz bytes) {
	return lls(str.ptr + bytes, str.size - bytes);
}

INLINE
usz lsfirst(ls str, u8 c) {
	for (u8* it = str.ptr, *end = it + str.size; it < end; ++it)
		if (*it == c)
			return it - str.ptr;
	return str.size;
}

ls lstrim_left(ls str);
ls lstrim_right(ls str);
ls lstrim(ls str);

f64 lstof(ls str, err* err);
i64 lstoi(ls str, err* err);
u64 lstou(ls str, err* err);
u64 hexlstou(ls str, err* err);

u8* lssubstr(ls str, ls substr);

ls vlsprintf(ls buf, const char* fmt, va_list args);
ls lsprintf(ls buf, const char* fmt, ...);

ls lsdirname(ls path);
ls lsbasename(ls path);

ls lstrim_trailing_slash(ls path);


