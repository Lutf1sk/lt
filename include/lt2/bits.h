#pragma once

#include <lt2/common.h>

INLINE
b8 is_pow2(usz n) {
	return !(n & (n - 1));
}

INLINE
b8 is_nzpow2(usz n) {
	if (!n)
		return 0;
	return !(n & (n - 1));
}

INLINE
usz clz_usz(usz n) {
#if SIZE_WIDTH > 32
	return __builtin_clzl(n);
#else
	return __builtin_clz(n);
#endif
}

INLINE
usz next_pow2(usz n) {
	if (n <= 1)
		return 1;
	return (usz)1 << (sizeof(n)*8 - clz_usz(n - 1));
}

INLINE
usz pad(usz size, usz align) {
	usz align_mask = (align - 1);
	return (align - (size & align_mask)) & align_mask;
}

INLINE
usz align_default(usz val) {
	return (val + (DEFAULT_ALIGN - 1)) & ~(DEFAULT_ALIGN - 1);
}

INLINE
usz align_bwd_default(usz val) {
	return val & ~(DEFAULT_ALIGN - 1);
}

INLINE
usz align(usz val, usz align) {
	usz align_mask = align - 1;
	return (val + align_mask) & ~align_mask;
}

INLINE
usz align_bwd(usz val, usz align) {
	return val & ~(align - 1);
}

#define alignptr(v, a) (void*)align((usz)(v), (a))
#define alignptr_bwd(v, a) (void*)align_bwd((usz)(v), (a))

