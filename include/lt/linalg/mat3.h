#ifndef LT_LINALG_MAT3_H
#define LT_LINALG_MAT3_H 1

#ifdef LT_LINALG_SHORTEN_NAMES
#	define mat3 lt_mat3_t

#	define MAT3_INIT(i1, i2, i3, j1, j2, j3, k1, k2, k3) { .data = { \
		{ (i1), (i2), (i3) }, \
		{ (j1), (j2), (j3) }, \
		{ (k1), (k2), (k3) } } }
#	define MAT3(i1, i2, i3, j1, j2, j3, k1, k2, k3) ((lt_mat3_t)LT_MAT3_INIT(i1, i2, i3, j1, j2, j3, k1, k2, k3))
#	define MAT3V_INIT(i, j, k) { .vdata = { (i), (j), (k) } }
#	define MAT3V(i, j, k) ((lt_mat3_t)LT_MAT3V_INIT(i, j, k))
#	define MAT3_IDENTITY_INIT LT_MAT3_INIT( \
		1.0f, 0.0f, 0.0f, \
		0.0f, 1.0f, 0.0f, \
		0.0f, 0.0f, 1.0f)
#	define MAT3_IDENTITY ((lt_mat3_t)LT_MAT3_IDENTITY_INIT)

#	define v3mulm lt_v3mulm

#	define m3mul lt_m3mul
#	define m3quat lt_m3quat
#endif

static LT_INLINE
lt_vec3_t lt_v3mulm(lt_vec3_t v, const lt_mat3_t m[static 1]) {
	return LT_VEC3(
		m->data[0][0] * v.x + m->data[1][0] * v.y + m->data[2][0] * v.z,
		m->data[0][1] * v.x + m->data[1][1] * v.y + m->data[2][1] * v.z,
		m->data[0][2] * v.x + m->data[1][2] * v.y + m->data[2][2] * v.z
	);
}


static LT_INLINE
lt_mat3_t lt_m3mul(const lt_mat3_t m1[static 1], const lt_mat3_t m2[static 1]) {
	return LT_MAT3V(
		lt_v3mulm(m1->i, m2),
		lt_v3mulm(m1->j, m2),
		lt_v3mulm(m1->k, m2)
	);
}

lt_mat3_t lt_m3quat(lt_quat_t quat);

#endif