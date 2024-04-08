#ifndef LT_LINALG_VEC4D_H
#define LT_LINALG_VEC4D_H 1

#ifdef LT_LINALG_SHORTEN_NAMES
#	define vec4d lt_vec4d_t

#	define VEC4D_INIT(x, y, z, w) { .data = { (x), (y), (z), (w) } }
#	define VEC4D(x, y, z, w) ((lt_vec4d_t){ .data = { (x), (y), (z), (w) } })

#	define v4dfill lt_v4dfill

#	define v4dadd lt_v4dadd
#	define v4dsub lt_v4dsub
#	define v4ddiv lt_v4ddiv
#	define v4dmod lt_v4dmod
#	define v4dmul lt_v4dmul

#	define v4daddf lt_v4daddf
#	define v4dsubf lt_v4dsubf
#	define v4ddivf lt_v4ddivf
#	define v4dmodf lt_v4dmodf
#	define v4dmulf lt_v4dmulf

#	define v4ddot lt_v4ddot
#	define v4dneg lt_v4dneg
#	define v4dmag lt_v4dmag
#	define v4ddist lt_v4ddist
#	define v4dnorm lt_v4dnorm
#	define v4dabs lt_v4dabs
#	define v4dequ lt_v4dequ
#	define v4deque lt_v4deque
#	define v4dlerp lt_v4dlerp
#	define v4dlerpc lt_v4dlerpc

#	define v4dmin lt_v4dmin
#	define v4dmax lt_v4dmax
#	define v4dclamp lt_v4dclamp
#	define v4dclampf lt_v4dclampf
#	define v4dmincomp lt_v4dmincomp
#	define v4dmaxcomp lt_v4dmaxcomp

#	define v4dround lt_v4dround
#	define v4dfloor lt_v4dfloor
#	define v4dceil lt_v4dceil
#endif


static LT_INLINE
lt_vec4d_t lt_v4dfill(f64 v)  {
	return LT_VEC4D(v, v, v, v);
}


static LT_INLINE
lt_vec4d_t lt_v4dadd(lt_vec4d_t a, lt_vec4d_t b) {
	return LT_VEC4D(a.x + b.x, a.y + b.y, a.z + b.z, a.w + b.w);
}

static LT_INLINE
lt_vec4d_t lt_v4dsub(lt_vec4d_t a, lt_vec4d_t b) {
	return LT_VEC4D(a.x - b.x, a.y - b.y, a.z - b.z, a.w - b.w);
}

static LT_INLINE
lt_vec4d_t lt_v4ddiv(lt_vec4d_t a, lt_vec4d_t b) {
	return LT_VEC4D(a.x / b.x, a.y / b.y, a.z / b.z, a.w / b.w);
}

static LT_INLINE
lt_vec4d_t lt_v4dmod(lt_vec4d_t a, lt_vec4d_t b) {
	return LT_VEC4D(fmod(a.x, b.x), fmod(a.y, b.y), fmod(a.z, b.z), fmod(a.w, b.w));
}

static LT_INLINE
lt_vec4d_t lt_v4dmul(lt_vec4d_t a, lt_vec4d_t b) {
	return LT_VEC4D(a.x * b.x, a.y * b.y, a.z * b.z, a.w * b.w);
}


static LT_INLINE
lt_vec4d_t lt_v4daddf(lt_vec4d_t a, f64 f) {
	return LT_VEC4D(a.x + f, a.y + f, a.z + f, a.w + f);
}

static LT_INLINE
lt_vec4d_t lt_v4dsubf(lt_vec4d_t a, f64 f) {
	return LT_VEC4D(a.x - f, a.y - f, a.z - f, a.w - f);
}

static LT_INLINE
lt_vec4d_t lt_v4ddivf(lt_vec4d_t a, f64 f) {
	return LT_VEC4D(a.x / f, a.y / f, a.z / f, a.w / f);
}

static LT_INLINE
lt_vec4d_t lt_v4dmodf(lt_vec4d_t a, f64 f) {
	return LT_VEC4D(fmod(a.x, f), fmod(a.y, f), fmod(a.z, f), fmod(a.w, f));
}

static LT_INLINE
lt_vec4d_t lt_v4dmulf(lt_vec4d_t a, f64 f) {
	return LT_VEC4D(a.x * f, a.y * f, a.z * f, a.w * f);
}


static LT_INLINE
f64 lt_v4ddot(lt_vec4d_t a, lt_vec4d_t b) {
	return a.x * b.x + a.y * b.y + a.z * b.z + a.w * b.w;
}

static LT_INLINE
lt_vec4d_t lt_v4dneg(lt_vec4d_t v) {
	return LT_VEC4D(-v.x, -v.y, -v.z, -v.w);
}

static LT_INLINE
f64 lt_v4dmag(lt_vec4d_t v) {
	f64 h = sqrt(v.x * v.x + v.y * v.y);
	f64 h2 = sqrt(v.z * v.z + h * h);
	return sqrt(v.w * v.w + h2 * h2);
}

static LT_INLINE
f64 lt_v4ddist(lt_vec4d_t a, lt_vec4d_t b) {
	return lt_v4dmag(lt_v4dsub(a, b));
}

static LT_INLINE
lt_vec4d_t lt_v4dnorm(lt_vec4d_t v) {
	f64 m = lt_v4dmag(v);
	if (m == 0.0f)
		return LT_VEC4D(0.0f, 0.0f, 0.0f, 0.0f);
	return lt_v4dmulf(v, 1.0f / m);
}

static LT_INLINE
lt_vec4d_t lt_v4dabs(lt_vec4d_t v) {
	return LT_VEC4D(fabs(v.x), fabs(v.y), fabs(v.z), fabs(v.w));
}

static LT_INLINE
int lt_v4dequ(lt_vec4d_t a, lt_vec4d_t b) {
	return a.x == b.x && a.y == b.y && a.z == b.z && a.w == b.w;
}

static LT_INLINE
int lt_v4deque(lt_vec4d_t a, lt_vec4d_t b, f64 epsilon) {
	return fabs(a.x - b.x) < epsilon && fabs(a.y - b.y) < epsilon && fabs(a.z - b.z) < epsilon && fabs(a.w - b.w) < epsilon;
}

static LT_INLINE
lt_vec4d_t lt_v4dlerp(lt_vec4d_t a, lt_vec4d_t b, f64 t) {
	return lt_v4dadd(lt_v4dmulf(a, t), lt_v4dmulf(b, 1.0f - t));
}

static LT_INLINE
lt_vec4d_t lt_v4dlerpc(lt_vec4d_t a, lt_vec4d_t b, f64 t) {
	return lt_v4dlerp(a, b, lt_clamp(t, 0.0f, 1.0f));
}


static LT_INLINE
lt_vec4d_t lt_v4dmin(lt_vec4d_t a, lt_vec4d_t b) {
	return LT_VEC4D(lt_min(a.x, b.x), lt_min(a.y, b.y), lt_min(a.z, b.z), lt_min(a.w, b.w));
}

static LT_INLINE
lt_vec4d_t lt_v4dmax(lt_vec4d_t a, lt_vec4d_t b) {
	return LT_VEC4D(lt_max(a.x, b.x), lt_max(a.y, b.y), lt_max(a.z, b.z), lt_max(a.w, b.w));
}

static LT_INLINE
lt_vec4d_t lt_v4dclamp(lt_vec4d_t v, lt_vec4d_t min, lt_vec4d_t max) {
	return LT_VEC4D(lt_clamp(v.x, min.x, max.x), lt_clamp(v.y, min.y, max.y), lt_clamp(v.z, min.z, max.z), lt_clamp(v.w, min.w, max.w));
}

static LT_INLINE
lt_vec4d_t lt_v4dclampf(lt_vec4d_t v, f64 min, f64 max) {
	return LT_VEC4D(lt_clamp(v.x, min, max), lt_clamp(v.y, min, max), lt_clamp(v.z, min, max), lt_clamp(v.w, min, max));
}

static LT_INLINE
f64 lt_v4dmincomp(lt_vec4d_t v) {
	return lt_min(v.w, lt_min(v.z, lt_min(v.x, v.y)));
}

static LT_INLINE
f64 lt_v4dmaxcomp(lt_vec4d_t v) {
	return lt_max(v.w, lt_max(v.z, lt_max(v.x, v.y)));
}


static LT_INLINE
lt_vec4i_t lt_v4dround(lt_vec4d_t v) {
	return LT_VEC4I(round(v.x), round(v.y), round(v.z), round(v.w));
}

static LT_INLINE
lt_vec4i_t lt_v4dfloor(lt_vec4d_t v) {
	return LT_VEC4I(floor(v.x), floor(v.y), floor(v.z), floor(v.w));
}

static LT_INLINE
lt_vec4i_t lt_v4dceil(lt_vec4d_t v) {
	return LT_VEC4I(ceil(v.x), ceil(v.y), ceil(v.z), ceil(v.w));
}

#endif