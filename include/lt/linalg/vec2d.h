#ifndef LT_LINALG_VEC2D_H
#define LT_LINALG_VEC2D_H 1

#ifdef LT_LINALG_SHORTEN_NAMES
#	define vec2d lt_vec2d_t

#	define VEC2D_INIT(x, y) { .data = { (x), (y) } }
#	define VEC2D(x, y) ((lt_vec2d_t){ .data = { (x), (y) } })

#	define v2dfill lt_v2dfill

#	define v2dadd lt_v2dadd
#	define v2dsub lt_v2dsub
#	define v2ddiv lt_v2ddiv
#	define v2dmod lt_v2dmod
#	define v2dmul lt_v2dmul

#	define v2daddf lt_v2daddf
#	define v2dsubf lt_v2dsubf
#	define v2ddivf lt_v2ddivf
#	define v2dmodf lt_v2dmodf
#	define v2dmulf lt_v2dmulf

#	define v2ddot lt_v2ddot
#	define v2dneg lt_v2dneg
#	define v2dmag lt_v2dmag
#	define v2ddist lt_v2ddist
#	define v2dnorm lt_v2dnorm
#	define v2dcross lt_v2dcross
#	define v2dabs lt_v2dabs
#	define v2dequ lt_v2dequ
#	define v2deque lt_v2deque
#	define v2dlerp lt_v2dlerp
#	define v2dlerpc lt_v2dlerpc

#	define v2dmin lt_v2dmin
#	define v2dmax lt_v2dmax
#	define v2dclamp lt_v2dclamp
#	define v2dclampf lt_v2dclampf
#	define v2dmincomp lt_v2dmincomp
#	define v2dmaxcomp lt_v2dmaxcomp

#	define v2dround lt_v2dround
#	define v2dfloor lt_v2dfloor
#	define v2dceil lt_v2dceil
#endif

static LT_INLINE
lt_vec2d_t lt_v2dfill(f64 v)  {
	return LT_VEC2D(v, v);
}


static LT_INLINE
lt_vec2d_t lt_v2dadd(lt_vec2d_t a, lt_vec2d_t b) {
	return LT_VEC2D(a.x + b.x, a.y + b.y);
}

static LT_INLINE
lt_vec2d_t lt_v2dsub(lt_vec2d_t a, lt_vec2d_t b) {
	return LT_VEC2D(a.x - b.x, a.y - b.y);
}

static LT_INLINE
lt_vec2d_t lt_v2ddiv(lt_vec2d_t a, lt_vec2d_t b) {
	return LT_VEC2D(a.x / b.x, a.y / b.y);
}

static LT_INLINE
lt_vec2d_t lt_v2dmod(lt_vec2d_t a, lt_vec2d_t b) {
	return LT_VEC2D(fmod(a.x, b.x), fmod(a.y, b.y));
}

static LT_INLINE
lt_vec2d_t lt_v2dmul(lt_vec2d_t a, lt_vec2d_t b) {
	return LT_VEC2D(a.x * b.x, a.y * b.y);
}


static LT_INLINE
lt_vec2d_t lt_v2daddf(lt_vec2d_t a, f64 f) {
	return LT_VEC2D(a.x + f, a.y + f);
}

static LT_INLINE
lt_vec2d_t lt_v2dsubf(lt_vec2d_t a, f64 f) {
	return LT_VEC2D(a.x - f, a.y - f);
}

static LT_INLINE
lt_vec2d_t lt_v2ddivf(lt_vec2d_t a, f64 f) {
	return LT_VEC2D(a.x / f, a.y / f);
}

static LT_INLINE
lt_vec2d_t lt_v2dmodf(lt_vec2d_t a, f64 f) {
	return LT_VEC2D(fmod(a.x, f), fmod(a.y, f));
}

static LT_INLINE
lt_vec2d_t lt_v2dmulf(lt_vec2d_t a, f64 f) {
	return LT_VEC2D(a.x * f, a.y * f);
}


static LT_INLINE
f64 lt_v2ddot(lt_vec2d_t a, lt_vec2d_t b) {
	return a.x * b.x + a.y * b.y;
}

static LT_INLINE
lt_vec2d_t lt_v2dneg(lt_vec2d_t v) {
	return LT_VEC2D(-v.x, -v.y);
}

static LT_INLINE
f64 lt_v2dmag(lt_vec2d_t v) {
	return sqrt(v.x * v.x + v.y * v.y);
}

static LT_INLINE
f64 lt_v2ddist(lt_vec2d_t a, lt_vec2d_t b) {
	return lt_v2dmag(lt_v2dsub(a, b));
}

static LT_INLINE
lt_vec2d_t lt_v2dnorm(lt_vec2d_t v) {
	f64 m = lt_v2dmag(v);
	if (m == 0.0f)
		return LT_VEC2D(0.0f, 0.0f);
	return lt_v2dmulf(v, 1.0f / m);
}

static LT_INLINE
f64 lt_v2dcross(lt_vec2d_t a, lt_vec2d_t b) {
	return a.x * b.y - a.y * b.x;
}

static LT_INLINE
lt_vec2d_t lt_v2dabs(lt_vec2d_t v) {
	return LT_VEC2D(fabs(v.x), fabs(v.y));
}

static LT_INLINE
int lt_v2dequ(lt_vec2d_t a, lt_vec2d_t b) {
	return a.x == b.x && a.y == b.y;
}

static LT_INLINE
int lt_v2deque(lt_vec2d_t a, lt_vec2d_t b, f64 epsilon) {
	return fabs(a.x - b.x) < epsilon && fabs(a.y - b.y) < epsilon;
}

static LT_INLINE
lt_vec2d_t lt_v2dlerp(lt_vec2d_t a, lt_vec2d_t b, f64 t) {
	return lt_v2dadd(lt_v2dmulf(a, t), lt_v2dmulf(b, 1.0f - t));
}

static LT_INLINE
lt_vec2d_t lt_v2dlerpc(lt_vec2d_t a, lt_vec2d_t b, f64 t) {
	return lt_v2dlerp(a, b, lt_clamp_f64(t, 0.0f, 1.0f));
}


static LT_INLINE
lt_vec2d_t lt_v2dmin(lt_vec2d_t a, lt_vec2d_t b) {
	return LT_VEC2D(lt_min_f64(a.x, b.x), lt_min_f64(a.y, b.y));
}

static LT_INLINE
lt_vec2d_t lt_v2dmax(lt_vec2d_t a, lt_vec2d_t b) {
	return LT_VEC2D(lt_max_f64(a.x, b.x), lt_max_f64(a.y, b.y));
}

static LT_INLINE
lt_vec2d_t lt_v2dclamp(lt_vec2d_t v, lt_vec2d_t min, lt_vec2d_t max) {
	return LT_VEC2D(lt_clamp_f64(v.x, min.x, max.x), lt_clamp_f64(v.y, min.y, max.y));
}

static LT_INLINE
lt_vec2d_t lt_v2dclampf(lt_vec2d_t v, f64 min, f64 max) {
	return LT_VEC2D(lt_clamp_f64(v.x, min, max), lt_clamp_f64(v.y, min, max));
}

static LT_INLINE
f64 lt_v2dmincomp(lt_vec2d_t v) {
	return lt_min_f64(v.x, v.y);
}

static LT_INLINE
f64 lt_v2dmaxcomp(lt_vec2d_t v) {
	return lt_max_f64(v.x, v.y);
}


static LT_INLINE
lt_vec2i_t lt_v2dround(lt_vec2d_t v) {
	return LT_VEC2I(round(v.x), round(v.y));
}

static LT_INLINE
lt_vec2i_t lt_v2dfloor(lt_vec2d_t v) {
	return LT_VEC2I(floor(v.x), floor(v.y));
}

static LT_INLINE
lt_vec2i_t lt_v2dceil(lt_vec2d_t v) {
	return LT_VEC2I(ceil(v.x), ceil(v.y));
}

#endif