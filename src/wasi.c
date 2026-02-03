#include <lt2/common.h>

#ifdef ON_WASI
#	include <lt2/wasi.h>

int memcmp(const void* p1, const void* p2, usz size) {
	const u8* it1 = p1;
	const u8* it2 = p2;
	const u8* end1 = p1 + size;
	while (it1 < end1) {
		ptrdiff_t diff = *it1 - *it2;
		if LIKELY (!diff) {
			++it1, ++it2;
			continue;
		}

		if (diff < 0)
			return -1;
		else
			return 1;
	}
	return 0;
}

usz strlen(const char* str) {
	const char* it = str;
	while (*it)
		++it;
	return it - str;
}

usz strnlen(const char* str, usz max) {
	const char* it = str, *end = it + max;
	while (it < end && *it)
		++it;
	return it - str;
}

NORETURN
void exit(int code) {
	__wasi_proc_exit(code);
}

#endif // ON_WASI
