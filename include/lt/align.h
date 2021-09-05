#ifndef LT_ALIGN_H
#define LT_ALIGN_H 1

#include <lt/lt.h>

#define LT_WORD_SIZE (sizeof(usz) << 1)

static LT_INLINE
b8 lt_is_pow2(usz n) {
	return !(n & (n - 1));
}

static LT_INLINE
usz lt_pad(usz size, usz align) {
	usz align_mask = (align - 1);
	usz n = size & align_mask;
	n ^= align_mask;
	return n + 1;
}

static LT_INLINE
usz lt_word_align_fwd(usz val) {
	return (val + (LT_WORD_SIZE - 1)) & ~(LT_WORD_SIZE - 1);
}

static LT_INLINE
usz lt_word_align_bwd(usz val) {
	return val & ~(LT_WORD_SIZE - 1);
}

static LT_INLINE
usz lt_align_fwd(usz val, usz align) {
	usz align_mask = align - 1 ;
	return (val + align_mask) & ~align_mask;
}

static LT_INLINE
usz lt_align_bwd(usz val, usz align) {
	return val & ~(align - 1);
}

#endif
