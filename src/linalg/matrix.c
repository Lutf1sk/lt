#include <lt/linalg.h>
#include <lt/mem.h>

void lt_mat2_identity(lt_mat2_t* mat) {
	static lt_mat2_t src = LT_MAT2_IDENTITY;
	memcpy(mat, &src, sizeof(lt_mat2_t));
}

void lt_mat3_identity(lt_mat3_t* mat) {
	static lt_mat3_t src = LT_MAT3_IDENTITY;
	memcpy(mat, &src, sizeof(lt_mat3_t));
}

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

