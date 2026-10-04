#pragma once

#include <lt2/common.h>

#define PI  3.1415926535
#define TAU (PI * 2.0)

#define RADDEG_MULT (180.0 / PI)
#define DEGRAD_MULT (1.0 / RADDEG_MULT)
#define RADTODEG(r) (r * RADDEG_MULT)
#define DEGTORAD(d) (d * DEGRAD_MULT)

#define DEF_CLAMPZ(T) \
	INLINE \
	T clampz_##T(T n) { \
		if (n < 0) \
			return 0; \
		return n; \
	}

DEF_CLAMPZ(isz)
DEF_CLAMPZ(i64)
DEF_CLAMPZ(i32)
DEF_CLAMPZ(i16)
DEF_CLAMPZ(i8)
DEF_CLAMPZ(f64)
DEF_CLAMPZ(f32)

#define DEF_CLAMP(T) \
	INLINE \
	T clamp_##T(T min, T max, T n) { \
		if (n >= max) \
			return max; \
		if (n <= min) \
			return min; \
		return n; \
	}

DEF_CLAMP(usz)
DEF_CLAMP(u64)
DEF_CLAMP(u32)
DEF_CLAMP(u16)
DEF_CLAMP(u8)
DEF_CLAMP(isz)
DEF_CLAMP(i64)
DEF_CLAMP(i32)
DEF_CLAMP(i16)
DEF_CLAMP(i8)
DEF_CLAMP(f64)
DEF_CLAMP(f32)

#define DEF_MIN(T) \
	INLINE \
	T min_##T(T a, T b) { \
		if (a < b) \
			return a; \
		return b; \
	}

DEF_MIN(usz)
DEF_MIN(u64)
DEF_MIN(u32)
DEF_MIN(u16)
DEF_MIN(u8)
DEF_MIN(isz)
DEF_MIN(i64)
DEF_MIN(i32)
DEF_MIN(i16)
DEF_MIN(i8)
DEF_MIN(f64)
DEF_MIN(f32)

#define DEF_MAX(T) \
	INLINE \
	T max_##T(T a, T b) { \
		if (a > b) \
			return a; \
		return b; \
	}

DEF_MAX(usz)
DEF_MAX(u64)
DEF_MAX(u32)
DEF_MAX(u16)
DEF_MAX(u8)
DEF_MAX(isz)
DEF_MAX(i64)
DEF_MAX(i32)
DEF_MAX(i16)
DEF_MAX(i8)
DEF_MAX(f64)
DEF_MAX(f32)

// undefined for INTXX_MIN, same as in libc

#define DEF_ABS(T) \
	INLINE \
	T abs_##T(T n) { \
		if (n < 0) \
			return -n; \
		return n; \
	}

DEF_ABS(isz)
DEF_ABS(i64)
DEF_ABS(i32)
DEF_ABS(i16)
DEF_ABS(i8)
DEF_ABS(f64)
DEF_ABS(f32)

