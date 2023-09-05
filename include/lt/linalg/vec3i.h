#ifndef LT_LINALG_VEC3I_H
#define LT_LINALG_VEC3I_H 1

#ifdef LT_LINALG_SHORTEN_NAMES
#	define vec3i lt_vec3i_t

#	define VEC3I_INIT(x, y, z) { .data = { (x), (y), (z) } }
#	define VEC3I(x, y, z) ((lt_vec3i_t){ .data = { (x), (y), (z) } })

#	define v3ifill lt_v3ifill

#	define v3iadd lt_v3iadd
#	define v3isub lt_v3isub
#	define v3idiv lt_v3idiv
#	define v3imod lt_v3imod
#	define v3imul lt_v3imul

#	define v3iaddi lt_v3iaddi
#	define v3isubi lt_v3isubi
#	define v3idivi lt_v3idivi
#	define v3imodi lt_v3imodi
#	define v3imuli lt_v3imuli

#	define v3ineg lt_v3ineg
#	define v3iabs lt_v3iabs
#	define v3iequ lt_v3iequ

#	define v3imin lt_v3imin
#	define v3imax lt_v3imax
#	define v3iclamp lt_v3iclamp
#	define v3iclampi lt_v3iclampi
#	define v3imincomp lt_v3imincomp
#	define v3imaxcomp lt_v3imaxcomp

#	define v3itof lt_v3itof
#endif


static LT_INLINE
lt_vec3i_t lt_v3ifill(i32 v)  {
	return LT_VEC3I(v, v, v);
}


static LT_INLINE
lt_vec3i_t lt_v3iadd(lt_vec3i_t a, lt_vec3i_t b) {
	return LT_VEC3I(a.x + b.x, a.y + b.y, a.z + b.z);
}

static LT_INLINE
lt_vec3i_t lt_v3isub(lt_vec3i_t a, lt_vec3i_t b) {
	return LT_VEC3I(a.x - b.x, a.y - b.y, a.z - b.z);
}

static LT_INLINE
lt_vec3i_t lt_v3idiv(lt_vec3i_t a, lt_vec3i_t b) {
	return LT_VEC3I(a.x / b.x, a.y / b.y, a.z / b.z);
}

static LT_INLINE
lt_vec3i_t lt_v3imul(lt_vec3i_t a, lt_vec3i_t b) {
	return LT_VEC3I(a.x * b.x, a.y * b.y, a.z * b.z);
}


static LT_INLINE
lt_vec3i_t lt_v3iaddi(lt_vec3i_t a, i32 f) {
	return LT_VEC3I(a.x + f, a.y + f, a.z + f);
}

static LT_INLINE
lt_vec3i_t lt_v3isubi(lt_vec3i_t a, i32 f) {
	return LT_VEC3I(a.x - f, a.y - f, a.z - f);
}

static LT_INLINE
lt_vec3i_t lt_v3idivi(lt_vec3i_t a, i32 f) {
	return LT_VEC3I(a.x / f, a.y / f, a.z / f);
}

static LT_INLINE
lt_vec3i_t lt_v3imodi(lt_vec3i_t a, i32 f) {
	return LT_VEC3I(a.x % f, a.y % f, a.z % f);
}

static LT_INLINE
lt_vec3i_t lt_v3imuli(lt_vec3i_t a, i32 f) {
	return LT_VEC3I(a.x * f, a.y * f, a.z * f);
}


static LT_INLINE
lt_vec3i_t lt_v3ineg(lt_vec3i_t v) {
	return LT_VEC3I(-v.x, -v.y, -v.z);
}

static LT_INLINE
lt_vec3i_t lt_v3iabs(lt_vec3i_t v) {
	return LT_VEC3I(lt_abs_i32(v.x), lt_abs_i32(v.y), lt_abs_i32(v.z));
}

static LT_INLINE
int lt_v3iequ(lt_vec3i_t a, lt_vec3i_t b) {
	return a.x == b.x && a.y == b.y && a.z == b.z;
}


static LT_INLINE
lt_vec3i_t lt_v3imin(lt_vec3i_t a, lt_vec3i_t b) {
	return LT_VEC3I(lt_min_i32(a.x, b.x), lt_min_i32(a.y, b.y), lt_min_i32(a.z, b.z));
}

static LT_INLINE
lt_vec3i_t lt_v3imax(lt_vec3i_t a, lt_vec3i_t b) {
	return LT_VEC3I(lt_max_i32(a.x, b.x), lt_max_i32(a.y, b.y), lt_max_i32(a.z, b.z));
}

static LT_INLINE
lt_vec3i_t lt_v3iclamp(lt_vec3i_t v, lt_vec3i_t min, lt_vec3i_t max) {
	return LT_VEC3I(lt_clamp_i32(v.x, min.x, max.x), lt_clamp_i32(v.y, min.y, max.y), lt_clamp_i32(v.z, min.z, max.z));
}

static LT_INLINE
lt_vec3i_t lt_v3iclampi(lt_vec3i_t v, i32 min, i32 max) {
	return LT_VEC3I(lt_clamp_i32(v.x, min, max), lt_clamp_i32(v.y, min, max), lt_clamp_i32(v.z, min, max));
}

static LT_INLINE
i32 lt_v3imincomp(lt_vec3i_t v) {
	return lt_min_i32(v.z, lt_min_i32(v.x, v.y));
}

static LT_INLINE
i32 lt_v3imaxcomp(lt_vec3i_t v) {
	return lt_max_i32(v.z, lt_max_i32(v.x, v.y));
}


static LT_INLINE
lt_vec3_t lt_v3itof(lt_vec3i_t v) {
	return LT_VEC3(v.x, v.y, v.z);
}

#endif