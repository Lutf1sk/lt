#ifndef LT_PRIMITIVES_H
#define LT_PRIMITIVES_H 1

typedef unsigned char uchar;
typedef unsigned short ushort;
typedef unsigned int uint;
typedef unsigned long ulong;

#include <stddef.h>
#include <stdint.h>

typedef size_t usz;
typedef intmax_t isz;

#define LT_USZ_MAX (~(usz)0)
#define LT_USZ_MIN ((usz)0)

#define LT_ISZ_MAX (-~(isz)0)
#define LT_ISZ_MIN (~(isz)0)

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

#define LT_U8_MAX (~(u8)0)
#define LT_U16_MAX (~(u16)0)
#define LT_U32_MAX (~(u32)0)
#define LT_U64_MAX (~(u64)0)

#define LT_U8_MIN ((u8)0)
#define LT_U16_MIN ((u16)0)
#define LT_U32_MIN ((u32)0)
#define LT_U64_MIN ((u64)0)

typedef int8_t i8;
typedef int16_t i16;
typedef int32_t i32;
typedef int64_t i64;

#define LT_I8_MAX (-~(i8)0)
#define LT_I16_MAX (-~(i16)0)
#define LT_I32_MAX (-~(i32)0)
#define LT_I64_MAX (-~(i64)0)

#define LT_I8_MIN (~(i8)0)
#define LT_I32_MIN (~(i32)0)
#define LT_I64_MIN (~(i64)0)
#define LT_I64_MIN (~(i64)0)

typedef float f32;
typedef double f64;

typedef u8 b8;

#endif
