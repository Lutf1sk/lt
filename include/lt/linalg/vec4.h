#ifndef LT_LINALG_VEC4_H
#define LT_LINALG_VEC4_H 1

#ifdef LT_LINALG_SHORTEN_NAMES
#	define vec4 lt_vec4_t

#	define VEC4_INIT(x, y, z, w) { .data = { (x), (y), (z), (w) } }
#	define VEC4(x, y, z, w) ((lt_vec4_t){ .data = { (x), (y), (z), (w) } })

#	define v4fill lt_v4fill

#	define v4add lt_v4add
#	define v4sub lt_v4sub
#	define v4div lt_v4div
#	define v4mod lt_v4mod
#	define v4mul lt_v4mul

#	define v4addf lt_v4addf
#	define v4subf lt_v4subf
#	define v4divf lt_v4divf
#	define v4modf lt_v4modf
#	define v4mulf lt_v4mulf

#	define v4dot lt_v4dot
#	define v4neg lt_v4neg
#	define v4mag lt_v4mag
#	define v4dist lt_v4dist
#	define v4norm lt_v4norm
#	define v4abs lt_v4abs
#	define v4equ lt_v4equ
#	define v4eque lt_v4eque
#	define v4lerp lt_v4lerp
#	define v4lerpc lt_v4lerpc

#	define v4min lt_v4min
#	define v4max lt_v4max
#	define v4clamp lt_v4clamp
#	define v4clampf lt_v4clampf
#	define v4mincomp lt_v4mincomp
#	define v4maxcomp lt_v4maxcomp

#	define v4round lt_v4round
#	define v4floor lt_v4floor
#	define v4ceil lt_v4ceil
#endif


static LT_INLINE
lt_vec4_t lt_v4fill(f32 v)  {
	return LT_VEC4(v, v, v, v);
}


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
lt_vec4_t lt_v4mod(lt_vec4_t a, lt_vec4_t b) {
	return LT_VEC4(fmod(a.x, b.x), fmod(a.y, b.y), fmod(a.z, b.z), fmod(a.w, b.w));
}

static LT_INLINE
lt_vec4_t lt_v4mul(lt_vec4_t a, lt_vec4_t b) {
	return LT_VEC4(a.x * b.x, a.y * b.y, a.z * b.z, a.w * b.w);
}


static LT_INLINE
lt_vec4_t lt_v4addf(lt_vec4_t a, f32 f) {
	return LT_VEC4(a.x + f, a.y + f, a.z + f, a.w + f);
}

static LT_INLINE
lt_vec4_t lt_v4subf(lt_vec4_t a, f32 f) {
	return LT_VEC4(a.x - f, a.y - f, a.z - f, a.w - f);
}

static LT_INLINE
lt_vec4_t lt_v4divf(lt_vec4_t a, f32 f) {
	return LT_VEC4(a.x / f, a.y / f, a.z / f, a.w / f);
}

static LT_INLINE
lt_vec4_t lt_v4modf(lt_vec4_t a, f32 f) {
	return LT_VEC4(fmod(a.x, f), fmod(a.y, f), fmod(a.z, f), fmod(a.w, f));
}

static LT_INLINE
lt_vec4_t lt_v4mulf(lt_vec4_t a, f32 f) {
	return LT_VEC4(a.x * f, a.y * f, a.z * f, a.w * f);
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
	f32 h = sqrt(v.x * v.x + v.y * v.y);
	f32 h2 = sqrt(v.z * v.z + h * h);
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
lt_vec4_t lt_v4abs(lt_vec4_t v) {
	return LT_VEC4(fabs(v.x), fabs(v.y), fabs(v.z), fabs(v.w));
}

static LT_INLINE
int lt_v4equ(lt_vec4_t a, lt_vec4_t b) {
	return a.x == b.x && a.y == b.y && a.z == b.z && a.w == b.w;
}

static LT_INLINE
int lt_v4eque(lt_vec4_t a, lt_vec4_t b, f32 epsilon) {
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


static LT_INLINE
lt_vec4_t lt_v4min(lt_vec4_t a, lt_vec4_t b) {
	return LT_VEC4(lt_min_f32(a.x, b.x), lt_min_f32(a.y, b.y), lt_min_f32(a.z, b.z), lt_min_f32(a.w, b.w));
}

static LT_INLINE
lt_vec4_t lt_v4max(lt_vec4_t a, lt_vec4_t b) {
	return LT_VEC4(lt_max_f32(a.x, b.x), lt_max_f32(a.y, b.y), lt_max_f32(a.z, b.z), lt_max_f32(a.w, b.w));
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
f32 lt_v4mincomp(lt_vec4_t v) {
	return lt_min_f32(v.w, lt_min_f32(v.z, lt_min_f32(v.x, v.y)));
}

static LT_INLINE
f32 lt_v4maxcomp(lt_vec4_t v) {
	return lt_max_f32(v.w, lt_max_f32(v.z, lt_max_f32(v.x, v.y)));
}


static LT_INLINE
lt_vec4i_t lt_v4round(lt_vec4_t v) {
	return LT_VEC4I(round(v.x), round(v.y), round(v.z), round(v.w));
}

static LT_INLINE
lt_vec4i_t lt_v4floor(lt_vec4_t v) {
	return LT_VEC4I(floor(v.x), floor(v.y), floor(v.z), floor(v.w));
}

static LT_INLINE
lt_vec4i_t lt_v4ceil(lt_vec4_t v) {
	return LT_VEC4I(ceil(v.x), ceil(v.y), ceil(v.z), ceil(v.w));
}

#endif