#ifndef LT_LINALG_VEC3_H
#define LT_LINALG_VEC3_H 1

#ifdef LT_LINALG_SHORTEN_NAMES
#	define vec3 lt_vec3_t

#	define VEC3_INIT(x, y, z) { .data = { (x), (y), (z) } }
#	define VEC3(x, y, z) ((lt_vec3_t){ .data = { (x), (y), (z) } })

#	define v3fill lt_v3fill

#	define v3add lt_v3add
#	define v3sub lt_v3sub
#	define v3div lt_v3div
#	define v3mod lt_v3mod
#	define v3mul lt_v3mul

#	define v3addf lt_v3addf
#	define v3subf lt_v3subf
#	define v3divf lt_v3divf
#	define v3modf lt_v3modf
#	define v3mulf lt_v3mulf

#	define v3dot lt_v3dot
#	define v3neg lt_v3neg
#	define v3mag lt_v3mag
#	define v3dist lt_v3dist
#	define v3norm lt_v3norm
#	define v3cross lt_v3cross
#	define v3crossn lt_v3crossn
#	define v3project lt_v3project
#	define v3pproject lt_v3pproject
#	define v3abs lt_v3abs
#	define v3equ lt_v3equ
#	define v3eque lt_v3eque
#	define v3lerp lt_v3lerp
#	define v3lerpc lt_v3lerpc
#	define v3rotate lt_v3rotate

#	define v3min lt_v3min
#	define v3max lt_v3max
#	define v3clamp lt_v3clamp
#	define v3clampf lt_v3clampf
#	define v3mincomp lt_v3mincomp
#	define v3maxcomp lt_v3maxcomp

#	define v3round lt_v3round
#	define v3floor lt_v3floor
#	define v3ceil lt_v3ceil
#endif

static LT_INLINE
lt_vec3_t lt_v3fill(f32 v)  {
	return LT_VEC3(v, v, v);
}


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
lt_vec3_t lt_v3mod(lt_vec3_t a, lt_vec3_t b) {
	return LT_VEC3(fmod(a.x, b.x), fmod(a.y, b.y), fmod(a.z, b.z));
}

static LT_INLINE
lt_vec3_t lt_v3mul(lt_vec3_t a, lt_vec3_t b) {
	return LT_VEC3(a.x * b.x, a.y * b.y, a.z * b.z);
}


static LT_INLINE
lt_vec3_t lt_v3addf(lt_vec3_t a, f32 f) {
	return LT_VEC3(a.x + f, a.y + f, a.z + f);
}

static LT_INLINE
lt_vec3_t lt_v3subf(lt_vec3_t a, f32 f) {
	return LT_VEC3(a.x - f, a.y - f, a.z - f);
}

static LT_INLINE
lt_vec3_t lt_v3divf(lt_vec3_t a, f32 f) {
	return LT_VEC3(a.x / f, a.y / f, a.z / f);
}

static LT_INLINE
lt_vec3_t lt_v3modf(lt_vec3_t a, f32 f) {
	return LT_VEC3(fmod(a.x, f), fmod(a.y, f), fmod(a.z, f));
}

static LT_INLINE
lt_vec3_t lt_v3mulf(lt_vec3_t a, f32 f) {
	return LT_VEC3(a.x * f, a.y * f, a.z * f);
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
	f32 nmag2 = lt_v3dot(normal, normal);
	if (nmag2 <= 0.0f)
		return point;

	f32 factor = lt_v3dot(point, normal) / nmag2;
	return lt_v3mulf(normal, factor);
}

static LT_INLINE
lt_vec3_t lt_v3pproject(lt_vec3_t normal, lt_vec3_t point) {
	f32 nmag2 = lt_v3dot(normal, normal);
	if (nmag2 <= 0.0f)
		return LT_VEC3(0.0f, 0.0f, 0.0f);

	f32 factor = lt_v3dot(point, normal) / nmag2;
	return LT_VEC3(
		point.x - normal.x * factor,
		point.y - normal.y * factor,
		point.z - normal.z * factor
	);
}

static LT_INLINE
lt_vec3_t lt_v3abs(lt_vec3_t v) {
	return LT_VEC3(fabs(v.x), fabs(v.y), fabs(v.z));
}

static LT_INLINE
int lt_v3equ(lt_vec3_t a, lt_vec3_t b) {
	return a.x == b.x && a.y == b.y && a.z == b.z;
}

static LT_INLINE
int lt_v3eque(lt_vec3_t a, lt_vec3_t b, f32 epsilon) {
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

static LT_INLINE
lt_vec3_t lt_v3rotate(lt_vec3_t v, f32 angle, lt_vec3_t axis) {
	lt_vec3_t ax, v1, v2;
	f32 x = cos(angle), y = sin(angle);

	ax = lt_v3norm(axis);
	v1 = lt_v3mulf(v, x);
	v2 = lt_v3mulf(lt_v3cross(ax, v), y);

	v1 = lt_v3add(v1, v2);
	v2 = lt_v3mulf(ax, lt_v3dot(ax, v) * (1.0f - x));

	return lt_v3add(v1, v2);
}


static LT_INLINE
lt_vec3_t lt_v3min(lt_vec3_t a, lt_vec3_t b) {
	return LT_VEC3(lt_min_f32(a.x, b.x), lt_min_f32(a.y, b.y), lt_min_f32(a.z, b.z));
}

static LT_INLINE
lt_vec3_t lt_v3max(lt_vec3_t a, lt_vec3_t b) {
	return LT_VEC3(lt_max_f32(a.x, b.x), lt_max_f32(a.y, b.y), lt_max_f32(a.z, b.z));
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
f32 lt_v3mincomp(lt_vec3_t v) {
	return lt_min_f32(v.z, lt_min_f32(v.x, v.y));
}

static LT_INLINE
f32 lt_v3maxcomp(lt_vec3_t v) {
	return lt_max_f32(v.z, lt_max_f32(v.x, v.y));
}


static LT_INLINE
lt_vec3i_t lt_v3round(lt_vec3_t v) {
	return LT_VEC3I(round(v.x), round(v.y), round(v.z));
}

static LT_INLINE
lt_vec3i_t lt_v3floor(lt_vec3_t v) {
	return LT_VEC3I(floor(v.x), floor(v.y), floor(v.z));
}

static LT_INLINE
lt_vec3i_t lt_v3ceil(lt_vec3_t v) {
	return LT_VEC3I(ceil(v.x), ceil(v.y), ceil(v.z));
}

#endif