#ifndef LT_LINALG_H
#define LT_LINALG_H 1

#include <lt/primitives.h>
#include <lt/mem.h>
#include <lt/lt.h>

typedef f32 lt_vec2_t[2];
typedef f32 lt_vec3_t[3];
typedef f32 lt_vec4_t[4];

typedef lt_vec2_t lt_mat2_t[2];
typedef lt_vec3_t lt_mat3_t[3];
typedef lt_vec4_t lt_mat4_t[4];

LT_INLINE
void lt_vec2_copy(lt_vec2_t dst, lt_vec2_t src) { memcpy(dst, src, sizeof(lt_vec2_t)); }
LT_INLINE
void lt_vec3_copy(lt_vec3_t dst, lt_vec3_t src) { memcpy(dst, src, sizeof(lt_vec3_t)); }
LT_INLINE
void lt_vec4_copy(lt_vec4_t dst, lt_vec4_t src) { memcpy(dst, src, sizeof(lt_vec4_t)); }

// vec.c

#define LT_VEC2(x, y) ((lt_vec2_t){ (x), (y) })
#define LT_VEC3(x, y, z) ((lt_vec3_t){ (x), (y), (z) })
#define LT_VEC4(x, y, z, w) ((lt_vec4_t){ (x), (y), (z), (w) })

#include <lt/linalg.h>

// ----- vec2

void lt_vec2_add(lt_vec2_t a, lt_vec2_t b, lt_vec2_t dst);
void lt_vec2_sub(lt_vec2_t a, lt_vec2_t b, lt_vec2_t dst);
void lt_vec2_div(lt_vec2_t a, lt_vec2_t b, lt_vec2_t dst);
void lt_vec2_mul(lt_vec2_t a, lt_vec2_t b, lt_vec2_t dst);

void lt_vec2_add_f(lt_vec2_t v, float f, lt_vec2_t dst);
void lt_vec2_sub_f(lt_vec2_t v, float f, lt_vec2_t dst);
void lt_vec2_div_f(lt_vec2_t v, float f, lt_vec2_t dst);
void lt_vec2_mul_f(lt_vec2_t v, float f, lt_vec2_t dst);

void lt_vec2_mul_mat2(lt_vec2_t v, lt_mat2_t m);

f32 lt_vec2_magnitude(lt_vec2_t v);

void lt_vec2_normalize(lt_vec2_t v);

// ----- vec3

void lt_vec3_add(lt_vec3_t a, lt_vec3_t b, lt_vec3_t dst);
void lt_vec3_sub(lt_vec3_t a, lt_vec3_t b, lt_vec3_t dst);
void lt_vec3_div(lt_vec3_t a, lt_vec3_t b, lt_vec3_t dst);
void lt_vec3_mul(lt_vec3_t a, lt_vec3_t b, lt_vec3_t dst);

void lt_vec3_add_f(lt_vec3_t v, float f, lt_vec3_t dst);
void lt_vec3_sub_f(lt_vec3_t v, float f, lt_vec3_t dst);
void lt_vec3_div_f(lt_vec3_t v, float f, lt_vec3_t dst);
void lt_vec3_mul_f(lt_vec3_t v, float f, lt_vec3_t dst);

void lt_vec3_mul_mat3(lt_vec3_t v, lt_mat3_t m);

f32 lt_vec3_magnitude(lt_vec3_t v);

void lt_vec3_normalize(lt_vec3_t v);

void lt_vec3_cross(lt_vec3_t a, lt_vec3_t b, lt_vec3_t dst);
f32 lt_vec3_dot(lt_vec3_t a, lt_vec3_t b);

// ----- vec4

void lt_vec4_add(lt_vec4_t a, lt_vec4_t b, lt_vec4_t dst);
void lt_vec4_sub(lt_vec4_t a, lt_vec4_t b, lt_vec4_t dst);
void lt_vec4_div(lt_vec4_t a, lt_vec4_t b, lt_vec4_t dst);
void lt_vec4_mul(lt_vec4_t a, lt_vec4_t b, lt_vec4_t dst);

void lt_vec4_add_f(lt_vec4_t v, float f, lt_vec4_t dst);
void lt_vec4_sub_f(lt_vec4_t v, float f, lt_vec4_t dst);
void lt_vec4_div_f(lt_vec4_t v, float f, lt_vec4_t dst);
void lt_vec4_mul_f(lt_vec4_t v, float f, lt_vec4_t dst);

void lt_vec4_mul_mat4(lt_vec4_t v, lt_mat4_t m);

f32 lt_vec4_magnitude(lt_vec4_t v);

void lt_vec4_normalize(lt_vec4_t v);

// matrix.c

void lt_mat2_identity(lt_mat2_t mat);
void lt_mat3_identity(lt_mat3_t mat);
void lt_mat4_identity(lt_mat4_t mat);

void lt_mat2_mul(lt_mat2_t dst, lt_mat2_t src);
void lt_mat3_mul(lt_mat3_t dst, lt_mat3_t src);
void lt_mat4_mul(lt_mat4_t dst, lt_mat4_t src);

void lt_mat4_perspective(lt_mat4_t mat, f32 fov, f32 aspect, f32 near, f32 far);
void lt_mat4_ortho(lt_mat4_t mat, f32 left, f32 right, f32 bottom, f32 top, f32 near, f32 far);

void lt_mat4_view(lt_mat4_t mat, lt_vec3_t eye, lt_vec3_t center, lt_vec3_t up);

#define LT_MAT2_IDENTITY ((lt_mat2_t)LT_MAT2_IDENTITY_INIT)
#define LT_MAT2_IDENTITY_INIT { \
	{ 1.0f, 0.0f }, \
	{ 0.0f, 1.0f }, \
}

#define LT_MAT3_IDENTITY ((lt_mat3_t)LT_MAT2_IDENTITY_INIT)
#define LT_MAT3_IDENTITY_INIT { \
	{ 1.0f, 0.0f, 0.0f }, \
	{ 0.0f, 1.0f, 0.0f }, \
	{ 0.0f, 0.0f, 1.0f }, \
}

#define LT_MAT4_IDENTITY ((lt_mat4_t)LT_MAT4_IDENTITY_INIT)
#define LT_MAT4_IDENTITY_INIT { \
	{ 1.0f, 0.0f, 0.0f, 0.0f }, \
	{ 0.0f, 1.0f, 0.0f, 0.0f }, \
	{ 0.0f, 0.0f, 1.0f, 0.0f }, \
	{ 0.0f, 0.0f, 0.0f, 1.0f }, \
}

#endif
