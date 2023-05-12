#ifndef LT_LINALG_H
#define LT_LINALG_H 1

#include <lt/primitives.h>
#include <lt/mem.h>
#include <lt/lt.h>

#include <math.h>

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

typedef
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

typedef
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
f32 lt_v2magnitude(lt_vec2_t v) {
	return sqrt(v.x * v.x + v.y * v.y);
}

static LT_INLINE
lt_vec2_t lt_v2normalize(lt_vec2_t v) {
	f32 m = lt_v2magnitude(v);
	if (m == 0.0f)
		return LT_VEC2(0.0f, 0.0f);
	return lt_v2mulf(v, 1.0f / m);
}

static LT_INLINE
float lt_v2cross(lt_vec2_t a, lt_vec2_t b) {
	return a.x * b.y - a.y * b.x;
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
f32 lt_v3magnitude(lt_vec3_t v) {
	return sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
}

static LT_INLINE
lt_vec3_t lt_v3normalize(lt_vec3_t v) {
	f32 m = lt_v3magnitude(v);
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

	ax = lt_v3normalize(axis);
	v1 = lt_v3mulf(v, x);
	v2 = lt_v3mulf(lt_v3cross(ax, v), y);

	v1 = lt_v3add(v1, v2);
	v2 = lt_v3mulf(ax, lt_v3dot(ax, v) * (1.0f - x));

	return lt_v3add(v1, v2);
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
f32 lt_v4magnitude(lt_vec4_t v) {
	float h = sqrt(v.x * v.x + v.y * v.y);
	float h2 = sqrt(v.z * v.z + h * h);
	return sqrt(v.w * v.w + h2 * h2);
}

static LT_INLINE
lt_vec4_t lt_v4normalize(lt_vec4_t v) {
	f32 m = lt_v4magnitude(v);
	if (m == 0.0f)
		return LT_VEC4(0.0f, 0.0f, 0.0f, 0.0f);
	return lt_v4mulf(v, 1.0f / m);
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

#define lt_vmagnitude(a) (_Generic((a), \
		lt_vec2_t: lt_v2magnitude, \
		lt_vec3_t: lt_v3magnitude, \
		lt_vec4_t: lt_v4magnitude \
	)((a)))

#define lt_vnormalize(a) (_Generic((a), \
		lt_vec2_t: lt_v2normalize, \
		lt_vec3_t: lt_v3normalize, \
		lt_vec4_t: lt_v4normalize \
	)((a)))

#define lt_vcross(a, b) (_Generic((a), \
		lt_vec2_t: lt_v2cross, \
		lt_vec3_t: lt_v3cross \
	)((a), (b)))

#define lt_vproject(a, b) (_Generic((a), \
		lt_vec3_t: lt_v3project \
	)((a), (b)))

#define lt_vpproject(a, b) (_Generic((a), \
		lt_vec3_t: lt_v3pproject \
	)((a), (b)))

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

#	define vdot lt_vdot
#	define vcross lt_vcross
#	define vmagnitude lt_vmagnitude
#	define vnormalize lt_vnormalize

#	define vproject lt_vproject
#	define vpproject lt_vpproject

#	define m4perspective lt_m4perspective
#	define m4ortho lt_m4ortho
#	define m4view lt_m4view
#	define m4euler lt_m4euler
#	define m4translate lt_m4translate
#	define m4inverse lt_m4inverse

#	define m2identity lt_m2identity
#	define m3identity lt_m3identity
#	define m4identity lt_m4identity

#	define m2mul lt_m2mul
#	define m3mul lt_m3mul
#	define m4mul lt_m4mul
#endif

// matrix.c

static LT_INLINE
void lt_m2identity(lt_mat2_t* mat) {
	static lt_mat2_t src = LT_MAT2_IDENTITY_INIT;
	memcpy(mat, &src, sizeof(lt_mat2_t));
}

static LT_INLINE
void lt_m3identity(lt_mat3_t* mat) {
	static lt_mat3_t src = LT_MAT3_IDENTITY_INIT;
	memcpy(mat, &src, sizeof(lt_mat3_t));
}

static LT_INLINE
void lt_m4identity(lt_mat4_t* mat) {
	static lt_mat4_t src = LT_MAT4_IDENTITY_INIT;
	memcpy(mat, &src, sizeof(lt_mat4_t));
}

static LT_INLINE
lt_mat2_t lt_m2mul(const lt_mat2_t* m1, const lt_mat2_t* m2) {
	return LT_MAT2V(
		lt_v2mulm(m1->i, m2),
		lt_v2mulm(m1->j, m2)
	);
}

static LT_INLINE
lt_mat3_t lt_m3mul(const lt_mat3_t* m1, const lt_mat3_t* m2) {
	return LT_MAT3V(
		lt_v3mulm(m1->i, m2),
		lt_v3mulm(m1->j, m2),
		lt_v3mulm(m1->k, m2)
	);
}

static LT_INLINE
lt_mat4_t lt_m4mul(const lt_mat4_t* m1, const lt_mat4_t* m2) {
	return LT_MAT4V(
		lt_v4mulm(m1->i, m2),
		lt_v4mulm(m1->j, m2),
		lt_v4mulm(m1->k, m2),
		lt_v4mulm(m1->l, m2)
	);
}

lt_mat4_t lt_m4perspective(f32 fov, f32 aspect, f32 near, f32 far);
lt_mat4_t lt_m4ortho(f32 left, f32 right, f32 bottom, f32 top, f32 near, f32 far);

lt_mat4_t lt_m4view(lt_vec3_t pos, lt_vec3_t fwd, lt_vec3_t up);
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

lt_mat4_t lt_m4inverse(const lt_mat4_t* m);

#endif
