#include <lt/linalg.h>
#include <lt/mem.h>
#include <lt/lt.h>

#include <math.h>

// ----- mat2

void lt_mat2_identity(lt_mat2_t mat) {
	static lt_mat2_t src = LT_MAT2_IDENTITY_INIT;
	memcpy(mat, &src, sizeof(lt_mat2_t));
}

// ----- mat3

void lt_mat3_identity(lt_mat3_t mat) {
	static lt_mat3_t src = LT_MAT3_IDENTITY_INIT;
	memcpy(mat, &src, sizeof(lt_mat3_t));
}

// ----- mat4

void lt_mat4_identity(lt_mat4_t mat) {
	static lt_mat4_t src = LT_MAT4_IDENTITY_INIT;
	memcpy(mat, &src, sizeof(lt_mat4_t));
}

void lt_mat4_perspective(lt_mat4_t mat, f32 fov, f32 aspect, f32 near, f32 far) {
	memset(mat, 0, sizeof(lt_mat4_t));

	f32 f = 1.0f / tanf(fov * 0.5f);
	f32 fn = 1.0f / (near - far);

	mat[0][0] = f / aspect;
	mat[1][1] = f;
	mat[2][2] = -far * fn;
	mat[2][3] = 1.0f;
	mat[3][2] = near * far * fn;
}

void lt_mat4_ortho(lt_mat4_t mat, f32 left, f32 right, f32 bottom, f32 top, f32 near, f32 far) {
	memset(mat, 0, sizeof(lt_mat4_t));

	f32 xspace = 1.0f / (right - left);
	f32 yspace = 1.0f / (top - bottom);
	f32 zspace = -1.0f / (far - near);

	mat[0][0] = 2.0f * xspace;
	mat[1][1] = 2.0f * yspace;
	mat[2][2] = -zspace;
	mat[3][0] = -(right + left) * xspace;
	mat[3][1] = -(top + bottom) * yspace;
	mat[3][2] = near * zspace;
	mat[3][3] = 1.0f;
}

void lt_mat4_view(lt_mat4_t mat, lt_vec3_t pos, lt_vec3_t fwd, lt_vec3_t up) {
	lt_vec3_t f, u, s;

	lt_vec3_copy(f, fwd);
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

	mat[3][0] = -lt_vec3_dot(s, pos);
	mat[3][1] = -lt_vec3_dot(u, pos);
	mat[3][2] = -lt_vec3_dot(f, pos);
	mat[3][3] = 1.0f;
}

void lt_mat4_euler(lt_vec3_t angles, lt_mat4_t dst) {
	float cx, cy, cz, sx, sy, sz, czsx, cxcz, sysz;

	sx   = sinf(angles[0]); cx = cosf(angles[0]);
	sy   = sinf(angles[1]); cy = cosf(angles[1]);
	sz   = sinf(angles[2]); cz = cosf(angles[2]);

	czsx = cz * sx;
	cxcz = cx * cz;
	sysz = sy * sz;

	dst[0][0] =  cy * cz;
	dst[0][1] =  czsx * sy + cx * sz;
	dst[0][2] = -cxcz * sy + sx * sz;

	dst[1][0] = -cy * sz;
	dst[1][1] =  cxcz - sx * sysz;
	dst[1][2] =  czsx + cx * sysz;

	dst[2][0] =  sy;
	dst[2][1] = -cy * sx;
	dst[2][2] =  cx * cy;

	dst[0][3] =  0.0f;
	dst[1][3] =  0.0f;
	dst[2][3] =  0.0f;

	dst[3][0] =  0.0f;
	dst[3][1] =  0.0f;
	dst[3][2] =  0.0f;
	dst[3][3] =  1.0f;
}

