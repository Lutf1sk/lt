#ifndef LT_LINALG_VEC2I_H
#define LT_LINALG_VEC2I_H 1

#ifdef LT_LINALG_SHORTEN_NAMES
#	define vec2i lt_vec2i_t

#	define VEC2I_INIT(x, y) { .data = { (x), (y) } }
#	define VEC2I(x, y) ((lt_vec2i_t){ .data = { (x), (y) } })

#	define v2ifill lt_v2ifill

#	define v2iadd lt_v2iadd
#	define v2isub lt_v2isub
#	define v2idiv lt_v2idiv
#	define v2imod lt_v2imod
#	define v2imul lt_v2imul

#	define v2iaddi lt_v2iaddi
#	define v2isubi lt_v2isubi
#	define v2idivi lt_v2idivi
#	define v2imodi lt_v2imodi
#	define v2imuli lt_v2imuli

#	define v2ineg lt_v2ineg
#	define v2iabs lt_v2iabs
#	define v2iequ lt_v2iequ

#	define v2imin lt_v2imin
#	define v2imax lt_v2imax
#	define v2iclamp lt_v2iclamp
#	define v2iclampi lt_v2iclampi
#	define v2imincomp lt_v2imincomp
#	define v2imaxcomp lt_v2imaxcomp

#	define v2itof lt_v2itof
#endif

static LT_INLINE
lt_vec2i_t lt_v2ifill(i32 v)  {
	return LT_VEC2I(v, v);
}


static LT_INLINE
lt_vec2i_t lt_v2iadd(lt_vec2i_t a, lt_vec2i_t b) {
	return LT_VEC2I(a.x + b.x, a.y + b.y);
}

static LT_INLINE
lt_vec2i_t lt_v2isub(lt_vec2i_t a, lt_vec2i_t b) {
	return LT_VEC2I(a.x - b.x, a.y - b.y);
}

static LT_INLINE
lt_vec2i_t lt_v2idiv(lt_vec2i_t a, lt_vec2i_t b) {
	return LT_VEC2I(a.x / b.x, a.y / b.y);
}

static LT_INLINE
lt_vec2i_t lt_v2imod(lt_vec2i_t a, lt_vec2i_t b) {
	return LT_VEC2I(a.x % b.x, a.y % b.y);
}

static LT_INLINE
lt_vec2i_t lt_v2imul(lt_vec2i_t a, lt_vec2i_t b) {
	return LT_VEC2I(a.x * b.x, a.y * b.y);
}


static LT_INLINE
lt_vec2i_t lt_v2iaddi(lt_vec2i_t a, i32 f) {
	return LT_VEC2I(a.x + f, a.y + f);
}

static LT_INLINE
lt_vec2i_t lt_v2isubi(lt_vec2i_t a, i32 f) {
	return LT_VEC2I(a.x - f, a.y - f);
}

static LT_INLINE
lt_vec2i_t lt_v2idivi(lt_vec2i_t a, i32 f) {
	return LT_VEC2I(a.x / f, a.y / f);
}

static LT_INLINE
lt_vec2i_t lt_v2imodi(lt_vec2i_t a, i32 f) {
	return LT_VEC2I(a.x % f, a.y % f);
}

static LT_INLINE
lt_vec2i_t lt_v2imuli(lt_vec2i_t a, i32 f) {
	return LT_VEC2I(a.x * f, a.y * f);
}


static LT_INLINE
lt_vec2i_t lt_v2ineg(lt_vec2i_t v) {
	return LT_VEC2I(-v.x, -v.y);
}

static LT_INLINE
lt_vec2i_t lt_v2iabs(lt_vec2i_t v) {
	return LT_VEC2I(lt_abs(v.x), lt_abs(v.y));
}

static LT_INLINE
int lt_v2iequ(lt_vec2i_t a, lt_vec2i_t b) {
	return a.x == b.x && a.y == b.y;
}


static LT_INLINE
lt_vec2i_t lt_v2imin(lt_vec2i_t a, lt_vec2i_t b) {
	return LT_VEC2I(lt_min(a.x, b.x), lt_min(a.y, b.y));
}

static LT_INLINE
lt_vec2i_t lt_v2imax(lt_vec2i_t a, lt_vec2i_t b) {
	return LT_VEC2I(lt_max(a.x, b.x), lt_max(a.y, b.y));
}

static LT_INLINE
lt_vec2i_t lt_v2iclamp(lt_vec2i_t v, lt_vec2i_t min, lt_vec2i_t max) {
	return LT_VEC2I(lt_clamp(v.x, min.x, max.x), lt_clamp(v.y, min.y, max.y));
}

static LT_INLINE
lt_vec2i_t lt_v2iclampi(lt_vec2i_t v, i32 min, i32 max) {
	return LT_VEC2I(lt_clamp(v.x, min, max), lt_clamp(v.y, min, max));
}

static LT_INLINE
i32 lt_v2imincomp(lt_vec2i_t v) {
	return lt_min(v.x, v.y);
}

static LT_INLINE
i32 lt_v2imaxcomp(lt_vec2i_t v) {
	return lt_max(v.x, v.y);
}


static LT_INLINE
lt_vec2_t lt_v2itof(lt_vec2i_t v) {
	return LT_VEC2(v.x, v.y);
}

#endif