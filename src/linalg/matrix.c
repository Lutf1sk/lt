#include <lt/linalg.h>
#include <lt/mem.h>
#include <lt/lt.h>

#include <math.h>

// ----- mat2

void lt_mat2_identity(lt_mat2_t mat) {
	static lt_mat2_t src = LT_MAT2_IDENTITY_INIT;
	memcpy(mat, &src, sizeof(lt_mat2_t));
}

void lt_mat2_mul(lt_mat2_t dst, lt_mat2_t src) {
	lt_vec2_mul_mat2(dst[0], src);
	lt_vec2_mul_mat2(dst[1], src);
}


// ----- mat3

void lt_mat3_identity(lt_mat3_t mat) {
	static lt_mat3_t src = LT_MAT3_IDENTITY_INIT;
	memcpy(mat, &src, sizeof(lt_mat3_t));
}

void lt_mat3_mul(lt_mat3_t dst, lt_mat3_t src) {
	lt_vec3_mul_mat3(dst[0], src);
	lt_vec3_mul_mat3(dst[1], src);
	lt_vec3_mul_mat3(dst[2], src);
}


// ----- mat4

void lt_mat4_identity(lt_mat4_t mat) {
	static lt_mat4_t src = LT_MAT4_IDENTITY_INIT;
	memcpy(mat, &src, sizeof(lt_mat4_t));
}

void lt_mat4_perspective(lt_mat4_t mat, f32 fov, f32 aspect, f32 near, f32 far) {
	memset(mat, 0, sizeof(*mat));

	f32 f = 1.0f / tanf(fov * 0.5f);
	f32 fn = 1.0f / (near - far);

	mat[0][0] = f / aspect;
	mat[1][1] = f;
	mat[2][2] = -far * fn;
	mat[2][3] = 1.0f;
	mat[3][2] = near * far * fn;
}

void lt_mat4_ortho(lt_mat4_t mat, f32 left, f32 right, f32 bottom, f32 top, f32 near, f32 far) {
	LT_ASSERT_NOT_REACHED();
}

void lt_mat4_view(lt_mat4_t mat, lt_vec3_t eye, lt_vec3_t center, lt_vec3_t up) {
	lt_vec3_t f, u, s;

	lt_vec3_sub(center, eye, f);
	lt_vec3_normalize(f);

	lt_vec3_cross(f, up, s);
	lt_vec3_normalize(s);
	lt_vec3_cross(s, f, u);

	mat[0][0] = s[0];
	mat[0][1] = u[0];
	mat[0][2] = f[0];
	mat[0][3] = 0.0f;

	mat[1][0] = s[1];
	mat[1][1] = u[1];
	mat[1][2] = f[1];
	mat[1][3] = 0.0f;

	mat[2][0] = s[2];
	mat[2][1] = u[2];
	mat[2][2] = f[2];
	mat[2][3] = 0.0f;

	mat[3][0] = -lt_vec3_dot(s, eye);
	mat[3][1] = -lt_vec3_dot(u, eye);
	mat[3][2] = -lt_vec3_dot(f, eye);
	mat[3][3] = 1.0f;
}

void lt_mat4_mul(lt_mat4_t dst, lt_mat4_t src) {
	lt_vec4_mul_mat4(dst[0], src);
	lt_vec4_mul_mat4(dst[1], src);
	lt_vec4_mul_mat4(dst[2], src);
	lt_vec4_mul_mat4(dst[3], src);
}

