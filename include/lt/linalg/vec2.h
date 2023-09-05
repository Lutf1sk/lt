#ifndef LT_LINALG_VEC2_H
#define LT_LINALG_VEC2_H 1

#ifdef LT_LINALG_SHORTEN_NAMES
#	define vec2 lt_vec2_t

#	define VEC2_INIT(x, y) { .data = { (x), (y) } }
#	define VEC2(x, y) ((lt_vec2_t){ .data = { (x), (y) } })

#	define v2fill lt_v2fill

#	define v2add lt_v2add
#	define v2sub lt_v2sub
#	define v2div lt_v2div
#	define v2mod lt_v2mod
#	define v2mul lt_v2mul

#	define v2addf lt_v2addf
#	define v2subf lt_v2subf
#	define v2divf lt_v2divf
#	define v2modf lt_v2modf
#	define v2mulf lt_v2mulf

#	define v2dot lt_v2dot
#	define v2neg lt_v2neg
#	define v2mag lt_v2mag
#	define v2dist lt_v2dist
#	define v2norm lt_v2norm
#	define v2cross lt_v2cross
#	define v2abs lt_v2abs
#	define v2equ lt_v2equ
#	define v2eque lt_v2eque
#	define v2lerp lt_v2lerp
#	define v2lerpc lt_v2lerpc

#	define v2min lt_v2min
#	define v2max lt_v2max
#	define v2clamp lt_v2clamp
#	define v2clampf lt_v2clampf
#	define v2mincomp lt_v2mincomp
#	define v2maxcomp lt_v2maxcomp

#	define v2round lt_v2round
#	define v2floor lt_v2floor
#	define v2ceil lt_v2ceil
#endif

static LT_INLINE
lt_vec2_t lt_v2fill(f32 v)  {
	return LT_VEC2(v, v);
}


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
lt_vec2_t lt_v2mod(lt_vec2_t a, lt_vec2_t b) {
	return LT_VEC2(fmod(a.x, b.x), fmod(a.y, b.y));
}

static LT_INLINE
lt_vec2_t lt_v2mul(lt_vec2_t a, lt_vec2_t b) {
	return LT_VEC2(a.x * b.x, a.y * b.y);
}


static LT_INLINE
lt_vec2_t lt_v2addf(lt_vec2_t a, f32 f) {
	return LT_VEC2(a.x + f, a.y + f);
}

static LT_INLINE
lt_vec2_t lt_v2subf(lt_vec2_t a, f32 f) {
	return LT_VEC2(a.x - f, a.y - f);
}

static LT_INLINE
lt_vec2_t lt_v2divf(lt_vec2_t a, f32 f) {
	return LT_VEC2(a.x / f, a.y / f);
}

static LT_INLINE
lt_vec2_t lt_v2modf(lt_vec2_t a, f32 f) {
	return LT_VEC2(fmod(a.x, f), fmod(a.y, f));
}

static LT_INLINE
lt_vec2_t lt_v2mulf(lt_vec2_t a, f32 f) {
	return LT_VEC2(a.x * f, a.y * f);
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
lt_vec2_t lt_v2abs(lt_vec2_t v) {
	return LT_VEC2(fabs(v.x), fabs(v.y));
}

static LT_INLINE
int lt_v2equ(lt_vec2_t a, lt_vec2_t b) {
	return a.x == b.x && a.y == b.y;
}

static LT_INLINE
int lt_v2eque(lt_vec2_t a, lt_vec2_t b, f32 epsilon) {
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


static LT_INLINE
lt_vec2_t lt_v2min(lt_vec2_t a, lt_vec2_t b) {
	return LT_VEC2(lt_min_f32(a.x, b.x), lt_min_f32(a.y, b.y));
}

static LT_INLINE
lt_vec2_t lt_v2max(lt_vec2_t a, lt_vec2_t b) {
	return LT_VEC2(lt_max_f32(a.x, b.x), lt_max_f32(a.y, b.y));
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
f32 lt_v2mincomp(lt_vec2_t v) {
	return lt_min_f32(v.x, v.y);
}

static LT_INLINE
f32 lt_v2maxcomp(lt_vec2_t v) {
	return lt_max_f32(v.x, v.y);
}


static LT_INLINE
lt_vec2i_t lt_v2round(lt_vec2_t v) {
	return LT_VEC2I(round(v.x), round(v.y));
}

static LT_INLINE
lt_vec2i_t lt_v2floor(lt_vec2_t v) {
	return LT_VEC2I(floor(v.x), floor(v.y));
}

static LT_INLINE
lt_vec2i_t lt_v2ceil(lt_vec2_t v) {
	return LT_VEC2I(ceil(v.x), ceil(v.y));
}

#endif