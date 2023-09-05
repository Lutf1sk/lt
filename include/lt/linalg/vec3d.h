#ifndef LT_LINALG_VEC3D_H
#define LT_LINALG_VEC3D_H 1

#ifdef LT_LINALG_SHORTEN_NAMES
#	define vec3d lt_vec3d_t

#	define VEC3D_INIT(x, y, z) { .data = { (x), (y), (z) } }
#	define VEC3D(x, y, z) ((lt_vec3d_t){ .data = { (x), (y), (z) } })

#	define v3dfill lt_v3dfill

#	define v3dadd lt_v3dadd
#	define v3dsub lt_v3dsub
#	define v3ddiv lt_v3ddiv
#	define v3dmod lt_v3dmod
#	define v3dmul lt_v3dmul

#	define v3daddf lt_v3daddf
#	define v3dsubf lt_v3dsubf
#	define v3ddivf lt_v3ddivf
#	define v3dmodf lt_v3dmodf
#	define v3dmulf lt_v3dmulf

#	define v3ddot lt_v3ddot
#	define v3dneg lt_v3dneg
#	define v3dmag lt_v3dmag
#	define v3ddist lt_v3ddist
#	define v3dnorm lt_v3dnorm
#	define v3dcross lt_v3dcross
#	define v3dcrossn lt_v3dcrossn
#	define v3dproject lt_v3dproject
#	define v3dpproject lt_v3dpproject
#	define v3dabs lt_v3dabs
#	define v3dequ lt_v3dequ
#	define v3deque lt_v3deque
#	define v3dlerp lt_v3dlerp
#	define v3dlerpc lt_v3dlerpc
#	define v3drotate lt_v3drotate

#	define v3dmin lt_v3dmin
#	define v3dmax lt_v3dmax
#	define v3dclamp lt_v3dclamp
#	define v3dclampf lt_v3dclampf
#	define v3dmincomp lt_v3dmincomp
#	define v3dmaxcomp lt_v3dmaxcomp

#	define v3dround lt_v3dround
#	define v3dfloor lt_v3dfloor
#	define v3dceil lt_v3dceil
#endif

static LT_INLINE
lt_vec3d_t lt_v3dfill(f64 v)  {
	return LT_VEC3D(v, v, v);
}


static LT_INLINE
lt_vec3d_t lt_v3dadd(lt_vec3d_t a, lt_vec3d_t b) {
	return LT_VEC3D(a.x + b.x, a.y + b.y, a.z + b.z);
}

static LT_INLINE
lt_vec3d_t lt_v3dsub(lt_vec3d_t a, lt_vec3d_t b) {
	return LT_VEC3D(a.x - b.x, a.y - b.y, a.z - b.z);
}

static LT_INLINE
lt_vec3d_t lt_v3ddiv(lt_vec3d_t a, lt_vec3d_t b) {
	return LT_VEC3D(a.x / b.x, a.y / b.y, a.z / b.z);
}

static LT_INLINE
lt_vec3d_t lt_v3dmod(lt_vec3d_t a, lt_vec3d_t b) {
	return LT_VEC3D(fmod(a.x, b.x), fmod(a.y, b.y), fmod(a.z, b.z));
}

static LT_INLINE
lt_vec3d_t lt_v3dmul(lt_vec3d_t a, lt_vec3d_t b) {
	return LT_VEC3D(a.x * b.x, a.y * b.y, a.z * b.z);
}


static LT_INLINE
lt_vec3d_t lt_v3daddf(lt_vec3d_t a, f64 f) {
	return LT_VEC3D(a.x + f, a.y + f, a.z + f);
}

static LT_INLINE
lt_vec3d_t lt_v3dsubf(lt_vec3d_t a, f64 f) {
	return LT_VEC3D(a.x - f, a.y - f, a.z - f);
}

static LT_INLINE
lt_vec3d_t lt_v3ddivf(lt_vec3d_t a, f64 f) {
	return LT_VEC3D(a.x / f, a.y / f, a.z / f);
}

static LT_INLINE
lt_vec3d_t lt_v3dmodf(lt_vec3d_t a, f64 f) {
	return LT_VEC3D(fmod(a.x, f), fmod(a.y, f), fmod(a.z, f));
}

static LT_INLINE
lt_vec3d_t lt_v3dmulf(lt_vec3d_t a, f64 f) {
	return LT_VEC3D(a.x * f, a.y * f, a.z * f);
}


static LT_INLINE
f64 lt_v3ddot(lt_vec3d_t a, lt_vec3d_t b) {
	return a.x * b.x + a.y * b.y + a.z * b.z;
}

static LT_INLINE
lt_vec3d_t lt_v3dneg(lt_vec3d_t v) {
	return LT_VEC3D(-v.x, -v.y, -v.z);
}

static LT_INLINE
f64 lt_v3dmag(lt_vec3d_t v) {
	return sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
}

static LT_INLINE
f64 lt_v3ddist(lt_vec3d_t a, lt_vec3d_t b) {
	return lt_v3dmag(lt_v3dsub(a, b));
}

static LT_INLINE
lt_vec3d_t lt_v3dnorm(lt_vec3d_t v) {
	f64 m = lt_v3dmag(v);
	if (m == 0.0f)
		return LT_VEC3D(0.0f, 0.0f, 0.0f);
	return lt_v3dmulf(v, 1.0f / m);
}

static LT_INLINE
lt_vec3d_t lt_v3dcross(lt_vec3d_t a, lt_vec3d_t b) {
	return LT_VEC3D(
		a.y * b.z - a.z * b.y,
		a.z * b.x - a.x * b.z,
		a.x * b.y - a.y * b.x
	);
}

static LT_INLINE
lt_vec3d_t lt_v3dcrossn(lt_vec3d_t a, lt_vec3d_t b) {
	return lt_v3dnorm(lt_v3dcross(a, b));
}

static LT_INLINE
lt_vec3d_t lt_v3dproject(lt_vec3d_t normal, lt_vec3d_t point) {
	f64 nmag2 = lt_v3ddot(normal, normal);
	if (nmag2 <= 0.0f)
		return point;

	f64 factor = lt_v3ddot(point, normal) / nmag2;
	return lt_v3dmulf(normal, factor);
}

static LT_INLINE
lt_vec3d_t lt_v3dpproject(lt_vec3d_t normal, lt_vec3d_t point) {
	f64 nmag2 = lt_v3ddot(normal, normal);
	if (nmag2 <= 0.0f)
		return LT_VEC3D(0.0f, 0.0f, 0.0f);

	f64 factor = lt_v3ddot(point, normal) / nmag2;
	return LT_VEC3D(
		point.x - normal.x * factor,
		point.y - normal.y * factor,
		point.z - normal.z * factor
	);
}

static LT_INLINE
lt_vec3d_t lt_v3dabs(lt_vec3d_t v) {
	return LT_VEC3D(fabs(v.x), fabs(v.y), fabs(v.z));
}

static LT_INLINE
int lt_v3dequ(lt_vec3d_t a, lt_vec3d_t b) {
	return a.x == b.x && a.y == b.y && a.z == b.z;
}

static LT_INLINE
int lt_v3deque(lt_vec3d_t a, lt_vec3d_t b, f64 epsilon) {
	return fabs(a.x - b.x) < epsilon && fabs(a.y - b.y) < epsilon && fabs(a.z - b.z) < epsilon;
}

static LT_INLINE
lt_vec3d_t lt_v3dlerp(lt_vec3d_t a, lt_vec3d_t b, f64 t) {
	return lt_v3dadd(lt_v3dmulf(a, t), lt_v3dmulf(b, 1.0f - t));
}

static LT_INLINE
lt_vec3d_t lt_v3dlerpc(lt_vec3d_t a, lt_vec3d_t b, f64 t) {
	return lt_v3dlerp(a, b, lt_clamp_f64(t, 0.0f, 1.0f));
}

static LT_INLINE
lt_vec3d_t lt_v3drotate(lt_vec3d_t v, f64 angle, lt_vec3d_t axis) {
	lt_vec3d_t ax, v1, v2;
	f64 x = cos(angle), y = sin(angle);

	ax = lt_v3dnorm(axis);
	v1 = lt_v3dmulf(v, x);
	v2 = lt_v3dmulf(lt_v3dcross(ax, v), y);

	v1 = lt_v3dadd(v1, v2);
	v2 = lt_v3dmulf(ax, lt_v3ddot(ax, v) * (1.0f - x));

	return lt_v3dadd(v1, v2);
}


static LT_INLINE
lt_vec3d_t lt_v3dmin(lt_vec3d_t a, lt_vec3d_t b) {
	return LT_VEC3D(lt_min_f64(a.x, b.x), lt_min_f64(a.y, b.y), lt_min_f64(a.z, b.z));
}

static LT_INLINE
lt_vec3d_t lt_v3dmax(lt_vec3d_t a, lt_vec3d_t b) {
	return LT_VEC3D(lt_max_f64(a.x, b.x), lt_max_f64(a.y, b.y), lt_max_f64(a.z, b.z));
}

static LT_INLINE
lt_vec3d_t lt_v3dclamp(lt_vec3d_t v, lt_vec3d_t min, lt_vec3d_t max) {
	return LT_VEC3D(lt_clamp_f64(v.x, min.x, max.x), lt_clamp_f64(v.y, min.y, max.y), lt_clamp_f64(v.z, min.z, max.z));
}

static LT_INLINE
lt_vec3d_t lt_v3dclampf(lt_vec3d_t v, f64 min, f64 max) {
	return LT_VEC3D(lt_clamp_f64(v.x, min, max), lt_clamp_f64(v.y, min, max), lt_clamp_f64(v.z, min, max));
}

static LT_INLINE
f64 lt_v3dmincomp(lt_vec3d_t v) {
	return lt_min_f64(v.z, lt_min_f64(v.x, v.y));
}

static LT_INLINE
f64 lt_v3dmaxcomp(lt_vec3d_t v) {
	return lt_max_f64(v.z, lt_max_f64(v.x, v.y));
}


static LT_INLINE
lt_vec3i_t lt_v3dround(lt_vec3d_t v) {
	return LT_VEC3I(round(v.x), round(v.y), round(v.z));
}

static LT_INLINE
lt_vec3i_t lt_v3dfloor(lt_vec3d_t v) {
	return LT_VEC3I(floor(v.x), floor(v.y), floor(v.z));
}

static LT_INLINE
lt_vec3i_t lt_v3dceil(lt_vec3d_t v) {
	return LT_VEC3I(ceil(v.x), ceil(v.y), ceil(v.z));
}

#endif