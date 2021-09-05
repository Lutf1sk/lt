#ifndef LT_LINALG_H
#define LT_LINALG_H 1

#include <lt/primitives.h>

// vec.c
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

#define LT_VEC2(x, y) ((lt_vec2_t){ (x), (y) })
#define LT_VEC3(x, y, z) ((lt_vec3_t){ (x), (y), (z) })
#define LT_VEC4(x, y, z, w) ((lt_vec4_t){ (x), (y), (z), (w) })

// matrix.c
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

void lt_mat2_identity(lt_mat2_t* mat);
void lt_mat3_identity(lt_mat3_t* mat);
void lt_mat4_identity(lt_mat4_t* mat);

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
