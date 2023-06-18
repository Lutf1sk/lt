#ifndef LT_MATH_H
#define LT_MATH_H 1

#include <lt/lt.h>
#include <lt/debug.h>

#define LT_DEFINE_MAX(T) static LT_INLINE T lt_max_##T (T a, T b) { return a > b ? a : b; }
LT_DEFINE_MAX(u8)
LT_DEFINE_MAX(u16)
LT_DEFINE_MAX(u32)
LT_DEFINE_MAX(u64)
LT_DEFINE_MAX(usz)

LT_DEFINE_MAX(i8)
LT_DEFINE_MAX(i16)
LT_DEFINE_MAX(i32)
LT_DEFINE_MAX(i64)
LT_DEFINE_MAX(isz)

LT_DEFINE_MAX(f32)
LT_DEFINE_MAX(f64)
#undef LT_DEFINE_MAX

#define LT_DEFINE_MIN(T) static LT_INLINE T lt_min_##T (T a, T b) { return a < b ? a : b; }
LT_DEFINE_MIN(u8)
LT_DEFINE_MIN(u16)
LT_DEFINE_MIN(u32)
LT_DEFINE_MIN(u64)
LT_DEFINE_MIN(usz)

LT_DEFINE_MIN(i8)
LT_DEFINE_MIN(i16)
LT_DEFINE_MIN(i32)
LT_DEFINE_MIN(i64)
LT_DEFINE_MIN(isz)

LT_DEFINE_MIN(f32)
LT_DEFINE_MIN(f64)
#undef LT_DEFINE_MAX

#define LT_DEFINE_CLAMP(T) static LT_INLINE T lt_clamp_##T (T v, T min, T max) { return v < min ? min : (v > max ? max : v); }
LT_DEFINE_CLAMP(u8)
LT_DEFINE_CLAMP(u16)
LT_DEFINE_CLAMP(u32)
LT_DEFINE_CLAMP(u64)
LT_DEFINE_CLAMP(usz)

LT_DEFINE_CLAMP(i8)
LT_DEFINE_CLAMP(i16)
LT_DEFINE_CLAMP(i32)
LT_DEFINE_CLAMP(i64)
LT_DEFINE_CLAMP(isz)

LT_DEFINE_CLAMP(f32)
LT_DEFINE_CLAMP(f64)
#undef LT_DEFINE_CLAMP

#define LT_DEFINE_ABS(T) static LT_INLINE T lt_abs_##T (T v) { return v < 0 ? -v : v; }
LT_DEFINE_ABS(i8)
LT_DEFINE_ABS(i16)
LT_DEFINE_ABS(i32)
LT_DEFINE_ABS(i64)
LT_DEFINE_ABS(isz)

LT_DEFINE_ABS(f32)
LT_DEFINE_ABS(f64)
#undef LT_DEFINE_ABS

#define LT_DEF_ROTL(bits) static LT_INLINE u##bits lt_rotl##bits(u##bits val, u8 shift) { return (val << shift) | (val >> (bits - shift)); }
LT_DEF_ROTL(8)
LT_DEF_ROTL(16)
LT_DEF_ROTL(32)
LT_DEF_ROTL(64)
#undef LT_DEF_ROTL

#define LT_DEF_ROTR(bits) static LT_INLINE u##bits lt_rotr##bits(u##bits val, u8 shift) { return (val >> shift) | (val << (bits - shift)); }
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
