#ifndef LT_LINALG_VEC4I_H
#define LT_LINALG_VEC4I_H 1

#ifdef LT_LINALG_SHORTEN_NAMES
#	define vec4i lt_vec4i_t

#	define VEC4I_INIT(x, y, z, w) { .data = { (x), (y), (z), (w) } }
#	define VEC4I(x, y, z, w) ((lt_vec4i_t){ .data = { (x), (y), (z), (w) } })

#	define v4ifill lt_v4ifill

#	define v4iadd lt_v4iadd
#	define v4isub lt_v4isub
#	define v4idiv lt_v4idiv
#	define v4imod lt_v4imod
#	define v4imul lt_v4imul

#	define v4iaddi lt_v4iaddi
#	define v4isubi lt_v4isubi
#	define v4idivi lt_v4idivi
#	define v4imodi lt_v4imodi
#	define v4imuli lt_v4imuli

#	define v4ineg lt_v4ineg
#	define v4iabs lt_v4iabs
#	define v4iequ lt_v4iequ

#	define v4imin lt_v4imin
#	define v4imax lt_v4imax
#	define v4iclamp lt_v4iclamp
#	define v4iclampi lt_v4iclampi
#	define v4imincomp lt_v4imincomp
#	define v4imaxcomp lt_v4imaxcomp

#	define v4itof lt_v4itof
#endif


static LT_INLINE
lt_vec4i_t lt_v4ifill(i32 v)  {
	return LT_VEC4I(v, v, v, v);
}


static LT_INLINE
lt_vec4i_t lt_v4iadd(lt_vec4i_t a, lt_vec4i_t b) {
	return LT_VEC4I(a.x + b.x, a.y + b.y, a.z + b.z, a.w + b.w);
}

static LT_INLINE
lt_vec4i_t lt_v4isub(lt_vec4i_t a, lt_vec4i_t b) {
	return LT_VEC4I(a.x - b.x, a.y - b.y, a.z - b.z, a.w - b.w);
}

static LT_INLINE
lt_vec4i_t lt_v4idiv(lt_vec4i_t a, lt_vec4i_t b) {
	return LT_VEC4I(a.x / b.x, a.y / b.y, a.z / b.z, a.w / b.w);
}

static LT_INLINE
lt_vec4i_t lt_v4imul(lt_vec4i_t a, lt_vec4i_t b) {
	return LT_VEC4I(a.x * b.x, a.y * b.y, a.z * b.z, a.w * b.w);
}


static LT_INLINE
lt_vec4i_t lt_v4iaddi(lt_vec4i_t a, i32 f) {
	return LT_VEC4I(a.x + f, a.y + f, a.z + f, a.w + f);
}

static LT_INLINE
lt_vec4i_t lt_v4isubi(lt_vec4i_t a, i32 f) {
	return LT_VEC4I(a.x - f, a.y - f, a.z - f, a.w - f);
}

static LT_INLINE
lt_vec4i_t lt_v4idivi(lt_vec4i_t a, i32 f) {
	return LT_VEC4I(a.x / f, a.y / f, a.z / f, a.w / f);
}

static LT_INLINE
lt_vec4i_t lt_v4imodi(lt_vec4i_t a, i32 f) {
	return LT_VEC4I(a.x % f, a.y % f, a.z % f, a.w % f);
}

static LT_INLINE
lt_vec4i_t lt_v4imuli(lt_vec4i_t a, i32 f) {
	return LT_VEC4I(a.x * f, a.y * f, a.z * f, a.w * f);
}


static LT_INLINE
lt_vec4i_t lt_v4ineg(lt_vec4i_t v) {
	return LT_VEC4I(-v.x, -v.y, -v.z, -v.w);
}

static LT_INLINE
lt_vec4i_t lt_v4iabs(lt_vec4i_t v) {
	return LT_VEC4I(lt_abs_i32(v.x), lt_abs_i32(v.y), lt_abs_i32(v.z), lt_abs_i32(v.w));
}

static LT_INLINE
int lt_v4iequ(lt_vec4i_t a, lt_vec4i_t b) {
	return a.x == b.x && a.y == b.y && a.z == b.z && a.w == b.w;
}


static LT_INLINE
lt_vec4i_t lt_v4imax(lt_vec4i_t a, lt_vec4i_t b) {
	return LT_VEC4I(lt_max_i32(a.x, b.x), lt_max_i32(a.y, b.y), lt_max_i32(a.z, b.z), lt_max_i32(a.w, b.w));
}

static LT_INLINE
lt_vec4i_t lt_v4imin(lt_vec4_t a, lt_vec4i_t b) {
	return LT_VEC4I(lt_min_i32(a.x, b.x), lt_min_i32(a.y, b.y), lt_min_i32(a.z, b.z), lt_min_i32(a.w, b.w));
}

static LT_INLINE
lt_vec4i_t lt_v4iclamp(lt_vec4i_t v, lt_vec4i_t min, lt_vec4i_t max) {
	return LT_VEC4I(lt_clamp_i32(v.x, min.x, max.x), lt_clamp_i32(v.y, min.y, max.y), lt_clamp_i32(v.z, min.z, max.z), lt_clamp_i32(v.w, min.w, max.w));
}

static LT_INLINE
lt_vec4i_t lt_v4iclampi(lt_vec4i_t v, i32 min, i32 max) {
	return LT_VEC4I(lt_clamp_i32(v.x, min, max), lt_clamp_i32(v.y, min, max), lt_clamp_i32(v.z, min, max), lt_clamp_i32(v.w, min, max));
}

static LT_INLINE
f32 lt_v4imincomp(lt_vec4i_t v) {
	return lt_min_i32(v.w, lt_min_i32(v.z, lt_min_i32(v.x, v.y)));
}

static LT_INLINE
f32 lt_v4imaxcomp(lt_vec4i_t v) {
	return lt_max_i32(v.w, lt_max_i32(v.z, lt_max_i32(v.x, v.y)));
}


static LT_INLINE
lt_vec4_t lt_v4itof(lt_vec4i_t v) {
	return LT_VEC4(v.x, v.y, v.z, v.w);
}

#endif