#include <lt/linalg.h>
#include <lt/mem.h>
#include <lt/lt.h>

#include <math.h>

lt_mat4_t lt_m4perspective(f32 fov, f32 aspect, f32 near, f32 far) {
	f32 f = 1.0f / tanf(fov * 0.5f);
	f32 fn = 1.0f / (near - far);

	lt_mat4_t mat;
	memset(&mat, 0, sizeof(lt_mat4_t));
	mat.data[0][0] = f / aspect;
	mat.data[1][1] = f;
	mat.data[2][2] = -far * fn;
	mat.data[2][3] = 1.0f;
	mat.data[3][2] = near * far * fn;
	return mat;
}

lt_mat4_t lt_m4ortho(f32 left, f32 right, f32 bottom, f32 top, f32 near, f32 far) {
	f32 xspace = 1.0f / (right - left);
	f32 yspace = 1.0f / (top - bottom);
	f32 zspace = -1.0f / (far - near);

	lt_mat4_t mat;
	memset(&mat, 0, sizeof(lt_mat4_t));
	mat.data[0][0] = 2.0f * xspace;
	mat.data[1][1] = 2.0f * yspace;
	mat.data[2][2] = -zspace;
	mat.data[3][0] = -(right + left) * xspace;
	mat.data[3][1] = -(top + bottom) * yspace;
	mat.data[3][2] = near * zspace;
	mat.data[3][3] = 1.0f;
	return mat;
}

lt_mat4_t lt_m4view(lt_vec3_t pos, lt_vec3_t fwd, lt_vec3_t up) {
	lt_vec3_t f, u, s;

	f = lt_v3normalize(fwd);
	s = lt_v3normalize(lt_v3cross(f, up));
	u = lt_v3cross(s, f);

	return LT_MAT4(
		s.x, u.x, f.x, 0.0f,
		s.y, u.y, f.y, 0.0f,
		s.z, u.z, f.z, 0.0f,
		-lt_v3dot(s, pos), -lt_v3dot(u, pos), -lt_v3dot(f, pos), 1.0f
	);
}

lt_mat4_t lt_m4euler(lt_vec3_t angles) {
	float cx, cy, cz, sx, sy, sz, czsx, cxcz, sysz;

	sx   = sinf(angles.x); cx = cosf(angles.x);
	sy   = sinf(angles.y); cy = cosf(angles.y);
	sz   = sinf(angles.z); cz = cosf(angles.z);

	czsx = cz * sx;
	cxcz = cx * cz;
	sysz = sy * sz;

	return LT_MAT4(
		cy * cz, czsx * sy + cx * sz, -cxcz * sy + sx * sz, 0.0f,
		-cy * sz, cxcz - sx * sysz, czsx + cx * sysz, 0.0f,
		sy, -cy * sx, cx * cy, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f
	);
}

