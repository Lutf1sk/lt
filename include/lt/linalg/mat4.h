#ifndef LT_LINALG_MAT4_H
#define LT_LINALG_MAT4_H 1

#ifdef LT_LINALG_SHORTEN_NAMES
#	define mat4 lt_mat4_t

#	define MAT4_INIT(i1, i2, i3, i4, j1, j2, j3, j4, k1, k2, k3, k4, l1, l2, l3, l4) { .data = { \
		{ (i1), (i2), (i3), (i4) }, \
		{ (j1), (j2), (j3), (j4) }, \
		{ (k1), (k2), (k3), (k4) }, \
		{ (l1), (l2), (l3), (l4) }, } }
#	define MAT4(i1, i2, i3, i4, j1, j2, j3, j4, k1, k2, k3, k4, l1, l2, l3, l4) ((lt_mat4_t)LT_MAT4_INIT(i1, i2, i3, i4, j1, j2, j3, j4, k1, k2, k3, k4, l1, l2, l3, l4))
#	define MAT4V_INIT(i, j, k, l) { .vdata = { (i), (j), (k), (l) } }
#	define MAT4V(i, j, k, l) ((lt_mat4_t)LT_MAT4V_INIT(i, j, k, l))
#	define MAT4_IDENTITY_INIT LT_MAT4_INIT( \
		1.0f, 0.0f, 0.0f, 0.0f, \
		0.0f, 1.0f, 0.0f, 0.0f, \
		0.0f, 0.0f, 1.0f, 0.0f, \
		0.0f, 0.0f, 0.0f, 1.0f)
#	define MAT4_IDENTITY ((lt_mat4_t)LT_MAT4_IDENTITY_INIT)

#	define v4mulm lt_v4mulm
#	define v3mulm4 lt_v3mulm4

#	define m4mul lt_m4mul
#	define m4perspective lt_m4perspective
#	define m4ortho lt_m4ortho
#	define m4view lt_m4view
#	define m4euler lt_m4euler
#	define m4quat lt_m4quat
#	define m4translate lt_m4translate
#	define m4scale lt_m4scale
#	define m4model lt_m4model
#	define m4inverse lt_m4inverse
#endif

static LT_INLINE
lt_vec4_t lt_v4mulm(lt_vec4_t v, const lt_mat4_t m[static 1]) {
	return LT_VEC4(
		m->data[0][0] * v.x + m->data[1][0] * v.y + m->data[2][0] * v.z + m->data[3][0] * v.w,
		m->data[0][1] * v.x + m->data[1][1] * v.y + m->data[2][1] * v.z + m->data[3][1] * v.w,
		m->data[0][2] * v.x + m->data[1][2] * v.y + m->data[2][2] * v.z + m->data[3][2] * v.w,
		m->data[0][3] * v.x + m->data[1][3] * v.y + m->data[2][3] * v.z + m->data[3][3] * v.w
	);
}

static LT_INLINE
lt_vec3_t lt_v3mulm4(lt_vec3_t v, f32 w, const lt_mat4_t m[static 1]) {
	return lt_v4mulm(LT_VEC4(v.x, v.y, v.z, w), m).xyz;
}


static LT_INLINE
lt_mat4_t lt_m4mul(const lt_mat4_t m1[static 1], const lt_mat4_t m2[static 1]) {
	return LT_MAT4V(
		lt_v4mulm(m2->i, m1),
		lt_v4mulm(m2->j, m1),
		lt_v4mulm(m2->k, m1),
		lt_v4mulm(m2->l, m1)
	);
}

lt_mat4_t lt_m4perspective(f32 fov, f32 aspect, f32 near, f32 far);
lt_mat4_t lt_m4ortho(f32 left, f32 right, f32 bottom, f32 top, f32 near, f32 far);

lt_mat4_t lt_m4view(lt_vec3_t pos, lt_vec3_t fwd, lt_vec3_t up);

lt_mat4_t lt_m4quat(lt_quat_t quat);
lt_mat4_t lt_m4euler(lt_vec3_t angles);

static LT_INLINE
lt_mat4_t lt_m4translate(lt_vec3_t pos) {
	return LT_MAT4V(
		LT_VEC4(1, 0, 0, 0),
		LT_VEC4(0, 1, 0, 0),
		LT_VEC4(0, 0, 1, 0),
		LT_VEC4(pos.x, pos.y, pos.z, 1)
	);
}

static LT_INLINE
lt_mat4_t lt_m4scale(lt_vec3_t scale) {
	return LT_MAT4V(
		LT_VEC4(scale.x, 0, 0, 0),
		LT_VEC4(0, scale.y, 0, 0),
		LT_VEC4(0, 0, scale.z, 0),
		LT_VEC4(0, 0, 0, 1)
	);
}

lt_mat4_t lt_m4model(lt_vec3_t pos, lt_vec3_t rot, lt_vec3_t scale);

lt_mat4_t lt_m4inverse(const lt_mat4_t m[static 1]);

#endif