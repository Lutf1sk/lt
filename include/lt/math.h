#ifndef LT_MATH_H
#define LT_MATH_H 1

#include <lt/lt.h>
#include <lt/debug.h>

#define LT_PI 3.14159265
#define LT_TAU (LT_PI * 2.0)

#define LT_RAD_HALF LT_PI
#define LT_RAD_MAX LT_TAU

#define LT_RADDEG_MULT (180.0 / LT_PI)
#define LT_DEGRAD_MULT (1.0 / LT_RADDEG_MULT)

#define LT_RADTODEG(r) (r * LT_RADDEG_MULT)
#define LT_DEGTORAD(d) (d * LT_DEGRAD_MULT)

#define LT_DEFINE_MAX_FUNC(T) static LT_INLINE T lt_##T##_max (T a, T b) { return a > b ? a : b; }
LT_DEFINE_MAX_FUNC(u8)
LT_DEFINE_MAX_FUNC(u16)
LT_DEFINE_MAX_FUNC(u32)
LT_DEFINE_MAX_FUNC(u64)
LT_DEFINE_MAX_FUNC(usz)

LT_DEFINE_MAX_FUNC(i8)
LT_DEFINE_MAX_FUNC(i16)
LT_DEFINE_MAX_FUNC(i32)
LT_DEFINE_MAX_FUNC(i64)
LT_DEFINE_MAX_FUNC(isz)

LT_DEFINE_MAX_FUNC(f32)
LT_DEFINE_MAX_FUNC(f64)

#define lt_max(a, b) (_Generic((a), \
		u8: lt_u8_max, \
		u16: lt_u16_max, \
		u32: lt_u32_max, \
		u64: lt_u64_max, \
		i8: lt_i8_max, \
		i16: lt_i16_max, \
		i32: lt_i32_max, \
		i64: lt_i64_max, \
		f32: lt_f32_max, \
		f64: lt_f64_max \
		)((a), (b)))

#define LT_DEFINE_MIN_FUNC(T) static LT_INLINE T lt_##T##_min (T a, T b) { return a < b ? a : b; }
LT_DEFINE_MIN_FUNC(u8)
LT_DEFINE_MIN_FUNC(u16)
LT_DEFINE_MIN_FUNC(u32)
LT_DEFINE_MIN_FUNC(u64)
LT_DEFINE_MIN_FUNC(usz)

LT_DEFINE_MIN_FUNC(i8)
LT_DEFINE_MIN_FUNC(i16)
LT_DEFINE_MIN_FUNC(i32)
LT_DEFINE_MIN_FUNC(i64)
LT_DEFINE_MIN_FUNC(isz)

LT_DEFINE_MIN_FUNC(f32)
LT_DEFINE_MIN_FUNC(f64)

#define lt_min(a, b) (_Generic((a), \
		u8: lt_u8_min, \
		u16: lt_u16_min, \
		u32: lt_u32_min, \
		u64: lt_u64_min, \
		i8: lt_i8_min, \
		i16: lt_i16_min, \
		i32: lt_i32_min, \
		i64: lt_i64_min, \
		f32: lt_f32_min, \
		f64: lt_f64_min \
		)((a), (b)))

#define LT_DEFINE_CLAMP_FUNC(T) static LT_INLINE T lt_##T##_clamp (T v, T min, T max) { return v < min ? min : (v > max ? max : v); }
LT_DEFINE_CLAMP_FUNC(u8)
LT_DEFINE_CLAMP_FUNC(u16)
LT_DEFINE_CLAMP_FUNC(u32)
LT_DEFINE_CLAMP_FUNC(u64)
LT_DEFINE_CLAMP_FUNC(usz)

LT_DEFINE_CLAMP_FUNC(i8)
LT_DEFINE_CLAMP_FUNC(i16)
LT_DEFINE_CLAMP_FUNC(i32)
LT_DEFINE_CLAMP_FUNC(i64)
LT_DEFINE_CLAMP_FUNC(isz)

LT_DEFINE_CLAMP_FUNC(f32)
LT_DEFINE_CLAMP_FUNC(f64)

#define lt_clamp(a, b, c) (_Generic((a), \
		u8: lt_u8_clamp, \
		u16: lt_u16_clamp, \
		u32: lt_u32_clamp, \
		u64: lt_u64_clamp, \
		i8: lt_i8_clamp, \
		i16: lt_i16_clamp, \
		i32: lt_i32_clamp, \
		i64: lt_i64_clamp, \
		f32: lt_f32_clamp, \
		f64: lt_f64_clamp \
		)((a), (b), (c)))

#define LT_DEFINE_ABS_FUNC(T) static LT_INLINE T lt_##T##_abs (T v) { return v < 0 ? -v : v; }
LT_DEFINE_ABS_FUNC(i8)
LT_DEFINE_ABS_FUNC(i16)
LT_DEFINE_ABS_FUNC(i32)
LT_DEFINE_ABS_FUNC(i64)
LT_DEFINE_ABS_FUNC(isz)

LT_DEFINE_ABS_FUNC(f32)
LT_DEFINE_ABS_FUNC(f64)

#define lt_abs(a) (_Generic((a), \
		u8: lt_i8_abs, \
		u16: lt_i16_abs, \
		u32: lt_i32_abs, \
		u64: lt_i64_abs, \
		i8: lt_i8_abs, \
		i16: lt_i16_abs, \
		i32: lt_i32_abs, \
		i64: lt_i64_abs, \
		f32: lt_f32_abs, \
		f64: lt_f64_abs \
		)((a)))


#define LT_DEF_ROTL_FUNC(bits) static LT_INLINE u##bits lt_rotl##bits(u##bits val, u8 shift) { return (val << shift) | (val >> (bits - shift)); }
LT_DEF_ROTL_FUNC(8)
LT_DEF_ROTL_FUNC(16)
LT_DEF_ROTL_FUNC(32)
LT_DEF_ROTL_FUNC(64)

#define LT_DEF_ROTR_FUNC(bits) static LT_INLINE u##bits lt_rotr##bits(u##bits val, u8 shift) { return (val >> shift) | (val << (bits - shift)); }
LT_DEF_ROTR_FUNC(8)
LT_DEF_ROTR_FUNC(16)
LT_DEF_ROTR_FUNC(32)
LT_DEF_ROTR_FUNC(64)

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

#define LT_ADDPTR(p, a) ((void*)((usz)(p) + (isz)(a)))
#define LT_SUBPTR(p, a) ((void*)((usz)(p) - (isz)(a)))
#define LT_MULPTR(p, a) ((void*)((usz)(p) * (isz)(a)))
#define LT_DIVPTR(p, a) ((void*)((usz)(p) / (isz)(a)))
#define LT_MODPTR(p, a) ((void*)((usz)(p) % (isz)(a)))
#define LT_SHLPTR(p, a) ((void*)((usz)(p) << (isz)(a)))
#define LT_SHRPTR(p, a) ((void*)((usz)(p) >> (isz)(a)))
#define LT_ANDPTR(p, a) ((void*)((usz)(p) & (isz)(a)))
#define LT_ORPTR(p, a) ((void*)((usz)(p) | (isz)(a)))
#define LT_NOTPTR(p) ((void*)~(usz)(p))
#define LT_NEGPTR(p) ((void*)-(usz)(p))


#endif
