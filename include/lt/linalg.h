#ifndef LT_LINALG_H
#define LT_LINALG_H 1

#include <lt/primitives.h>
#include <lt/mem.h>
#include <lt/lt.h>
#include <lt/math.h>

#include <math.h>

// vec2

typedef
union lt_vec2 {
	struct { f32 x, y; };
	struct { f32 u, v; };
	f32 data[2];
} lt_vec2_t;

#define LT_VEC2_INIT(x, y) { .data = { (x), (y) } }
#define LT_VEC2(x, y) ((lt_vec2_t){ .data = { (x), (y) } })

// vec3

typedef
union lt_vec3 {
	struct { f32 x, y, z; };
	struct { f32 r, g, b; };
	f32 data[3];

	// Using this could be dangerous, since the layout is technically undefined
	lt_vec2_t xy, rg;
} lt_vec3_t;

#define LT_VEC3_INIT(x, y, z) { .data = { (x), (y), (z) } }
#define LT_VEC3(x, y, z) ((lt_vec3_t)LT_VEC3_INIT(x, y, z))

// vec4

typedef
LT_ALIGN(16)
union lt_vec4 {
	struct { f32 x, y, z, w; };
	struct { f32 r, g, b, a; };
	f32 data[4];

	// Using these could be dangerous, since the layout is technically undefined
	lt_vec2_t xy, rg;
	lt_vec3_t xyz, rgb;
} lt_vec4_t;

#define LT_VEC4_INIT(x, y, z, w) { .data = { (x), (y), (z), (w) } }
#define LT_VEC4(x, y, z, w) ((lt_vec4_t)LT_VEC4_INIT(x, y, z, w))

// vec2i

typedef
union lt_vec2i {
	struct { i32 x, y; };
	struct { i32 u, v; };
	i32 data[2];
} lt_vec2i_t;

#define LT_VEC2I_INIT(x, y) { .data = { (x), (y) } }
#define LT_VEC2I(x, y) ((lt_vec2i_t){ .data = { (x), (y) } })

// vec3i

typedef
union lt_vec3i {
	struct { i32 x, y, z; };
	struct { i32 r, g, b; };
	i32 data[3];

	// Using this could be dangerous, since the layout is technically undefined
	lt_vec2i_t xy, rg;
} lt_vec3i_t;

#define LT_VEC3I_INIT(x, y, z) { .data = { (x), (y), (z) } }
#define LT_VEC3I(x, y, z) ((lt_vec3i_t){ .data = { (x), (y), (z) } })

// vec4i

typedef
LT_ALIGN(16)
union lt_vec4i {
	struct { i32 x, y, z, w; };
	struct { i32 r, g, b, a; };
	i32 data[4];

	// Using these could be dangerous, since the layout is technically undefined
	lt_vec2i_t xy, rg;
	lt_vec3i_t xyz, rgb;
} lt_vec4i_t;

#define LT_VEC4I_INIT(x, y, z, w) { .data = { (x), (y), (z), (w) } }
#define LT_VEC4I(x, y, z, w) ((lt_vec4i_t){ .data = { (x), (y), (z), (w) } })

// vec2d

typedef
union lt_vec2d {
	struct { f64 x, y; };
	struct { f64 u, v; };
	f64 data[2];
} lt_vec2d_t;

#define LT_VEC2D_INIT(x, y) { .data = { (x), (y) } }
#define LT_VEC2D(x, y) ((lt_vec2d_t){ .data = { (x), (y) } })

// vec3d

typedef
union lt_vec3d {
	struct { f64 x, y, z; };
	struct { f64 r, g, b; };
	f64 data[3];

	// Using this could be dangerous, since the layout is technically undefined
	lt_vec2d_t xy, rg;
} lt_vec3d_t;

#define LT_VEC3D_INIT(x, y, z) { .data = { (x), (y), (z) } }
#define LT_VEC3D(x, y, z) ((lt_vec3d_t){ .data = { (x), (y), (z) } })

// vec4d

typedef
LT_ALIGN(16)
union lt_vec4d {
	struct { f64 x, y, z, w; };
	struct { f64 r, g, b, a; };
	f64 data[4];

	// Using these could be dangerous, since the layout is technically undefined
	lt_vec2d_t xy, rg;
	lt_vec3d_t xyz, rgb;
} lt_vec4d_t;

#define LT_VEC4D_INIT(x, y, z, w) { .data = { (x), (y), (z), (w) } }
#define LT_VEC4D(x, y, z, w) ((lt_vec4d_t){ .data = { (x), (y), (z), (w) } })


#include <lt/linalg/vec2.h>
#include <lt/linalg/vec3.h>
#include <lt/linalg/vec4.h>

#include <lt/linalg/vec2i.h>
#include <lt/linalg/vec3i.h>
#include <lt/linalg/vec4i.h>

#include <lt/linalg/vec2d.h>
#include <lt/linalg/vec3d.h>
#include <lt/linalg/vec4d.h>

// quat

typedef
union lt_quat {
	struct { f32 x, y, z, w; };
	f32 data[4];
} lt_quat_t;

#define LT_QUAT_INIT(x, y, z, w) { .data = { (x), (y), (z), (w) } }
#define LT_QUAT(x, y, z, w) ((lt_quat_t){ .data = { (x), (y), (z), (w) } })

#define LT_QUAT_IDENTITY_INIT { .data = { 0.0f, 0.0f, 0.0f, 1.0f } }
#define LT_QUAT_IDENTITY ((lt_quat_t){ .data = { 0.0f, 0.0f, 0.0f, 1.0f } })

#include <lt/linalg/quat.h>

// mat2

typedef
LT_ALIGN(16)
union lt_mat2 {
	struct { lt_vec2_t i, j; };
	lt_vec2_t vdata[2];
	f32 data[2][2];
} lt_mat2_t;

#define LT_MAT2_INIT(i1, i2, j1, j2) { .data = { { (i1), (i2) }, { (j1), (j2) } } }
#define LT_MAT2(i1, i2, j1, j2) ((lt_mat2_t){ .data = { { (i1), (i2) }, { (j1), (j2) } } })
#define LT_MAT2V_INIT(i, j) { .vdata = { (i), (j) } }
#define LT_MAT2V(i, j) ((lt_mat2_t) { .vdata = { (i), (j) } })
#define LT_MAT2_IDENTITY_INIT { .data = { { 1.0f, 0.0f }, { 0.0f, 1.0f } } }
#define LT_MAT2_IDENTITY ((lt_mat2_t){ .data = { { 1.0f, 0.0f }, { 0.0f, 1.0f } } })

#include <lt/linalg/mat2.h>

// mat3

typedef
union lt_mat3 {
	struct { lt_vec3_t i, j, k; };
	lt_vec3_t vdata[3];
	f32 data[3][3];
} lt_mat3_t;

#define LT_MAT3_INIT(i1, i2, i3, j1, j2, j3, k1, k2, k3) { .data = { \
	{ (i1), (i2), (i3) }, \
	{ (j1), (j2), (j3) }, \
	{ (k1), (k2), (k3) } } }
#define LT_MAT3(i1, i2, i3, j1, j2, j3, k1, k2, k3) ((lt_mat3_t)LT_MAT3_INIT(i1, i2, i3, j1, j2, j3, k1, k2, k3))
#define LT_MAT3V_INIT(i, j, k) { .vdata = { (i), (j), (k) } }
#define LT_MAT3V(i, j, k) ((lt_mat3_t)LT_MAT3V_INIT(i, j, k))
#define LT_MAT3_IDENTITY_INIT LT_MAT3_INIT( \
	1.0f, 0.0f, 0.0f, \
	0.0f, 1.0f, 0.0f, \
	0.0f, 0.0f, 1.0f)
#define LT_MAT3_IDENTITY ((lt_mat3_t)LT_MAT3_IDENTITY_INIT)

#include <lt/linalg/mat3.h>

// mat4

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
	f32 data[4][4];
} lt_mat4_t;

#define LT_MAT4_INIT(i1, i2, i3, i4, j1, j2, j3, j4, k1, k2, k3, k4, l1, l2, l3, l4) { .data = { \
	{ (i1), (i2), (i3), (i4) }, \
	{ (j1), (j2), (j3), (j4) }, \
	{ (k1), (k2), (k3), (k4) }, \
	{ (l1), (l2), (l3), (l4) }, } }
#define LT_MAT4(i1, i2, i3, i4, j1, j2, j3, j4, k1, k2, k3, k4, l1, l2, l3, l4) ((lt_mat4_t)LT_MAT4_INIT(i1, i2, i3, i4, j1, j2, j3, j4, k1, k2, k3, k4, l1, l2, l3, l4))
#define LT_MAT4V_INIT(i, j, k, l) { .vdata = { (i), (j), (k), (l) } }
#define LT_MAT4V(i, j, k, l) ((lt_mat4_t)LT_MAT4V_INIT(i, j, k, l))
#define LT_MAT4_IDENTITY_INIT LT_MAT4_INIT( \
	1.0f, 0.0f, 0.0f, 0.0f, \
	0.0f, 1.0f, 0.0f, 0.0f, \
	0.0f, 0.0f, 1.0f, 0.0f, \
	0.0f, 0.0f, 0.0f, 1.0f)
#define LT_MAT4_IDENTITY ((lt_mat4_t)LT_MAT4_IDENTITY_INIT)

#include <lt/linalg/mat4.h>

// ----- Generic

#define lt_vadd(a, b) (_Generic((a), \
		lt_vec2_t: lt_v2add, \
		lt_vec3_t: lt_v3add, \
		lt_vec4_t: lt_v4add, \
		lt_vec2d_t: lt_v2dadd, \
		lt_vec3d_t: lt_v3dadd, \
		lt_vec4d_t: lt_v4dadd, \
		lt_vec2i_t: lt_v2iadd, \
		lt_vec3i_t: lt_v3iadd, \
		lt_vec4i_t: lt_v4iadd \
	)((a), (b)))

#define lt_vsub(a, b) (_Generic((a), \
		lt_vec2_t: lt_v2sub, \
		lt_vec3_t: lt_v3sub, \
		lt_vec4_t: lt_v4sub, \
		lt_vec2d_t: lt_v2dsub, \
		lt_vec3d_t: lt_v3dsub, \
		lt_vec4d_t: lt_v4dsub, \
		lt_vec2i_t: lt_v2isub, \
		lt_vec3i_t: lt_v3isub, \
		lt_vec4i_t: lt_v4isub \
	)((a), (b)))

#define lt_vdiv(a, b) (_Generic((a), \
		lt_vec2_t: lt_v2div, \
		lt_vec3_t: lt_v3div, \
		lt_vec4_t: lt_v4div, \
		lt_vec2d_t: lt_v2ddiv, \
		lt_vec3d_t: lt_v3ddiv, \
		lt_vec4d_t: lt_v4ddiv, \
		lt_vec2i_t: lt_v2idiv, \
		lt_vec3i_t: lt_v3idiv, \
		lt_vec4i_t: lt_v4idiv \
	)((a), (b)))

#define lt_vmod(a, b) (_Generic((a), \
		lt_vec2_t: lt_v2mod, \
		lt_vec3_t: lt_v3mod, \
		lt_vec4_t: lt_v4mod, \
		lt_vec2d_t: lt_v2dmod, \
		lt_vec3d_t: lt_v3dmod, \
		lt_vec4d_t: lt_v4dmod, \
		lt_vec2i_t: lt_v2imod, \
		lt_vec3i_t: lt_v3imod, \
		lt_vec4i_t: lt_v4imod \
	)((a), (b)))

#define lt_vmul(a, b) (_Generic((a), \
		lt_vec2_t: lt_v2mul, \
		lt_vec3_t: lt_v3mul, \
		lt_vec4_t: lt_v4mul, \
		lt_vec2d_t: lt_v2dmul, \
		lt_vec3d_t: lt_v3dmul, \
		lt_vec4d_t: lt_v4dmul, \
		lt_vec2i_t: lt_v2imul, \
		lt_vec3i_t: lt_v3imul, \
		lt_vec4i_t: lt_v4imul \
	)((a), (b)))

#define lt_vaddf(a, b) (_Generic((a), \
		lt_vec2_t: lt_v2addf, \
		lt_vec3_t: lt_v3addf, \
		lt_vec4_t: lt_v4addf, \
		lt_vec2d_t: lt_v2daddf, \
		lt_vec3d_t: lt_v3daddf, \
		lt_vec4d_t: lt_v4daddf \
	)((a), (b)))

#define lt_vsubf(a, b) (_Generic((a), \
		lt_vec2_t: lt_v2subf, \
		lt_vec3_t: lt_v3subf, \
		lt_vec4_t: lt_v4subf, \
		lt_vec2d_t: lt_v2dsubf, \
		lt_vec3d_t: lt_v3dsubf, \
		lt_vec4d_t: lt_v4dsubf \
	)((a), (b)))

#define lt_vdivf(a, b) (_Generic((a), \
		lt_vec2_t: lt_v2divf, \
		lt_vec3_t: lt_v3divf, \
		lt_vec4_t: lt_v4divf, \
		lt_vec2d_t: lt_v2ddivf, \
		lt_vec3d_t: lt_v3ddivf, \
		lt_vec4d_t: lt_v4ddivf \
	)((a), (b)))

#define lt_vmodf(a, b) (_Generic((a), \
		lt_vec2_t: lt_v2modf, \
		lt_vec3_t: lt_v3modf, \
		lt_vec4_t: lt_v4modf, \
		lt_vec2d_t: lt_v2dmodf, \
		lt_vec3d_t: lt_v3dmodf, \
		lt_vec4d_t: lt_v4dmodf \
	)((a), (b)))

#define lt_vmulf(a, b) (_Generic((a), \
		lt_vec2_t: lt_v2mulf, \
		lt_vec3_t: lt_v3mulf, \
		lt_vec4_t: lt_v4mulf, \
		lt_vec2d_t: lt_v2dmulf, \
		lt_vec3d_t: lt_v3dmulf, \
		lt_vec4d_t: lt_v4dmulf \
	)((a), (b)))

#define lt_vaddi(a, b) (_Generic((a), \
		lt_vec2i_t: lt_v2iaddi, \
		lt_vec3i_t: lt_v3iaddi, \
		lt_vec4i_t: lt_v4iaddi \
	)((a), (b)))

#define lt_vsubi(a, b) (_Generic((a), \
		lt_vec2i_t: lt_v2isubi, \
		lt_vec3i_t: lt_v3isubi, \
		lt_vec4i_t: lt_v4isubi \
	)((a), (b)))

#define lt_vdivi(a, b) (_Generic((a), \
		lt_vec2i_t: lt_v2idivi, \
		lt_vec3i_t: lt_v3idivi, \
		lt_vec4i_t: lt_v4idivi \
	)((a), (b)))

#define lt_vmodi(a, b) (_Generic((a), \
		lt_vec2i_t: lt_v2imodi, \
		lt_vec3i_t: lt_v3imodi, \
		lt_vec4i_t: lt_v4imodi \
	)((a), (b)))

#define lt_vmuli(a, b) (_Generic((a), \
		lt_vec2i_t: lt_v2imuli, \
		lt_vec3i_t: lt_v3imuli, \
		lt_vec4i_t: lt_v4imuli \
	)((a), (b)))

// !! missing vecd
#define lt_vmulm(a, b) (_Generic((a), \
		lt_vec2_t: lt_v2mulm, \
		lt_vec3_t: lt_v3mulm, \
		lt_vec4_t: lt_v4mulm \
	)((a), (b)))

// !! missing vecd
#define lt_vmulm4(a, w, m) (lt_v3mulm4((a), (w), (m)))

#define lt_vdot(a, b) (_Generic((a), \
		lt_vec2_t: lt_v2dot, \
		lt_vec3_t: lt_v3dot, \
		lt_vec4_t: lt_v4dot, \
		lt_vec2d_t: lt_v2ddot, \
		lt_vec3d_t: lt_v3ddot, \
		lt_vec4d_t: lt_v4ddot \
	)((a), (b)))

#define lt_vneg(a) (_Generic((a), \
		lt_vec2_t: lt_v2neg, \
		lt_vec3_t: lt_v3neg, \
		lt_vec4_t: lt_v4neg, \
		lt_vec2d_t: lt_v2dneg, \
		lt_vec3d_t: lt_v3dneg, \
		lt_vec4d_t: lt_v4dneg, \
		lt_vec2i_t: lt_v2ineg, \
		lt_vec3i_t: lt_v3ineg, \
		lt_vec4i_t: lt_v4ineg \
	)((a)))

#define lt_vmag(a) (_Generic((a), \
		lt_vec2_t: lt_v2mag, \
		lt_vec3_t: lt_v3mag, \
		lt_vec4_t: lt_v4mag, \
		lt_vec2d_t: lt_v2dmag, \
		lt_vec3d_t: lt_v3dmag, \
		lt_vec4d_t: lt_v4dmag \
	)((a)))

#define lt_vdist(a, b) (_Generic((a), \
		lt_vec2_t: lt_v2dist, \
		lt_vec3_t: lt_v3dist, \
		lt_vec4_t: lt_v4dist, \
		lt_vec2d_t: lt_v2ddist, \
		lt_vec3d_t: lt_v3ddist, \
		lt_vec4d_t: lt_v4ddist \
	)((a), (b)))

#define lt_vnorm(a) (_Generic((a), \
		lt_vec2_t: lt_v2norm, \
		lt_vec3_t: lt_v3norm, \
		lt_vec4_t: lt_v4norm, \
		lt_vec2d_t: lt_v2dnorm, \
		lt_vec3d_t: lt_v3dnorm, \
		lt_vec4d_t: lt_v4dnorm \
	)((a)))

// !! missing vec4
#define lt_vcross(a, b) (_Generic((a), \
		lt_vec2_t: lt_v2cross, \
		lt_vec3_t: lt_v3cross, \
		lt_vec2d_t: lt_v2dcross, \
		lt_vec3d_t: lt_v3dcross \
	)((a), (b)))

#define lt_vcrossn(a, b) (_Generic((a), \
		lt_vec3_t: lt_v3crossn, \
		lt_vec3d_t: lt_v3dcrossn \
	)((a), (b)))

#define lt_vproject(a, b) (_Generic((a), \
		lt_vec3_t: lt_v3project, \
		lt_vec3d_t: lt_v3dproject \
	)((a), (b)))

#define lt_vpproject(a, b) (_Generic((a), \
		lt_vec3_t: lt_v3pproject, \
		lt_vec3d_t: lt_v3dpproject \
	)((a), (b)))

#define lt_vmin(a, b) (_Generic((a), \
		lt_vec2_t: lt_v2min, \
		lt_vec3_t: lt_v3min, \
		lt_vec4_t: lt_v4min, \
		lt_vec2d_t: lt_v2dmin, \
		lt_vec3d_t: lt_v3dmin, \
		lt_vec4d_t: lt_v4dmin, \
		lt_vec2i_t: lt_v2imin, \
		lt_vec3i_t: lt_v3imin, \
		lt_vec4i_t: lt_v4imin \
	)((a), (b)))

#define lt_vmax(a, b) (_Generic((a), \
		lt_vec2_t: lt_v2max, \
		lt_vec3_t: lt_v3max, \
		lt_vec4_t: lt_v4max, \
		lt_vec2d_t: lt_v2dmax, \
		lt_vec3d_t: lt_v3dmax, \
		lt_vec4d_t: lt_v4dmax, \
		lt_vec2i_t: lt_v2imax, \
		lt_vec3i_t: lt_v3imax, \
		lt_vec4i_t: lt_v4imax \
	)((a), (b)))

#define lt_vclamp(v, min, max) (_Generic((v), \
		lt_vec2_t: lt_v2clamp, \
		lt_vec3_t: lt_v3clamp, \
		lt_vec4_t: lt_v4clamp, \
		lt_vec2d_t: lt_v2dclamp, \
		lt_vec3d_t: lt_v3dclamp, \
		lt_vec4d_t: lt_v4dclamp, \
		lt_vec2i_t: lt_v2iclamp, \
		lt_vec3i_t: lt_v3iclamp, \
		lt_vec4i_t: lt_v4iclamp \
	)((v), (min), (max)))

#define lt_vclampf(v, min, max) (_Generic((v), \
		lt_vec2_t: lt_v2clampf, \
		lt_vec3_t: lt_v3clampf, \
		lt_vec4_t: lt_v4clampf, \
		lt_vec2d_t: lt_v2dclampf, \
		lt_vec3d_t: lt_v3dclampf, \
		lt_vec4d_t: lt_v4dclampf \
	)((v), (min), (max)))

#define lt_vclampi(v, min, max) (_Generic((v), \
		lt_vec2i_t: lt_v2iclampi, \
		lt_vec3i_t: lt_v3iclampi, \
		lt_vec4i_t: lt_v4iclampi \
	)((v), (min), (max)))

#define lt_vmincomp(a) (_Generic((a), \
		lt_vec2_t: lt_v2mincomp, \
		lt_vec3_t: lt_v3mincomp, \
		lt_vec4_t: lt_v4mincomp, \
		lt_vec2d_t: lt_v2dmincomp, \
		lt_vec3d_t: lt_v3dmincomp, \
		lt_vec4d_t: lt_v4dmincomp, \
		lt_vec2i_t: lt_v2imincomp, \
		lt_vec3i_t: lt_v3imincomp, \
		lt_vec4i_t: lt_v4imincomp \
	)((a)))

#define lt_vmaxcomp(a) (_Generic((a), \
		lt_vec2_t: lt_v2maxcomp, \
		lt_vec3_t: lt_v3maxcomp, \
		lt_vec4_t: lt_v4maxcomp, \
		lt_vec2d_t: lt_v2dmaxcomp, \
		lt_vec3d_t: lt_v3dmaxcomp, \
		lt_vec4d_t: lt_v4dmaxcomp, \
		lt_vec2i_t: lt_v2imaxcomp, \
		lt_vec3i_t: lt_v3imaxcomp, \
		lt_vec4i_t: lt_v4imaxcomp \
	)((a)))

#define lt_vabs(a) (_Generic((a), \
		lt_vec2_t: lt_v2abs, \
		lt_vec3_t: lt_v3abs, \
		lt_vec4_t: lt_v4abs, \
		lt_vec2d_t: lt_v2dabs, \
		lt_vec3d_t: lt_v3dabs, \
		lt_vec4d_t: lt_v4dabs, \
		lt_vec2i_t: lt_v2iabs, \
		lt_vec3i_t: lt_v3iabs, \
		lt_vec4i_t: lt_v4iabs \
	)((a)))

#define lt_vequ(a, b) (_Generic((a), \
		lt_vec2_t: lt_v2equ, \
		lt_vec3_t: lt_v3equ, \
		lt_vec4_t: lt_v4equ, \
		lt_vec2d_t: lt_v2dequ, \
		lt_vec3d_t: lt_v3dequ, \
		lt_vec4d_t: lt_v4dequ, \
		lt_vec2i_t: lt_v2iequ, \
		lt_vec3i_t: lt_v3iequ, \
		lt_vec4i_t: lt_v4iequ \
	)((a), (b)))

#define lt_veque(a, b, c) (_Generic((a), \
		lt_vec2_t: lt_v2eque, \
		lt_vec3_t: lt_v3eque, \
		lt_vec4_t: lt_v4eque, \
		lt_vec2d_t: lt_v2deque, \
		lt_vec3d_t: lt_v3deque, \
		lt_vec4d_t: lt_v4deque \
	)((a), (b), (c)))

#define lt_vlerp(a, b, c) (_Generic((a), \
		lt_vec2_t: lt_v2lerp, \
		lt_vec3_t: lt_v3lerp, \
		lt_vec4_t: lt_v4lerp, \
		lt_vec2d_t: lt_v2dlerp, \
		lt_vec3d_t: lt_v3dlerp, \
		lt_vec4d_t: lt_v4dlerp \
	)((a), (b), (c)))

#define lt_vlerpc(a, b, c) (_Generic((a), \
		lt_vec2_t: lt_v2lerpc, \
		lt_vec3_t: lt_v3lerpc, \
		lt_vec4_t: lt_v4lerpc, \
		lt_vec2d_t: lt_v2dlerpc, \
		lt_vec3d_t: lt_v3dlerpc, \
		lt_vec4d_t: lt_v4dlerpc \
	)((a), (b), (c)))

#define lt_vround(a) (_Generic((a), \
		lt_vec2_t: lt_v2round, \
		lt_vec3_t: lt_v3round, \
		lt_vec4_t: lt_v4round, \
		lt_vec2d_t: lt_v2dround, \
		lt_vec3d_t: lt_v3dround, \
		lt_vec4d_t: lt_v4dround \
	)((a)))

#define lt_vfloor(a) (_Generic((a), \
		lt_vec2_t: lt_v2floor, \
		lt_vec3_t: lt_v3floor, \
		lt_vec4_t: lt_v4floor, \
		lt_vec2d_t: lt_v2dfloor, \
		lt_vec3d_t: lt_v3dfloor, \
		lt_vec4d_t: lt_v4dfloor \
	)((a)))

#define lt_vceil(a) (_Generic((a), \
		lt_vec2_t: lt_v2ceil, \
		lt_vec3_t: lt_v3ceil, \
		lt_vec4_t: lt_v4ceil, \
		lt_vec2d_t: lt_v2dceil, \
		lt_vec3d_t: lt_v3dceil, \
		lt_vec4d_t: lt_v4dceil \
	)((a)))

#define lt_vitof(a) (_Generic((a), \
		lt_vec2i_t: lt_v2itof, \
		lt_vec3i_t: lt_v3itof, \
		lt_vec4i_t: lt_v4itof \
	)((a)))

// ----- Shortened names

#ifdef LT_LINALG_SHORTEN_NAMES
#	define vadd lt_vadd
#	define vsub lt_vsub
#	define vdiv lt_vdiv
#	define vmod lt_vmod
#	define vmul lt_vmul
#	define vaddf lt_vaddf
#	define vsubf lt_vsubf
#	define vdivf lt_vdivf
#	define vmodf lt_vmodf
#	define vmulf lt_vmulf
#	define vaddi lt_vaddi
#	define vsubi lt_vsubi
#	define vdivi lt_vdivi
#	define vmodi lt_vmodi
#	define vmuli lt_vmuli

#	define vdot lt_vdot
#	define vneg lt_vneg
#	define vcross lt_vcross
#	define vcrossn lt_vcrossn
#	define vmag lt_vmag
#	define vdist lt_vdist
#	define vnorm lt_vnorm
#	define vabs lt_vabs
#	define vequ lt_vequ
#	define veque lt_veque
#	define vlerp lt_vlerp
#	define vlerpc lt_vlerpc
#	define vproject lt_vproject
#	define vpproject lt_vpproject

#	define vmax lt_vmax
#	define vmin lt_vmin
#	define vclamp lt_vclamp
#	define vclampf lt_vclampf
#	define vclampi lt_vclampi
#	define vmaxcomp lt_vmaxcomp
#	define vmincomp lt_vmincomp

#	define vround lt_vround
#	define vfloor lt_vfloor
#	define vceil lt_vceil
#	define vitof lt_vitof

#	define vmulm lt_vmulm
#	define vmulm4 lt_vmulm4
#endif

#endif
