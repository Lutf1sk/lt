#ifndef LT_LINALG_H
#define LT_LINALG_H 1

#include <lt/primitives.h>

typedef
struct lt_vec2 {
	f32 x, y;
} lt_vec2_t;

typedef
struct lt_vec3 {
	f32 x, y, z;
} lt_vec3_t;

typedef
struct lt_vec4 {
	f32 x, y, z, w;
} lt_vec4_t;

typedef
struct lt_mat2 {
	lt_vec2_t i, j;
} lt_mat2_t;

typedef
struct lt_mat3 {
	lt_vec3_t i, j, k;
} lt_mat3_t;

typedef
struct lt_mat4 {
	lt_vec4_t i, j, k, l;
} lt_mat4_t;

// vec.c

#define LT_VEC2(x, y) ((lt_vec2_t){ (x), (y) })
#define LT_VEC3(x, y, z) ((lt_vec3_t){ (x), (y), (z) })
#define LT_VEC4(x, y, z, w) ((lt_vec4_t){ (x), (y), (z), (w) })

#include <lt/linalg.h>

// ----- vec2

void lt_vec2_add(lt_vec2_t* dst, lt_vec2_t* src);
void lt_vec2_sub(lt_vec2_t* dst, lt_vec2_t* src);
void lt_vec2_div(lt_vec2_t* dst, lt_vec2_t* src);
void lt_vec2_mul(lt_vec2_t* dst, lt_vec2_t* src);

void lt_vec2_add_f(lt_vec2_t* v, float f);
void lt_vec2_sub_f(lt_vec2_t* v, float f);
void lt_vec2_div_f(lt_vec2_t* v, float f);
void lt_vec2_mul_f(lt_vec2_t* v, float f);

void lt_vec2_mul_mat2(lt_vec2_t* v, lt_mat2_t* m);

// ----- vec3

void lt_vec3_add(lt_vec3_t* dst, lt_vec3_t* src);
void lt_vec3_sub(lt_vec3_t* dst, lt_vec3_t* src);
void lt_vec3_div(lt_vec3_t* dst, lt_vec3_t* src);
void lt_vec3_mul(lt_vec3_t* dst, lt_vec3_t* src);

void lt_vec3_add_f(lt_vec3_t* v, float f);
void lt_vec3_sub_f(lt_vec3_t* v, float f);
void lt_vec3_div_f(lt_vec3_t* v, float f);
void lt_vec3_mul_f(lt_vec3_t* v, float f);

void lt_vec3_mul_mat3(lt_vec3_t* v, lt_mat3_t* m);

// ----- vec4

void lt_vec4_add(lt_vec4_t* dst, lt_vec4_t* src);
void lt_vec4_sub(lt_vec4_t* dst, lt_vec4_t* src);
void lt_vec4_div(lt_vec4_t* dst, lt_vec4_t* src);
void lt_vec4_mul(lt_vec4_t* dst, lt_vec4_t* src);

void lt_vec4_add_f(lt_vec4_t* v, float f);
void lt_vec4_sub_f(lt_vec4_t* v, float f);
void lt_vec4_div_f(lt_vec4_t* v, float f);
void lt_vec4_mul_f(lt_vec4_t* v, float f);

void lt_vec4_mul_mat4(lt_vec4_t* v, lt_mat4_t* m);

// matrix.c

void lt_mat2_identity(lt_mat2_t* mat);
void lt_mat3_identity(lt_mat3_t* mat);
void lt_mat4_identity(lt_mat4_t* mat);

void lt_mat2_mul(lt_mat2_t* dst, lt_mat2_t* src);
void lt_mat3_mul(lt_mat3_t* dst, lt_mat3_t* src);
void lt_mat4_mul(lt_mat4_t* dst, lt_mat4_t* src);

void lt_mat4_projection(lt_mat4_t* mat, int width, int height, f32 fov, f32 near, f32 far);

#define LT_MAT2_IDENTITY { \
	{ 1.0f, 0.0f }, \
	{ 0.0f, 1.0f }, \
}

#define LT_MAT3_IDENTITY { \
	{ 1.0f, 0.0f, 0.0f }, \
	{ 0.0f, 1.0f, 0.0f }, \
	{ 0.0f, 0.0f, 1.0f }, \
}

#define LT_MAT4_IDENTITY { \
	{ 1.0f, 0.0f, 0.0f, 0.0f }, \
	{ 0.0f, 1.0f, 0.0f, 0.0f }, \
	{ 0.0f, 0.0f, 1.0f, 0.0f }, \
	{ 0.0f, 0.0f, 0.0f, 1.0f }, \
}

#endif
