#pragma once

#include <lt2/common.h>

INLINE
b8 is_pow2(usz n) {
	return !(n & (n - 1));
}

INLINE
usz pad(usz size, usz align) {
	usz align_mask = (align - 1);
	usz n = size & align_mask;
	n ^= align_mask;
	return n + 1;
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

