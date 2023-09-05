#ifndef LT_LINALG_MAT2_H
#define LT_LINALG_MAT2_H 1

#ifdef LT_LINALG_SHORTEN_NAMES
#	define mat2 lt_mat2_t

#	define MAT2_INIT(i1, i2, j1, j2) { .data = { { (i1), (i2) }, { (j1), (j2) } } }
#	define MAT2(i1, i2, j1, j2) ((lt_mat2_t){ .data = { { (i1), (i2) }, { (j1), (j2) } } })
#	define MAT2V_INIT(i, j) { .vdata = { (i), (j) } }
#	define MAT2V(i, j) ((lt_mat2_t) { .vdata = { (i), (j) } })
#	define MAT2_IDENTITY_INIT { .data = { { 1.0f, 0.0f }, { 0.0f, 1.0f } } }
#	define MAT2_IDENTITY ((lt_mat2_t){ .data = { { 1.0f, 0.0f }, { 0.0f, 1.0f } } })

#	define v2mulm lt_v2mulm

#	define m2mul lt_m2mul
#endif

static LT_INLINE
lt_vec2_t lt_v2mulm(lt_vec2_t v, const lt_mat2_t* m) {
	return LT_VEC2(
		m->data[0][0] * v.x + m->data[1][0] * v.y,
		m->data[0][1] * v.x + m->data[1][1] * v.y
	);
}


static LT_INLINE
lt_mat2_t lt_m2mul(const lt_mat2_t* m1, const lt_mat2_t* m2) {
	return LT_MAT2V(
		lt_v2mulm(m1->i, m2),
		lt_v2mulm(m1->j, m2)
	);
}

#endif