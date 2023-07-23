#ifndef LT_LINALG_H
#define LT_LINALG_H 1

#include <lt/primitives.h>
#include <lt/mem.h>
#include <lt/lt.h>
#include <lt/math.h>

#include <math.h>

// ----- Vector

typedef
union lt_vec2 {
	struct { float x, y; };
	struct { float u, v; };
	float data[2];
} lt_vec2_t;

typedef
union lt_vec3 {
	struct { float x, y, z; };
	struct { float r, g, b; };
	float data[3];

	// Using this could be dangerous, since the layout is technically undefined
	lt_vec2_t xy, rg;
} lt_vec3_t;

typedef
LT_ALIGN(16)
union lt_vec4 {
	struct { float x, y, z, w; };
	struct { float r, g, b, a; };
	float data[4];

	// Using these could be dangerous, since the layout is technically undefined
	lt_vec2_t xy, rg;
	lt_vec3_t xyz, rgb;
} lt_vec4_t;

#define LT_VEC2_INIT(x, y) { .data = { (x), (y) } }
#define LT_VEC3_INIT(x, y, z) { .data = { (x), (y), (z) } }
#define LT_VEC4_INIT(x, y, z, w) { .data = { (x), (y), (z), (w) } }

#define LT_VEC2(x, y) ((lt_vec2_t)LT_VEC2_INIT(x, y))
#define LT_VEC3(x, y, z) ((lt_vec3_t)LT_VEC3_INIT(x, y, z))
#define LT_VEC4(x, y, z, w) ((lt_vec4_t)LT_VEC4_INIT(x, y, z, w))

// ----- Quaternion

typedef
union lt_quat {
	struct { float x, y, z, w; };
	float data[4];
} lt_quat_t;

#define LT_QUAT_INIT(x, y, z, w) { .data = { (x), (y), (z), (w) } }
#define LT_QUAT(x, y, z, w) ((lt_quat_t)LT_QUAT_INIT(x, y, z, w))

#define LT_QUAT_IDENTITY_INIT LT_QUAT_INIT(0.0f, 0.0f, 0.0f, 1.0f)
#define LT_QUAT_IDENTITY ((lt_quat_t)LT_QUAT_IDENTITY_INIT)

// ----- Matrix

typedef
LT_ALIGN(16)
union lt_mat2 {
	struct { lt_vec2_t i, j; };
	lt_vec2_t vdata[2];
	float data[2][2];
} lt_mat2_t;

typedef
union lt_mat3 {
	struct { lt_vec3_t i, j, k; };
	lt_vec3_t vdata[3];
	float data[3][3];
} lt_mat3_t;

#ifdef LT_AVX
#	define LT_MAT4_ALIGN LT_ALIGN(32)
#else
#	define LT_MAT4_ALIGN LT_ALIGN(16)
#endif

typedef
LT_MAT4_ALIGN
union lt_mat4 {
	struct { lt_vec4_t i, j, k, l; };
	lt_vec4_t vdata[4];
	float data[4][4];
} lt_mat4_t;

#define LT_MAT2_INIT(i1, i2, j1, j2) { .data = { \
	{ (i1), (i2) }, \
	{ (j1), (j2) }, \
}}
#define LT_MAT3_INIT(i1, i2, i3, j1, j2, j3, k1, k2, k3) { .data = { \
	{ (i1), (i2), (i3) }, \
	{ (j1), (j2), (j3) }, \
	{ (k1), (k2), (k3) }, \
}}
#define LT_MAT4_INIT(i1, i2, i3, i4, j1, j2, j3, j4, k1, k2, k3, k4, l1, l2, l3, l4) { .data = { \
	{ (i1), (i2), (i3), (i4) }, \
	{ (j1), (j2), (j3), (j4) }, \
	{ (k1), (k2), (k3), (k4) }, \
	{ (l1), (l2), (l3), (l4) }, \
}}

#define LT_MAT2(i1, i2, j1, j2) ((lt_mat2_t)LT_MAT2_INIT(i1, i2, j1, j2))
#define LT_MAT3(i1, i2, i3, j1, j2, j3, k1, k2, k3) ((lt_mat3_t)LT_MAT3_INIT(i1, i2, i3, j1, j2, j3, k1, k2, k3))
#define LT_MAT4(i1, i2, i3, i4, j1, j2, j3, j4, k1, k2, k3, k4, l1, l2, l3, l4) ((lt_mat4_t)LT_MAT4_INIT(i1, i2, i3, i4, j1, j2, j3, j4, k1, k2, k3, k4, l1, l2, l3, l4))

#define LT_MAT2V_INIT(i, j) { .vdata = { (i), (j) } }
#define LT_MAT3V_INIT(i, j, k) { .vdata = { (i), (j), (k) } }
#define LT_MAT4V_INIT(i, j, k, l) { .vdata = { (i), (j), (k), (l) } }

#define LT_MAT2V(i, j) ((lt_mat2_t)LT_MAT2V_INIT(i, j))
#define LT_MAT3V(i, j, k) ((lt_mat3_t)LT_MAT3V_INIT(i, j, k))
#define LT_MAT4V(i, j, k, l) ((lt_mat4_t)LT_MAT4V_INIT(i, j, k, l))

#define LT_MAT2_IDENTITY_INIT LT_MAT2_INIT(	1.0f, 0.0f, \
											0.0f, 1.0f)
#define LT_MAT3_IDENTITY_INIT LT_MAT3_INIT(	1.0f, 0.0f, 0.0f, \
											0.0f, 1.0f, 0.0f, \
											0.0f, 0.0f, 1.0f)

#define LT_MAT4_IDENTITY_INIT LT_MAT4_INIT(	1.0f, 0.0f, 0.0f, 0.0f, \
											0.0f, 1.0f, 0.0f, 0.0f, \
											0.0f, 0.0f, 1.0f, 0.0f, \
											0.0f, 0.0f, 0.0f, 1.0f)
#define LT_MAT2_IDENTITY ((lt_mat2_t)LT_MAT2_IDENTITY_INIT)
#define LT_MAT3_IDENTITY ((lt_mat3_t)LT_MAT2_IDENTITY_INIT)
#define LT_MAT4_IDENTITY ((lt_mat4_t)LT_MAT4_IDENTITY_INIT)

// Forward declarations
static
lt_vec4_t lt_v4mulm(lt_vec4_t v, const lt_mat4_t* m);

// ----- vec2

static LT_INLINE
lt_vec2_t lt_v2add(lt_vec2_t a, lt_vec2_t b) {
	return LT_VEC2(a.x + b.x, a.y + b.y);
}

static LT_INLINE
lt_vec2_t lt_v2sub(lt_vec2_t a, lt_vec2_t b) {
	return LT_VEC2(a.x - b.x, a.y - b.y);
}

static LT_INLINE
lt_vec2_t lt_v2div(lt_vec2_t a, lt_vec2_t b) {
	return LT_VEC2(a.x / b.x, a.y / b.y);
}

static LT_INLINE
lt_vec2_t lt_v2mul(lt_vec2_t a, lt_vec2_t b) {
	return LT_VEC2(a.x * b.x, a.y * b.y);
}

static LT_INLINE
lt_vec2_t lt_v2addf(lt_vec2_t a, float f) {
	return LT_VEC2(a.x + f, a.y + f);
}

static LT_INLINE
lt_vec2_t lt_v2subf(lt_vec2_t a, float f) {
	return LT_VEC2(a.x - f, a.y - f);
}

static LT_INLINE
lt_vec2_t lt_v2divf(lt_vec2_t a, float f) {
	return LT_VEC2(a.x / f, a.y / f);
}

static LT_INLINE
lt_vec2_t lt_v2mulf(lt_vec2_t a, float f) {
	return LT_VEC2(a.x * f, a.y * f);
}

static LT_INLINE
lt_vec2_t lt_v2mulm(lt_vec2_t v, const lt_mat2_t* m) {
	return LT_VEC2(
		m->data[0][0] * v.x + m->data[1][0] * v.y,
		m->data[0][1] * v.x + m->data[1][1] * v.y
	);
}

static LT_INLINE
f32 lt_v2dot(lt_vec2_t a, lt_vec2_t b) {
	return a.x * b.x + a.y * b.y;
}

static LT_INLINE
lt_vec2_t lt_v2neg(lt_vec2_t v) {
	return LT_VEC2(-v.x, -v.y);
}

static LT_INLINE
f32 lt_v2mag(lt_vec2_t v) {
	return sqrt(v.x * v.x + v.y * v.y);
}

static LT_INLINE
f32 lt_v2dist(lt_vec2_t a, lt_vec2_t b) {
	return lt_v2mag(lt_v2sub(a, b));
}

static LT_INLINE
lt_vec2_t lt_v2norm(lt_vec2_t v) {
	f32 m = lt_v2mag(v);
	if (m == 0.0f)
		return LT_VEC2(0.0f, 0.0f);
	return lt_v2mulf(v, 1.0f / m);
}

static LT_INLINE
f32 lt_v2cross(lt_vec2_t a, lt_vec2_t b) {
	return a.x * b.y - a.y * b.x;
}

static LT_INLINE
lt_vec2_t lt_v2max(lt_vec2_t a, lt_vec2_t b) {
	return LT_VEC2(lt_max_f32(a.x, b.x), lt_max_f32(a.y, b.y));
}

static LT_INLINE
lt_vec2_t lt_v2min(lt_vec2_t a, lt_vec2_t b) {
	return LT_VEC2(lt_min_f32(a.x, b.x), lt_min_f32(a.y, b.y));
}

static LT_INLINE
lt_vec2_t lt_v2clamp(lt_vec2_t v, lt_vec2_t min, lt_vec2_t max) {
	return LT_VEC2(lt_clamp_f32(v.x, min.x, max.x), lt_clamp_f32(v.y, min.y, max.y));
}

static LT_INLINE
lt_vec2_t lt_v2clampf(lt_vec2_t v, f32 min, f32 max) {
	return LT_VEC2(lt_clamp_f32(v.x, min, max), lt_clamp_f32(v.y, min, max));
}

static LT_INLINE
f32 lt_v2maxcomp(lt_vec2_t v) {
	return lt_max_f32(v.x, v.y);
}

static LT_INLINE
f32 lt_v2mincomp(lt_vec2_t v) {
	return lt_min_f32(v.x, v.y);
}

static LT_INLINE
lt_vec2_t lt_v2abs(lt_vec2_t v) {
	return LT_VEC2(fabs(v.x), fabs(v.y));
}

static LT_INLINE
int lt_v2equ(lt_vec2_t a, lt_vec2_t b, f32 epsilon) {
	return fabs(a.x - b.x) < epsilon && fabs(a.y - b.y) < epsilon;
}

static LT_INLINE
lt_vec2_t lt_v2lerp(lt_vec2_t a, lt_vec2_t b, f32 t) {
	return lt_v2add(lt_v2mulf(a, t), lt_v2mulf(b, 1.0f - t));
}

static LT_INLINE
lt_vec2_t lt_v2lerpc(lt_vec2_t a, lt_vec2_t b, f32 t) {
	return lt_v2lerp(a, b, lt_clamp_f32(t, 0.0f, 1.0f));
}

// ----- vec3

static LT_INLINE
lt_vec3_t lt_v3add(lt_vec3_t a, lt_vec3_t b) {
	return LT_VEC3(a.x + b.x, a.y + b.y, a.z + b.z);
}

static LT_INLINE
lt_vec3_t lt_v3sub(lt_vec3_t a, lt_vec3_t b) {
	return LT_VEC3(a.x - b.x, a.y - b.y, a.z - b.z);
}

static LT_INLINE
lt_vec3_t lt_v3div(lt_vec3_t a, lt_vec3_t b) {
	return LT_VEC3(a.x / b.x, a.y / b.y, a.z / b.z);
}

static LT_INLINE
lt_vec3_t lt_v3mul(lt_vec3_t a, lt_vec3_t b) {
	return LT_VEC3(a.x * b.x, a.y * b.y, a.z * b.z);
}

static LT_INLINE
lt_vec3_t lt_v3addf(lt_vec3_t a, float f) {
	return LT_VEC3(a.x + f, a.y + f, a.z + f);
}

static LT_INLINE
lt_vec3_t lt_v3subf(lt_vec3_t a, float f) {
	return LT_VEC3(a.x - f, a.y - f, a.z - f);
}

static LT_INLINE
lt_vec3_t lt_v3divf(lt_vec3_t a, float f) {
	return LT_VEC3(a.x / f, a.y / f, a.z / f);
}

static LT_INLINE
lt_vec3_t lt_v3mulf(lt_vec3_t a, float f) {
	return LT_VEC3(a.x * f, a.y * f, a.z * f);
}

static LT_INLINE
lt_vec3_t lt_v3mulm(lt_vec3_t v, const lt_mat3_t* m) {
	return LT_VEC3(
		m->data[0][0] * v.x + m->data[1][0] * v.y + m->data[2][0] * v.z,
		m->data[0][1] * v.x + m->data[1][1] * v.y + m->data[2][1] * v.z,
		m->data[0][2] * v.x + m->data[1][2] * v.y + m->data[2][2] * v.z
	);
}

static LT_INLINE
lt_vec3_t lt_v3mulm4(lt_vec3_t v, float w, const lt_mat4_t* m) {
	return lt_v4mulm(LT_VEC4(v.x, v.y, v.z, w), m).xyz;
}

static LT_INLINE
f32 lt_v3dot(lt_vec3_t a, lt_vec3_t b) {
	return a.x * b.x + a.y * b.y + a.z * b.z;
}

static LT_INLINE
lt_vec3_t lt_v3neg(lt_vec3_t v) {
	return LT_VEC3(-v.x, -v.y, -v.z);
}

static LT_INLINE
f32 lt_v3mag(lt_vec3_t v) {
	return sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
}

static LT_INLINE
f32 lt_v3dist(lt_vec3_t a, lt_vec3_t b) {
	return lt_v3mag(lt_v3sub(a, b));
}

static LT_INLINE
lt_vec3_t lt_v3norm(lt_vec3_t v) {
	f32 m = lt_v3mag(v);
	if (m == 0.0f)
		return LT_VEC3(0.0f, 0.0f, 0.0f);
	return lt_v3mulf(v, 1.0f / m);
}

static LT_INLINE
lt_vec3_t lt_v3cross(lt_vec3_t a, lt_vec3_t b) {
	return LT_VEC3(
		a.y * b.z - a.z * b.y,
		a.z * b.x - a.x * b.z,
		a.x * b.y - a.y * b.x
	);
}

static LT_INLINE
lt_vec3_t lt_v3crossn(lt_vec3_t a, lt_vec3_t b) {
	return lt_v3norm(lt_v3cross(a, b));
}

static LT_INLINE
lt_vec3_t lt_v3project(lt_vec3_t normal, lt_vec3_t point) {
	float nmag2 = lt_v3dot(normal, normal);
	if (nmag2 <= 0.0f)
		return point;

	float factor = lt_v3dot(point, normal) / nmag2;
	return lt_v3mulf(normal, factor);
}

static LT_INLINE
lt_vec3_t lt_v3pproject(lt_vec3_t normal, lt_vec3_t point) {
	float nmag2 = lt_v3dot(normal, normal);
	if (nmag2 <= 0.0f)
		return LT_VEC3(0.0f, 0.0f, 0.0f);

	float factor = lt_v3dot(point, normal) / nmag2;
	return LT_VEC3(
		point.x - normal.x * factor,
		point.y - normal.y * factor,
		point.z - normal.z * factor
	);
}

static LT_INLINE
lt_vec3_t lt_vec3_rotate(lt_vec3_t v, float angle, lt_vec3_t axis) {
	lt_vec3_t ax, v1, v2;
	float x = cos(angle), y = sin(angle);

	ax = lt_v3norm(axis);
	v1 = lt_v3mulf(v, x);
	v2 = lt_v3mulf(lt_v3cross(ax, v), y);

	v1 = lt_v3add(v1, v2);
	v2 = lt_v3mulf(ax, lt_v3dot(ax, v) * (1.0f - x));

	return lt_v3add(v1, v2);
}

static LT_INLINE
lt_vec3_t lt_v3max(lt_vec3_t a, lt_vec3_t b) {
	return LT_VEC3(lt_max_f32(a.x, b.x), lt_max_f32(a.y, b.y), lt_max_f32(a.z, b.z));
}

static LT_INLINE
lt_vec3_t lt_v3min(lt_vec3_t a, lt_vec3_t b) {
	return LT_VEC3(lt_min_f32(a.x, b.x), lt_min_f32(a.y, b.y), lt_min_f32(a.z, b.z));
}

static LT_INLINE
lt_vec3_t lt_v3clamp(lt_vec3_t v, lt_vec3_t min, lt_vec3_t max) {
	return LT_VEC3(lt_clamp_f32(v.x, min.x, max.x), lt_clamp_f32(v.y, min.y, max.y), lt_clamp_f32(v.z, min.z, max.z));
}

static LT_INLINE
lt_vec3_t lt_v3clampf(lt_vec3_t v, f32 min, f32 max) {
	return LT_VEC3(lt_clamp_f32(v.x, min, max), lt_clamp_f32(v.y, min, max), lt_clamp_f32(v.z, min, max));
}

static LT_INLINE
f32 lt_v3maxcomp(lt_vec3_t v) {
	return lt_max_f32(v.z, lt_max_f32(v.x, v.y));
}

static LT_INLINE
f32 lt_v3mincomp(lt_vec3_t v) {
	return lt_min_f32(v.z, lt_min_f32(v.x, v.y));
}

static LT_INLINE
lt_vec3_t lt_v3abs(lt_vec3_t v) {
	return LT_VEC3(fabs(v.x), fabs(v.y), fabs(v.z));
}

static LT_INLINE
int lt_v3equ(lt_vec3_t a, lt_vec3_t b, f32 epsilon) {
	return fabs(a.x - b.x) < epsilon && fabs(a.y - b.y) < epsilon && fabs(a.z - b.z) < epsilon;
}

static LT_INLINE
lt_vec3_t lt_v3lerp(lt_vec3_t a, lt_vec3_t b, f32 t) {
	return lt_v3add(lt_v3mulf(a, t), lt_v3mulf(b, 1.0f - t));
}

static LT_INLINE
lt_vec3_t lt_v3lerpc(lt_vec3_t a, lt_vec3_t b, f32 t) {
	return lt_v3lerp(a, b, lt_clamp_f32(t, 0.0f, 1.0f));
}

// ----- vec4

static LT_INLINE
lt_vec4_t lt_v4add(lt_vec4_t a, lt_vec4_t b) {
	return LT_VEC4(a.x + b.x, a.y + b.y, a.z + b.z, a.w + b.w);
}

static LT_INLINE
lt_vec4_t lt_v4sub(lt_vec4_t a, lt_vec4_t b) {
	return LT_VEC4(a.x - b.x, a.y - b.y, a.z - b.z, a.w - b.w);
}

static LT_INLINE
lt_vec4_t lt_v4div(lt_vec4_t a, lt_vec4_t b) {
	return LT_VEC4(a.x / b.x, a.y / b.y, a.z / b.z, a.w / b.w);
}

static LT_INLINE
lt_vec4_t lt_v4mul(lt_vec4_t a, lt_vec4_t b) {
	return LT_VEC4(a.x * b.x, a.y * b.y, a.z * b.z, a.w * b.w);
}

static LT_INLINE
lt_vec4_t lt_v4addf(lt_vec4_t a, float f) {
	return LT_VEC4(a.x + f, a.y + f, a.z + f, a.w + f);
}

static LT_INLINE
lt_vec4_t lt_v4subf(lt_vec4_t a, float f) {
	return LT_VEC4(a.x - f, a.y - f, a.z - f, a.w - f);
}

static LT_INLINE
lt_vec4_t lt_v4divf(lt_vec4_t a, float f) {
	return LT_VEC4(a.x / f, a.y / f, a.z / f, a.w / f);
}

static LT_INLINE
lt_vec4_t lt_v4mulf(lt_vec4_t a, float f) {
	return LT_VEC4(a.x * f, a.y * f, a.z * f, a.w * f);
}

static LT_INLINE
lt_vec4_t lt_v4mulm(lt_vec4_t v, const lt_mat4_t* m) {
	return LT_VEC4(
		m->data[0][0] * v.x + m->data[1][0] * v.y + m->data[2][0] * v.z + m->data[3][0] * v.w,
		m->data[0][1] * v.x + m->data[1][1] * v.y + m->data[2][1] * v.z + m->data[3][1] * v.w,
		m->data[0][2] * v.x + m->data[1][2] * v.y + m->data[2][2] * v.z + m->data[3][2] * v.w,
		m->data[0][3] * v.x + m->data[1][3] * v.y + m->data[2][3] * v.z + m->data[3][3] * v.w
	);
}

static LT_INLINE
f32 lt_v4dot(lt_vec4_t a, lt_vec4_t b) {
	return a.x * b.x + a.y * b.y + a.z * b.z + a.w * b.w;
}

static LT_INLINE
lt_vec4_t lt_v4neg(lt_vec4_t v) {
	return LT_VEC4(-v.x, -v.y, -v.z, -v.w);
}

static LT_INLINE
f32 lt_v4mag(lt_vec4_t v) {
	float h = sqrt(v.x * v.x + v.y * v.y);
	float h2 = sqrt(v.z * v.z + h * h);
	return sqrt(v.w * v.w + h2 * h2);
}

static LT_INLINE
f32 lt_v4dist(lt_vec4_t a, lt_vec4_t b) {
	return lt_v4mag(lt_v4sub(a, b));
}

static LT_INLINE
lt_vec4_t lt_v4norm(lt_vec4_t v) {
	f32 m = lt_v4mag(v);
	if (m == 0.0f)
		return LT_VEC4(0.0f, 0.0f, 0.0f, 0.0f);
	return lt_v4mulf(v, 1.0f / m);
}

static LT_INLINE
lt_vec4_t lt_v4max(lt_vec4_t a, lt_vec4_t b) {
	return LT_VEC4(lt_max_f32(a.x, b.x), lt_max_f32(a.y, b.y), lt_max_f32(a.z, b.z), lt_max_f32(a.w, b.w));
}

static LT_INLINE
lt_vec4_t lt_v4min(lt_vec4_t a, lt_vec4_t b) {
	return LT_VEC4(lt_min_f32(a.x, b.x), lt_min_f32(a.y, b.y), lt_min_f32(a.z, b.z), lt_min_f32(a.w, b.w));
}

static LT_INLINE
lt_vec4_t lt_v4clamp(lt_vec4_t v, lt_vec4_t min, lt_vec4_t max) {
	return LT_VEC4(lt_clamp_f32(v.x, min.x, max.x), lt_clamp_f32(v.y, min.y, max.y), lt_clamp_f32(v.z, min.z, max.z), lt_clamp_f32(v.w, min.w, max.w));
}

static LT_INLINE
lt_vec4_t lt_v4clampf(lt_vec4_t v, f32 min, f32 max) {
	return LT_VEC4(lt_clamp_f32(v.x, min, max), lt_clamp_f32(v.y, min, max), lt_clamp_f32(v.z, min, max), lt_clamp_f32(v.w, min, max));
}

static LT_INLINE
f32 lt_v4maxcomp(lt_vec4_t v) {
	return lt_max_f32(v.w, lt_max_f32(v.z, lt_max_f32(v.x, v.y)));
}

static LT_INLINE
f32 lt_v4mincomp(lt_vec4_t v) {
	return lt_min_f32(v.w, lt_min_f32(v.z, lt_min_f32(v.x, v.y)));
}

static LT_INLINE
lt_vec4_t lt_v4abs(lt_vec4_t v) {
	return LT_VEC4(fabs(v.x), fabs(v.y), fabs(v.z), fabs(v.w));
}

static LT_INLINE
int lt_v4equ(lt_vec4_t a, lt_vec4_t b, f32 epsilon) {
	return fabs(a.x - b.x) < epsilon && fabs(a.y - b.y) < epsilon && fabs(a.z - b.z) < epsilon && fabs(a.w - b.w) < epsilon;
}

static LT_INLINE
lt_vec4_t lt_v4lerp(lt_vec4_t a, lt_vec4_t b, f32 t) {
	return lt_v4add(lt_v4mulf(a, t), lt_v4mulf(b, 1.0f - t));
}

static LT_INLINE
lt_vec4_t lt_v4lerpc(lt_vec4_t a, lt_vec4_t b, f32 t) {
	return lt_v4lerp(a, b, lt_clamp_f32(t, 0.0f, 1.0f));
}

// ----- Generic

#define lt_vadd(a, b) (_Generic((a), \
		lt_vec2_t: lt_v2add, \
		lt_vec3_t: lt_v3add, \
		lt_vec4_t: lt_v4add \
	)((a), (b)))

#define lt_vsub(a, b) (_Generic((a), \
		lt_vec2_t: lt_v2sub, \
		lt_vec3_t: lt_v3sub, \
		lt_vec4_t: lt_v4sub \
	)((a), (b)))

#define lt_vdiv(a, b) (_Generic((a), \
		lt_vec2_t: lt_v2div, \
		lt_vec3_t: lt_v3div, \
		lt_vec4_t: lt_v4div \
	)((a), (b)))

#define lt_vmul(a, b) (_Generic((a), \
		lt_vec2_t: lt_v2mul, \
		lt_vec3_t: lt_v3mul, \
		lt_vec4_t: lt_v4mul \
	)((a), (b)))

#define lt_vaddf(a, b) (_Generic((a), \
		lt_vec2_t: lt_v2addf, \
		lt_vec3_t: lt_v3addf, \
		lt_vec4_t: lt_v4addf \
	)((a), (b)))

#define lt_vsubf(a, b) (_Generic((a), \
		lt_vec2_t: lt_v2subf, \
		lt_vec3_t: lt_v3subf, \
		lt_vec4_t: lt_v4subf \
	)((a), (b)))

#define lt_vdivf(a, b) (_Generic((a), \
		lt_vec2_t: lt_v2divf, \
		lt_vec3_t: lt_v3divf, \
		lt_vec4_t: lt_v4divf \
	)((a), (b)))

#define lt_vmulf(a, b) (_Generic((a), \
		lt_vec2_t: lt_v2mulf, \
		lt_vec3_t: lt_v3mulf, \
		lt_vec4_t: lt_v4mulf \
	)((a), (b)))

#define lt_vmulm(a, b) (_Generic((a), \
		lt_vec2_t: lt_v2mulm, \
		lt_vec3_t: lt_v3mulm, \
		lt_vec4_t: lt_v4mulm \
	)((a), (b)))

#define lt_vmulm4(a, w, m) (lt_v3mulm4((a), (w), (m)))

#define lt_vdot(a, b) (_Generic((a), \
		lt_vec2_t: lt_v2dot, \
		lt_vec3_t: lt_v3dot, \
		lt_vec4_t: lt_v4dot \
	)((a), (b)))

#define lt_vneg(a) (_Generic((a), \
		lt_vec2_t: lt_v2neg, \
		lt_vec3_t: lt_v3neg, \
		lt_vec4_t: lt_v4neg \
	)((a)))

#define lt_vmag(a) (_Generic((a), \
		lt_vec2_t: lt_v2mag, \
		lt_vec3_t: lt_v3mag, \
		lt_vec4_t: lt_v4mag \
	)((a)))

#define lt_vdist(a, b) (_Generic((a), \
		lt_vec2_t: lt_v2dist, \
		lt_vec3_t: lt_v3dist, \
		lt_vec4_t: lt_v4dist \
	)((a), (b)))

#define lt_vnorm(a) (_Generic((a), \
		lt_vec2_t: lt_v2norm, \
		lt_vec3_t: lt_v3norm, \
		lt_vec4_t: lt_v4norm \
	)((a)))

#define lt_vcross(a, b) (_Generic((a), \
		lt_vec2_t: lt_v2cross, \
		lt_vec3_t: lt_v3cross \
	)((a), (b)))

#define lt_vcrossn(a, b) (_Generic((a), \
		lt_vec3_t: lt_v3crossn \
	)((a), (b)))

#define lt_vproject(a, b) (_Generic((a), \
		lt_vec3_t: lt_v3project \
	)((a), (b)))

#define lt_vpproject(a, b) (_Generic((a), \
		lt_vec3_t: lt_v3pproject \
	)((a), (b)))

#define lt_vmax(a, b) (_Generic((a), \
		lt_vec2_t: lt_v2max, \
		lt_vec3_t: lt_v3max, \
		lt_vec4_t: lt_v4max \
	)((a), (b)))

#define lt_vmin(a, b) (_Generic((a), \
		lt_vec2_t: lt_v2min, \
		lt_vec3_t: lt_v3min, \
		lt_vec4_t: lt_v4min \
	)((a), (b)))

#define lt_vclamp(v, min, max) (_Generic((v), \
		lt_vec2_t: lt_v2clamp, \
		lt_vec3_t: lt_v3clamp, \
		lt_vec4_t: lt_v4clamp \
	)((v), (min), (max)))

#define lt_vclampf(v, min, max) (_Generic((v), \
		lt_vec2_t: lt_v2clampf, \
		lt_vec3_t: lt_v3clampf, \
		lt_vec4_t: lt_v4clampf \
	)((v), (min), (max)))

#define lt_vmaxcomp(a) (_Generic((a), \
		lt_vec2_t: lt_v2maxcomp, \
		lt_vec3_t: lt_v3maxcomp, \
		lt_vec4_t: lt_v4maxcomp \
	)((a)))

#define lt_vmincomp(a) (_Generic((a), \
		lt_vec2_t: lt_v2mincomp, \
		lt_vec3_t: lt_v3mincomp, \
		lt_vec4_t: lt_v4mincomp \
	)((a)))

#define lt_vabs(a) (_Generic((a), \
		lt_vec2_t: lt_v2abs, \
		lt_vec3_t: lt_v3abs, \
		lt_vec4_t: lt_v4abs \
	)((a)))

#define lt_vequ(a, b, c) (_Generic((a), \
		lt_vec2_t: lt_v2equ, \
		lt_vec3_t: lt_v3equ, \
		lt_vec4_t: lt_v4equ \
	)((a), (b), (c)))

// ----- quat

static LT_INLINE
lt_quat_t lt_qadd(lt_quat_t a, lt_quat_t b) {
	return LT_QUAT(a.x + b.x, a.y + b.y, a.z + b.z, a.w + b.w);
}

static LT_INLINE
lt_quat_t lt_qsub(lt_quat_t a, lt_quat_t b) {
	return LT_QUAT(a.x - b.x, a.y - b.y, a.z - b.z, a.w - b.w);
}

static LT_INLINE
lt_quat_t lt_qmul(lt_quat_t a, lt_quat_t b) {
	return LT_QUAT(
		a.w * b.x + a.x * b.w + a.y * b.z - a.z * b.y,
		a.w * b.y + a.y * b.w + a.z * b.x - a.x * b.z,
		a.w * b.z + a.z * b.w + a.x * b.y - a.y * b.x,
		a.w * b.w - a.x * b.x - a.y * b.y - a.z * b.z
	);
}

static LT_INLINE
lt_quat_t lt_qaddf(lt_quat_t q, f32 f) {
	return LT_QUAT(q.x + f, q.y + f, q.z + f, q.w + f);
}

static LT_INLINE
lt_quat_t lt_qsubf(lt_quat_t q, f32 f) {
	return LT_QUAT(q.x - f, q.y - f, q.z - f, q.w - f);
}

static LT_INLINE
lt_quat_t lt_qdivf(lt_quat_t q, f32 f) {
	return LT_QUAT(q.x / f, q.y / f, q.z / f, q.w / f);
}

static LT_INLINE
lt_quat_t lt_qmulf(lt_quat_t q, f32 f) {
	return LT_QUAT(q.x * f, q.y * f, q.z * f, q.w * f);
}

static LT_INLINE
f32 lt_qdot(lt_quat_t a, lt_quat_t b) {
	return a.x * b.x + a.y * b.y + a.z * b.z + a.w * b.w;
}

static LT_INLINE
lt_quat_t lt_qnorm(lt_quat_t q) {
	f32 mag = sqrt(lt_qdot(q, q));
	if (mag == 0.0f)
		return LT_QUAT(0.0f, 0.0f, 0.0f, 0.0f);
	return lt_qmulf(q, 1.0f / mag);
}

static LT_INLINE
int lt_qequ(lt_quat_t a, lt_quat_t b, f32 epsilon) {
	return fabs(a.x - b.x) < epsilon && fabs(a.y - b.y) < epsilon && fabs(a.z - b.z) < epsilon && fabs(a.w - b.w) < epsilon;
}

// ----- mat2

static LT_INLINE
void lt_m2identity(lt_mat2_t* mat) {
	static lt_mat2_t src = LT_MAT2_IDENTITY_INIT;
	memcpy(mat, &src, sizeof(lt_mat2_t));
}

static LT_INLINE
lt_mat2_t lt_m2mul(const lt_mat2_t* m1, const lt_mat2_t* m2) {
	return LT_MAT2V(
		lt_v2mulm(m1->i, m2),
		lt_v2mulm(m1->j, m2)
	);
}

// ----- mat3

static LT_INLINE
void lt_m3identity(lt_mat3_t* mat) {
	static lt_mat3_t src = LT_MAT3_IDENTITY_INIT;
	memcpy(mat, &src, sizeof(lt_mat3_t));
}

static LT_INLINE
lt_mat3_t lt_m3mul(const lt_mat3_t* m1, const lt_mat3_t* m2) {
	return LT_MAT3V(
		lt_v3mulm(m1->i, m2),
		lt_v3mulm(m1->j, m2),
		lt_v3mulm(m1->k, m2)
	);
}

lt_mat3_t lt_m3quat(lt_quat_t quat);

// ----- mat4

static LT_INLINE
void lt_m4identity(lt_mat4_t* mat) {
	static lt_mat4_t src = LT_MAT4_IDENTITY_INIT;
	memcpy(mat, &src, sizeof(lt_mat4_t));
}

static LT_INLINE
lt_mat4_t lt_m4mul(const lt_mat4_t* m1, const lt_mat4_t* m2) {
	return LT_MAT4V(
		lt_v4mulm(m2->i, m1),
		lt_v4mulm(m2->j, m1),
		lt_v4mulm(m2->k, m1),
		lt_v4mulm(m2->l, m1)
	);
}

lt_mat4_t lt_m4perspective(f32 fov, f32 aspect, f32 near, f32 far);
lt_mat4_t lt_m4ortho(f32 left, f32 right, f32 bottom, f32 top, f32 near, f32 far);

lt_mat4_t lt_m4view(lt_vec3_t pos, lt_vec3_t fwd, lt_vec3_t up);

lt_mat4_t lt_m4quat(lt_quat_t quat);
lt_mat4_t lt_m4euler(lt_vec3_t angles);

static LT_INLINE
lt_mat4_t lt_m4translate(lt_vec3_t pos) {
	return LT_MAT4V(
		LT_VEC4(1, 0, 0, 0),
		LT_VEC4(0, 1, 0, 0),
		LT_VEC4(0, 0, 1, 0),
		LT_VEC4(pos.x, pos.y, pos.z, 1)
	);
}

static LT_INLINE
lt_mat4_t lt_m4scale(lt_vec3_t scale) {
	return LT_MAT4V(
		LT_VEC4(scale.x, 0, 0, 0),
		LT_VEC4(0, scale.y, 0, 0),
		LT_VEC4(0, 0, scale.z, 0),
		LT_VEC4(0, 0, 0, 1)
	);
}

lt_mat4_t lt_m4model(lt_vec3_t pos, lt_vec3_t rot, lt_vec3_t scale);

lt_mat4_t lt_m4inverse(const lt_mat4_t* m);

// ----- Shortened names

#ifdef LT_LINALG_SHORTEN_NAMES
#	define vec2 lt_vec2_t
#	define vec3 lt_vec3_t
#	define vec4 lt_vec4_t

#	define mat2 lt_mat2_t
#	define mat3 lt_mat3_t
#	define mat4 lt_mat4_t

#	define vadd lt_vadd
#	define vsub lt_vsub
#	define vdiv lt_vdiv
#	define vmul lt_vmul
#	define vaddf lt_vaddf
#	define vsubf lt_vsubf
#	define vdivf lt_vdivf
#	define vmulf lt_vmulf
#	define vmulm lt_vmulm
#	define vmulm4 lt_vmulm4
#	define vmax lt_vmax
#	define vmin lt_vmin
#	define vclamp lt_vclamp
#	define vclampf lt_vclampf
#	define vmaxcomp lt_vmaxcomp
#	define vmincomp lt_vmincomp

#	define vdot lt_vdot
#	define vneg lt_vneg
#	define vcross lt_vcross
#	define vcrossn lt_vcrossn
#	define vmag lt_vmag
#	define vdist lt_vdist
#	define vnorm lt_vnorm
#	define vabs lt_vabs
#	define vequ lt_vequ

#	define vproject lt_vproject
#	define vpproject lt_vpproject

#	define qadd lt_qmul
#	define qsub lt_qsub
#	define qmul lt_qmul
#	define qaddf lt_qmulf
#	define qsubf lt_qsubf
#	define qdivf lt_qdivf
#	define qmulf lt_qmulf
#	define qdot lt_qdot
#	define qnorm lt_qnorm
#	define qequ lt_qequ

#	define m4identity lt_m4identity
#	define m4mul lt_m4mul
#	define m4perspective lt_m4perspective
#	define m4ortho lt_m4ortho
#	define m4view lt_m4view
#	define m4euler lt_m4euler
#	define m4quat lt_m4quat
#	define m4translate lt_m4translate
#	define m4scale lt_m4scale
#	define m4model lt_m4model
#	define m4inverse lt_m4inverse

#	define m2identity lt_m2identity
#	define m2mul lt_m2mul

#	define m3identity lt_m3identity
#	define m3mul lt_m3mul
#	define m3quat lt_m3quat

#	define VEC2_INIT(...) LT_VEC2_INIT(__VA_ARGS__)
#	define VEC3_INIT(...) LT_VEC3_INIT(__VA_ARGS__)
#	define VEC4_INIT(...) LT_VEC4_INIT(__VA_ARGS__)

#	define VEC2(...) LT_VEC2(__VA_ARGS__)
#	define VEC3(...) LT_VEC3(__VA_ARGS__)
#	define VEC4(...) LT_VEC4(__VA_ARGS__)

#	define QUAT_INIT(...) LT_QUAT_INIT(__VA_ARGS__)
#	define QUAT(...) LT_QUAT(__VA_ARGS__)

#	define MAT2_INIT(...) LT_MAT4_INIT(__VA_ARGS__)
#	define MAT3_INIT(...) LT_MAT3_INIT(__VA_ARGS__)
#	define MAT4_INIT(...) LT_MAT4_INIT(__VA_ARGS__)

#	define MAT2(...) LT_MAT4(__VA_ARGS__)
#	define MAT3(...) LT_MAT3(__VA_ARGS__)
#	define MAT4(...) LT_MAT4(__VA_ARGS__)
#endif

#endif
