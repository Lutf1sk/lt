#ifndef LT_LINALG_H
#define LT_LINALG_H 1

#include <lt/primitives.h>
#include <lt/mem.h>
#include <lt/lt.h>

#include <math.h>

typedef f32 lt_vec2_t[2];
typedef f32 lt_vec3_t[3];
typedef f32 lt_vec4_t[4];

#define LT_VEC2_INIT(x, y) { (x), (y) }
#define LT_VEC3_INIT(x, y, z) { (x), (y), (z) }
#define LT_VEC4_INIT(x, y, z, w) { (x), (y), (z), (w) }

#define LT_VEC2(x, y) ((lt_vec2_t){ (x), (y) })
#define LT_VEC3(x, y, z) ((lt_vec3_t){ (x), (y), (z) })
#define LT_VEC4(x, y, z, w) ((lt_vec4_t){ (x), (y), (z), (w) })

typedef lt_vec2_t lt_mat2_t[2];
typedef lt_vec3_t lt_mat3_t[3];
typedef lt_vec4_t lt_mat4_t[4];

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

LT_INLINE
void lt_vec2_copy(lt_vec2_t dst, lt_vec2_t src) { memcpy(dst, src, sizeof(lt_vec2_t)); }
LT_INLINE
void lt_vec3_copy(lt_vec3_t dst, lt_vec3_t src) { memcpy(dst, src, sizeof(lt_vec3_t)); }
LT_INLINE
void lt_vec4_copy(lt_vec4_t dst, lt_vec4_t src) { memcpy(dst, src, sizeof(lt_vec4_t)); }

// Forward declarations
static
void lt_vec4_mul_mat4(lt_vec4_t v, lt_mat4_t m, lt_vec4_t dst);

// vec.c

// ----- vec2

static LT_INLINE
void lt_vec2_add(lt_vec2_t a, lt_vec2_t b, lt_vec2_t dst) {
	dst[0] = a[0] + b[0];
	dst[1] = a[1] + b[1];
}

static LT_INLINE
void lt_vec2_sub(lt_vec2_t a, lt_vec2_t b, lt_vec2_t dst) {
	dst[0] = a[0] - b[0];
	dst[1] = a[1] - b[1];
}

static LT_INLINE
void lt_vec2_div(lt_vec2_t a, lt_vec2_t b, lt_vec2_t dst) {
	dst[0] = a[0] / b[0];
	dst[1] = a[1] / b[1];
}

static LT_INLINE
void lt_vec2_mul(lt_vec2_t a, lt_vec2_t b, lt_vec2_t dst) {
	dst[0] = a[0] * b[0];
	dst[1] = a[1] * b[1];
}

static LT_INLINE
void lt_vec2_add_f(lt_vec2_t v, float f, lt_vec2_t dst) {
	dst[0] = v[0] + f;
	dst[1] = v[1] + f;
}

static LT_INLINE
void lt_vec2_sub_f(lt_vec2_t v, float f, lt_vec2_t dst) {
	dst[0] = v[0] - f;
	dst[1] = v[1] - f;
}

static LT_INLINE
void lt_vec2_div_f(lt_vec2_t v, float f, lt_vec2_t dst) {
	dst[0] = v[0] / f;
	dst[1] = v[1] / f;
}

static LT_INLINE
void lt_vec2_mul_f(lt_vec2_t v, float f, lt_vec2_t dst) {
	dst[0] = v[0] * f;
	dst[1] = v[1] * f;
}

static LT_INLINE
void lt_vec2_mul_mat2(lt_vec2_t v, lt_mat2_t m, lt_vec2_t dst) {
	lt_vec2_t tmp = LT_VEC2_INIT(
		m[0][0] * v[0] + m[1][0] * v[1],
		m[0][1] * v[0] + m[1][1] * v[1]
	);
	lt_vec2_copy(dst, tmp);
}

static LT_INLINE
f32 lt_vec2_magnitude(lt_vec2_t v) {
	return sqrt(v[0] * v[0] + v[1] * v[1]);
}

static LT_INLINE
void lt_vec2_normalize(lt_vec2_t v) {
	f32 m = lt_vec2_magnitude(v);

	if (m == 0.0f)
		v[0] = 0.0f, v[1] = 0.0f;
	else
		lt_vec2_mul_f(v, 1.0f / m, v);
}

// ----- vec3

#include <math.h>

static LT_INLINE
void lt_vec3_add(lt_vec3_t a, lt_vec3_t b, lt_vec3_t dst) {
	dst[0] = a[0] + b[0];
	dst[1] = a[1] + b[1];
	dst[2] = a[2] + b[2];
}

static LT_INLINE
void lt_vec3_sub(lt_vec3_t a, lt_vec3_t b, lt_vec3_t dst) {
	dst[0] = a[0] - b[0];
	dst[1] = a[1] - b[1];
	dst[2] = a[2] - b[2];
}

static LT_INLINE
void lt_vec3_div(lt_vec3_t a, lt_vec3_t b, lt_vec3_t dst) {
	dst[0] = a[0] / b[0];
	dst[1] = a[1] / b[1];
	dst[2] = a[2] / b[2];
}

static LT_INLINE
void lt_vec3_mul(lt_vec3_t a, lt_vec3_t b, lt_vec3_t dst) {
	dst[0] = a[0] * b[0];
	dst[1] = a[1] * b[1];
	dst[2] = a[2] * b[2];
}

static LT_INLINE
void lt_vec3_add_f(lt_vec3_t v, float f, lt_vec3_t dst) {
	dst[0] = v[0] + f;
	dst[1] = v[1] + f;
	dst[2] = v[2] + f;
}

static LT_INLINE
void lt_vec3_sub_f(lt_vec3_t v, float f, lt_vec3_t dst) {
	dst[0] = v[0] - f;
	dst[1] = v[1] - f;
	dst[2] = v[2] - f;
}

static LT_INLINE
void lt_vec3_div_f(lt_vec3_t v, float f, lt_vec3_t dst) {
	dst[0] = v[0] / f;
	dst[1] = v[1] / f;
	dst[2] = v[2] / f;
}

static LT_INLINE
void lt_vec3_mul_f(lt_vec3_t v, float f, lt_vec3_t dst) {
	dst[0] = v[0] * f;
	dst[1] = v[1] * f;
	dst[2] = v[2] * f;
}

static LT_INLINE
void lt_vec3_mul_mat3(lt_vec3_t v, lt_mat3_t m, lt_vec3_t dst) {
	lt_vec3_t tmp = LT_VEC3_INIT(
		m[0][0] * v[0] + m[1][0] * v[1] + m[2][0] * v[2],
		m[0][1] * v[0] + m[1][1] * v[1] + m[2][1] * v[2],
		m[0][2] * v[0] + m[1][2] * v[1] + m[2][2] * v[2]
	);
	lt_vec3_copy(dst, tmp);
}

static LT_INLINE
void lt_vec3_mul_mat4(lt_vec3_t v, lt_mat4_t m, lt_vec3_t dst) {
	lt_vec4_t tmp = LT_VEC4_INIT(v[0], v[1], v[2], 1.0f);
	lt_vec4_mul_mat4(tmp, m, tmp);
	lt_vec3_copy(dst, tmp);
}

static LT_INLINE
f32 lt_vec3_dot(lt_vec3_t a, lt_vec3_t b) {
	return a[0] * b[0] + a[1] * b[1] + a[2] * b[2];
}

static LT_INLINE
f32 lt_vec3_magnitude(lt_vec3_t v) {
	return sqrt(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]);
}

static LT_INLINE
void lt_vec3_normalize(lt_vec3_t v) {
	f32 m = lt_vec3_magnitude(v);

	if (m == 0.0f)
		v[0] = 0.0f, v[1] = 0.0f, v[2] = 0.0f;
	else
		lt_vec3_mul_f(v, 1.0f / m, v);
}

static LT_INLINE
void lt_vec3_cross(lt_vec3_t a, lt_vec3_t b, lt_vec3_t dst) {
	lt_vec3_t tmp;
	tmp[0] = a[1] * b[2] - a[2] * b[1];
	tmp[1] = a[2] * b[0] - a[0] * b[2];
	tmp[2] = a[0] * b[1] - a[1] * b[0];
	lt_vec3_copy(dst, tmp);
}

static LT_INLINE
void lt_vec3_rotate(lt_vec3_t v, float angle, lt_vec3_t axis) {
	lt_vec3_t ax, v1, v2;
	float x = cos(angle), y = sin(angle);

	lt_vec3_copy(ax, axis);
	lt_vec3_normalize(ax);

	lt_vec3_mul_f(v, x, v1);

	lt_vec3_cross(ax, v, v2);
	lt_vec3_mul_f(v2, y, v2);

	lt_vec3_add(v1, v2, v1);

	lt_vec3_mul_f(ax, lt_vec3_dot(ax, v) * (1.0f - x), v2);
	lt_vec3_add(v1, v2, v);
}

// ----- vec4

static LT_INLINE
void lt_vec4_add(lt_vec4_t a, lt_vec4_t b, lt_vec4_t dst) {
	dst[0] = a[0] + b[0];
	dst[1] = a[1] + b[1];
	dst[2] = a[2] + b[2];
	dst[3] = a[3] + b[3];
}

static LT_INLINE
void lt_vec4_sub(lt_vec4_t a, lt_vec4_t b, lt_vec4_t dst) {
	dst[0] = a[0] - b[0];
	dst[1] = a[1] - b[1];
	dst[2] = a[2] - b[2];
	dst[3] = a[3] - b[3];
}

static LT_INLINE
void lt_vec4_div(lt_vec4_t a, lt_vec4_t b, lt_vec4_t dst) {
	dst[0] = a[0] / b[0];
	dst[1] = a[1] / b[1];
	dst[2] = a[2] / b[2];
	dst[3] = a[3] / b[3];
}

static LT_INLINE
void lt_vec4_mul(lt_vec4_t a, lt_vec4_t b, lt_vec4_t dst) {
	dst[0] = a[0] * b[0];
	dst[1] = a[1] * b[1];
	dst[2] = a[2] * b[2];
	dst[3] = a[3] * b[3];
}

static LT_INLINE
void lt_vec4_add_f(lt_vec4_t v, float f, lt_vec4_t dst) {
	dst[0] = v[0] + f;
	dst[1] = v[1] + f;
	dst[2] = v[2] + f;
	dst[3] = v[3] + f;
}

static LT_INLINE
void lt_vec4_sub_f(lt_vec4_t v, float f, lt_vec4_t dst) {
	dst[0] = v[0] - f;
	dst[1] = v[1] - f;
	dst[2] = v[2] - f;
	dst[3] = v[3] - f;
}

static LT_INLINE
void lt_vec4_div_f(lt_vec4_t v, float f, lt_vec4_t dst) {
	dst[0] = v[0] / f;
	dst[1] = v[1] / f;
	dst[2] = v[2] / f;
	dst[3] = v[3] / f;
}

static LT_INLINE
void lt_vec4_mul_f(lt_vec4_t v, float f, lt_vec4_t dst) {
	dst[0] = v[0] * f;
	dst[1] = v[1] * f;
	dst[2] = v[2] * f;
	dst[3] = v[3] * f;
}

static LT_INLINE
void lt_vec4_mul_mat4(lt_vec4_t v, lt_mat4_t m, lt_vec4_t dst) {
	lt_vec4_t tmp = LT_VEC4_INIT(
		m[0][0] * v[0] + m[1][0] * v[1] + m[2][0] * v[2] + m[3][0] * v[3],
		m[0][1] * v[0] + m[1][1] * v[1] + m[2][1] * v[2] + m[3][1] * v[3],
		m[0][2] * v[0] + m[1][2] * v[1] + m[2][2] * v[2] + m[3][2] * v[3],
		m[0][3] * v[0] + m[1][3] * v[1] + m[2][3] * v[2] + m[3][3] * v[3]
	);
	lt_vec4_copy(dst, tmp);
}

static LT_INLINE
f32 lt_vec4_magnitude(lt_vec4_t v) {
	float h = sqrt(v[0] * v[0] + v[1] * v[1]);
	float h2 = sqrt(v[2] * v[2] + h * h);
	return sqrt(v[3] * v[3] + h2 * h2);
}

static LT_INLINE
void lt_vec4_normalize(lt_vec4_t v) {
	f32 m = lt_vec4_magnitude(v);

	if (m == 0.0f)
		v[0] = 0.0f, v[1] = 0.0f, v[2] = 0.0f, v[3] = 0.0f;
	else
		lt_vec4_mul_f(v, 1.0f / m, v);
}


// matrix.c

static LT_INLINE
void lt_mat2_identity(lt_mat2_t mat) {
	static lt_mat2_t src = LT_MAT2_IDENTITY_INIT;
	memcpy(mat, &src, sizeof(lt_mat2_t));
}

static LT_INLINE
void lt_mat3_identity(lt_mat3_t mat) {
	static lt_mat3_t src = LT_MAT3_IDENTITY_INIT;
	memcpy(mat, &src, sizeof(lt_mat3_t));
}

static LT_INLINE
void lt_mat4_identity(lt_mat4_t mat) {
	static lt_mat4_t src = LT_MAT4_IDENTITY_INIT;
	memcpy(mat, &src, sizeof(lt_mat4_t));
}

static LT_INLINE
void lt_mat2_mul(lt_mat2_t m1, lt_mat2_t m2, lt_mat2_t dst) {
	lt_vec2_mul_mat2(m1[0], m2, dst[0]);
	lt_vec2_mul_mat2(m1[1], m2, dst[1]);
}

static LT_INLINE
void lt_mat3_mul(lt_mat3_t m1, lt_mat3_t m2, lt_mat3_t dst) {
	lt_vec3_mul_mat3(m1[0], m2, dst[0]);
	lt_vec3_mul_mat3(m1[1], m2, dst[1]);
	lt_vec3_mul_mat3(m1[2], m2, dst[2]);
}

static LT_INLINE
void lt_mat4_mul(lt_mat4_t m1, lt_mat4_t m2, lt_mat4_t dst) {
	lt_vec4_mul_mat4(m1[0], m2, dst[0]);
	lt_vec4_mul_mat4(m1[1], m2, dst[1]);
	lt_vec4_mul_mat4(m1[2], m2, dst[2]);
	lt_vec4_mul_mat4(m1[3], m2, dst[3]);
}

void lt_mat4_perspective(lt_mat4_t mat, f32 fov, f32 aspect, f32 near, f32 far);
void lt_mat4_ortho(lt_mat4_t mat, f32 left, f32 right, f32 bottom, f32 top, f32 near, f32 far);

void lt_mat4_view(lt_mat4_t mat, lt_vec3_t pos, lt_vec3_t fwd, lt_vec3_t up);
void lt_mat4_euler(lt_vec3_t angles, lt_mat4_t dst);

static LT_INLINE
void lt_mat2_copy(lt_mat2_t dst, lt_mat2_t src) { memcpy(dst, src, sizeof(lt_mat2_t)); }
static LT_INLINE
void lt_mat3_copy(lt_mat3_t dst, lt_mat3_t src) { memcpy(dst, src, sizeof(lt_mat3_t)); }
static LT_INLINE
void lt_mat4_copy(lt_mat4_t dst, lt_mat4_t src) { memcpy(dst, src, sizeof(lt_mat4_t)); }

#endif
