#include <lt/linalg.h>
#include <lt/mem.h>

// ----- mat2

void lt_mat2_identity(lt_mat2_t* mat) {
	static lt_mat2_t src = LT_MAT2_IDENTITY;
	memcpy(mat, &src, sizeof(lt_mat2_t));
}

void lt_mat2_mul(lt_mat2_t* dst, lt_mat2_t* src) {
	lt_vec2_mul_mat2(&dst->i, src);
	lt_vec2_mul_mat2(&dst->j, src);
}


// ----- mat3

void lt_mat3_identity(lt_mat3_t* mat) {
	static lt_mat3_t src = LT_MAT3_IDENTITY;
	memcpy(mat, &src, sizeof(lt_mat3_t));
}

void lt_mat3_mul(lt_mat3_t* dst, lt_mat3_t* src) {
	lt_vec3_mul_mat3(&dst->i, src);
	lt_vec3_mul_mat3(&dst->j, src);
	lt_vec3_mul_mat3(&dst->k, src);
}


// ----- mat4

void lt_mat4_identity(lt_mat4_t* mat) {
	static lt_mat4_t src = LT_MAT4_IDENTITY;
	memcpy(mat, &src, sizeof(lt_mat4_t));
}

void lt_mat4_projection(lt_mat4_t* mat, int width, int height, f32 fov, f32 near, f32 far) {
	f32 w = width, h = height;

	f32 aspect = h / w;

	mat->i = LT_VEC4(aspect, 0, 0, 0);
	mat->j = LT_VEC4(0, 1, 0, 0);
	mat->k = LT_VEC4(0, 0, 1, 0);
	mat->l = LT_VEC4(0, 0, 0, 1);
}

void lt_mat4_mul(lt_mat4_t* dst, lt_mat4_t* src) {
	lt_vec4_mul_mat4(&dst->i, src);
	lt_vec4_mul_mat4(&dst->j, src);
	lt_vec4_mul_mat4(&dst->k, src);
	lt_vec4_mul_mat4(&dst->l, src);
}

