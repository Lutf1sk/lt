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

lt_mat4_t lt_m4inverse(const lt_mat4_t* mat) {
	float t[6];
	float det;
	float
		a = mat->data[0][0], b = mat->data[0][1], c = mat->data[0][2], d = mat->data[0][3],
		e = mat->data[1][0], f = mat->data[1][1], g = mat->data[1][2], h = mat->data[1][3],
		i = mat->data[2][0], j = mat->data[2][1], k = mat->data[2][2], l = mat->data[2][3],
		m = mat->data[3][0], n = mat->data[3][1], o = mat->data[3][2], p = mat->data[3][3];

	t[0] = k * p - o * l; t[1] = j * p - n * l; t[2] = j * o - n * k;
	t[3] = i * p - m * l; t[4] = i * o - m * k; t[5] = i * n - m * j;

	lt_mat4_t res;
	res.data[0][0] =  f * t[0] - g * t[1] + h * t[2];
	res.data[1][0] =-(e * t[0] - g * t[3] + h * t[4]);
	res.data[2][0] =  e * t[1] - f * t[3] + h * t[5];
	res.data[3][0] =-(e * t[2] - f * t[4] + g * t[5]);

	res.data[0][1] =-(b * t[0] - c * t[1] + d * t[2]);
	res.data[1][1] =  a * t[0] - c * t[3] + d * t[4];
	res.data[2][1] =-(a * t[1] - b * t[3] + d * t[5]);
	res.data[3][1] =  a * t[2] - b * t[4] + c * t[5];

	t[0] = g * p - o * h; t[1] = f * p - n * h; t[2] = f * o - n * g;
	t[3] = e * p - m * h; t[4] = e * o - m * g; t[5] = e * n - m * f;

	res.data[0][2] =  b * t[0] - c * t[1] + d * t[2];
	res.data[1][2] =-(a * t[0] - c * t[3] + d * t[4]);
	res.data[2][2] =  a * t[1] - b * t[3] + d * t[5];
	res.data[3][2] =-(a * t[2] - b * t[4] + c * t[5]);

	t[0] = g * l - k * h; t[1] = f * l - j * h; t[2] = f * k - j * g;
	t[3] = e * l - i * h; t[4] = e * k - i * g; t[5] = e * j - i * f;

	res.data[0][3] =-(b * t[0] - c * t[1] + d * t[2]);
	res.data[1][3] =  a * t[0] - c * t[3] + d * t[4];
	res.data[2][3] =-(a * t[1] - b * t[3] + d * t[5]);
	res.data[3][3] =  a * t[2] - b * t[4] + c * t[5];

	det = 1.0f / (a * res.data[0][0] + b * res.data[1][0] + c * res.data[2][0] + d * res.data[3][0]);

	res.data[0][0] *= det; res.data[0][1] *= det; res.data[0][2] *= det; res.data[0][3] *= det;
	res.data[1][0] *= det; res.data[1][1] *= det; res.data[1][2] *= det; res.data[1][3] *= det;
	res.data[2][0] *= det; res.data[2][1] *= det; res.data[2][2] *= det; res.data[2][3] *= det;
	res.data[3][0] *= det; res.data[3][1] *= det; res.data[3][2] *= det; res.data[3][3] *= det;

	return res;
}

