#include <lt/linalg.h>
#include <lt/mem.h>
#include <lt/lt.h>

#include <math.h>

static
lt_mat4_t lt_m4frustum(f32 bottom, f32 top, f32 left, f32 right, f32 near, f32 far) {
	lt_mat4_t m;
	m.data[0][0] = 2 * near / (right - left);
	m.data[0][1] = 0;
	m.data[0][2] = 0;
	m.data[0][3] = 0;

	m.data[1][0] = 0;
	m.data[1][1] = 2 * near / (top - bottom);
	m.data[1][2] = 0;
	m.data[1][3] = 0;

	m.data[2][0] = (right + left) / (right - left);
	m.data[2][1] = (top + bottom) / (top - bottom);
	m.data[2][2] = -(far + near) / (far - near);
	m.data[2][3] = -1;

	m.data[3][0] = 0;
	m.data[3][1] = 0;
	m.data[3][2] = -2 * far * near / (far - near);
	m.data[3][3] = 0;

	return m;
}

LT_FLATTEN
lt_mat4_t lt_m4perspective(f32 fov, f32 aspect, f32 near, f32 far) {
	f32 top = tanf(fov * 0.5f) * near;
	f32 bottom = -top;
	f32 right = top * aspect;
	f32 left = -right;
	return lt_m4frustum(bottom, top, left, right, near, far);
}

lt_mat4_t lt_m4ortho(f32 left, f32 right, f32 bottom, f32 top, f32 near, f32 far) {
	f32 rl = 1.0f / (right - left);
	f32 tb = 1.0f / (top - bottom);
	f32 fn = -1.0f / (far - near);

	lt_mat4_t mat;
	memset(&mat, 0, sizeof(lt_mat4_t));
	mat.data[0][0] = 2.0f * rl;
	mat.data[1][1] = 2.0f * tb;
	mat.data[2][2] = fn;
	mat.data[3][0] = -(right + left) * rl;
	mat.data[3][1] = -(top + bottom) * tb;
	mat.data[3][2] = near * fn;
	mat.data[3][3] = 1.0f;
	return mat;
}

lt_mat4_t lt_m4view(lt_vec3_t pos, lt_vec3_t fwd, lt_vec3_t up) {
	lt_vec3_t f, u, s;

	f = lt_vnorm(fwd);
	s = lt_vnorm(lt_vcross(f, up));
	u = lt_vcross(s, f);

	return LT_MAT4(
		s.x, u.x, -f.x, 0.0f,
		s.y, u.y, -f.y, 0.0f,
		s.z, u.z, -f.z, 0.0f,
		-lt_vdot(s, pos), -lt_vdot(u, pos), lt_vdot(f, pos), 1.0f
	);
}

lt_mat4_t lt_m4euler(lt_vec3_t angles) {
	float cx, cy, cz, sx, sy, sz, czsx, cxcz, sysz;

	sx = sinf(angles.x); cx = cosf(angles.x);
	sy = sinf(angles.y); cy = cosf(angles.y);
	sz = sinf(angles.z); cz = cosf(angles.z);

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

lt_mat4_t lt_m4inverse(const lt_mat4_t* m_) {
	lt_mat4_t d_;
	float (*dest)[4] = d_.data;

	const float (*mat)[4] = m_->data;

	float t[6];
	float det;
	float	a = mat[0][0], b = mat[0][1], c = mat[0][2], d = mat[0][3],
			e = mat[1][0], f = mat[1][1], g = mat[1][2], h = mat[1][3],
			i = mat[2][0], j = mat[2][1], k = mat[2][2], l = mat[2][3],
			m = mat[3][0], n = mat[3][1], o = mat[3][2], p = mat[3][3];

	t[0] = k * p - o * l; t[1] = j * p - n * l; t[2] = j * o - n * k;
	t[3] = i * p - m * l; t[4] = i * o - m * k; t[5] = i * n - m * j;

	dest[0][0] =  f * t[0] - g * t[1] + h * t[2];
	dest[1][0] =-(e * t[0] - g * t[3] + h * t[4]);
	dest[2][0] =  e * t[1] - f * t[3] + h * t[5];
	dest[3][0] =-(e * t[2] - f * t[4] + g * t[5]);

	dest[0][1] =-(b * t[0] - c * t[1] + d * t[2]);
	dest[1][1] =  a * t[0] - c * t[3] + d * t[4];
	dest[2][1] =-(a * t[1] - b * t[3] + d * t[5]);
	dest[3][1] =  a * t[2] - b * t[4] + c * t[5];

	t[0] = g * p - o * h; t[1] = f * p - n * h; t[2] = f * o - n * g;
	t[3] = e * p - m * h; t[4] = e * o - m * g; t[5] = e * n - m * f;

	dest[0][2] =  b * t[0] - c * t[1] + d * t[2];
	dest[1][2] =-(a * t[0] - c * t[3] + d * t[4]);
	dest[2][2] =  a * t[1] - b * t[3] + d * t[5];
	dest[3][2] =-(a * t[2] - b * t[4] + c * t[5]);

	t[0] = g * l - k * h; t[1] = f * l - j * h; t[2] = f * k - j * g;
	t[3] = e * l - i * h; t[4] = e * k - i * g; t[5] = e * j - i * f;

	dest[0][3] =-(b * t[0] - c * t[1] + d * t[2]);
	dest[1][3] =  a * t[0] - c * t[3] + d * t[4];
	dest[2][3] =-(a * t[1] - b * t[3] + d * t[5]);
	dest[3][3] =  a * t[2] - b * t[4] + c * t[5];

	det = 1.0f / (a * dest[0][0] + b * dest[1][0] + c * dest[2][0] + d * dest[3][0]);

	dest[0][0] *= det; dest[0][1] *= det; dest[0][2] *= det; dest[0][3] *= det;
	dest[1][0] *= det; dest[1][1] *= det; dest[1][2] *= det; dest[1][3] *= det;
	dest[2][0] *= det; dest[2][1] *= det; dest[2][2] *= det; dest[2][3] *= det;
	dest[3][0] *= det; dest[3][1] *= det; dest[3][2] *= det; dest[3][3] *= det;

	return d_;
}

lt_mat4_t lt_m4model(lt_vec3_t pos, lt_vec3_t rot, lt_vec3_t scale) {
	float cx, cy, cz, sx, sy, sz, czsx, cxcz, sysz;

	sx = sinf(rot.x); cx = cosf(rot.x);
	sy = sinf(rot.y); cy = cosf(rot.y);
	sz = sinf(rot.z); cz = cosf(rot.z);

	czsx = cz * sx;
	cxcz = cx * cz;
	sysz = sy * sz;

	lt_mat4_t mrot = LT_MAT4(
		cy * cz,	czsx * sy + cx * sz,	-cxcz * sy + sx * sz,	0.0f,
		-cy * sz,	cxcz - sx * sysz,		czsx + cx * sysz,		0.0f,
		sy,			-cy * sx,				cx * cy,				0.0f,
		pos.x,		pos.y,					pos.z,					1.0f
	);

	lt_mat4_t mscale = LT_MAT4(
		scale.x, 0, 0, 0,
		0, scale.y, 0, 0,
		0, 0, scale.z, 0,
		0, 0, 0, 1
	);

	return lt_m4mul(&mrot, &mscale);
}

