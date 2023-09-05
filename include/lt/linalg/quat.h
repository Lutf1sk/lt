#ifndef LT_LINALG_QUAT_H
#define LT_LINALG_QUAT_H

#ifdef LT_LINALG_SHORTEN_NAMES
#	define quat lt_quat_t

#	define QUAT_INIT(x, y, z, w) { .data = { (x), (y), (z), (w) } }
#	define QUAT(x, y, z, w) ((lt_quat_t){ .data = { (x), (y), (z), (w) } })

#	define QUAT_IDENTITY_INIT { .data = { 0.0f, 0.0f, 0.0f, 1.0f } }
#	define QUAT_IDENTITY ((lt_quat_t){ .data = { 0.0f, 0.0f, 0.0f, 1.0f } })

#	define qadd lt_qmul
#	define qsub lt_qsub
#	define qmul lt_qmul
#	define qaddf lt_qmulf
#	define qsubf lt_qsubf
#	define qdivf lt_qdivf
#	define qmulf lt_qmulf
#	define qdot lt_qdot
#	define qnorm lt_qnorm
#	define qequ lt_qequ
#endif

static LT_INLINE
lt_quat_t lt_qadd(lt_quat_t a, lt_quat_t b) {
	return LT_QUAT(a.x + b.x, a.y + b.y, a.z + b.z, a.w + b.w);
}

static LT_INLINE
lt_quat_t lt_qsub(lt_quat_t a, lt_quat_t b) {
	return LT_QUAT(a.x - b.x, a.y - b.y, a.z - b.z, a.w - b.w);
}

static LT_INLINE
lt_quat_t lt_qmul(lt_quat_t a, lt_quat_t b) {
	return LT_QUAT(
		a.w * b.x + a.x * b.w + a.y * b.z - a.z * b.y,
		a.w * b.y + a.y * b.w + a.z * b.x - a.x * b.z,
		a.w * b.z + a.z * b.w + a.x * b.y - a.y * b.x,
		a.w * b.w - a.x * b.x - a.y * b.y - a.z * b.z
	);
}

static LT_INLINE
lt_quat_t lt_qaddf(lt_quat_t q, f32 f) {
	return LT_QUAT(q.x + f, q.y + f, q.z + f, q.w + f);
}

static LT_INLINE
lt_quat_t lt_qsubf(lt_quat_t q, f32 f) {
	return LT_QUAT(q.x - f, q.y - f, q.z - f, q.w - f);
}

static LT_INLINE
lt_quat_t lt_qdivf(lt_quat_t q, f32 f) {
	return LT_QUAT(q.x / f, q.y / f, q.z / f, q.w / f);
}

static LT_INLINE
lt_quat_t lt_qmulf(lt_quat_t q, f32 f) {
	return LT_QUAT(q.x * f, q.y * f, q.z * f, q.w * f);
}

static LT_INLINE
f32 lt_qdot(lt_quat_t a, lt_quat_t b) {
	return a.x * b.x + a.y * b.y + a.z * b.z + a.w * b.w;
}

static LT_INLINE
lt_quat_t lt_qnorm(lt_quat_t q) {
	f32 mag = sqrt(lt_qdot(q, q));
	if (mag == 0.0f)
		return LT_QUAT(0.0f, 0.0f, 0.0f, 0.0f);
	return lt_qmulf(q, 1.0f / mag);
}

static LT_INLINE
int lt_qequ(lt_quat_t a, lt_quat_t b, f32 epsilon) {
	return fabs(a.x - b.x) < epsilon && fabs(a.y - b.y) < epsilon && fabs(a.z - b.z) < epsilon && fabs(a.w - b.w) < epsilon;
}

#endif