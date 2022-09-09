#ifndef LT_BITS_H
#define LT_BITS_H 1

#include <lt/lt.h>

#define LT_DEF_ROTL(bits) \
	static LT_INLINE \
	u##bits lt_rotl##bits(u##bits val, u8 shift) { \
		return (val << shift) | (val >> (bits - shift)); \
	}

LT_DEF_ROTL(8)
LT_DEF_ROTL(16)
LT_DEF_ROTL(32)
LT_DEF_ROTL(64)

#undef LT_DEF_ROTL

#define LT_DEF_ROTR(bits) \
	static LT_INLINE \
	u##bits lt_rotr##bits(u##bits val, u8 shift) { \
		return (val >> shift) | (val << (bits - shift)); \
	}

LT_DEF_ROTR(8)
LT_DEF_ROTR(16)
LT_DEF_ROTR(32)
LT_DEF_ROTR(64)

#undef LT_DEF_ROTR

static LT_INLINE
i64 lt_sign_ext(i64 v, usz bytes) {
	switch (bytes) {
	case 1: return (i8)v;
	case 2: return (i16)v;
	case 4: return (i32)v;
	case 8: return v;
	default: LT_ASSERT_NOT_REACHED(); return 0;
	}
}

#endif
