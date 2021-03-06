#include <lt/linalg.h>
#include <lt/mem.h>

#include <math.h>

// ----- vec2

void lt_vec2_add(lt_vec2_t a, lt_vec2_t b, lt_vec2_t dst) {
	dst[0] = a[0] + b[0];
	dst[1] = a[1] + b[1];
}

void lt_vec2_sub(lt_vec2_t a, lt_vec2_t b, lt_vec2_t dst) {
	dst[0] = a[0] - b[0];
	dst[1] = a[1] - b[1];
}

void lt_vec2_div(lt_vec2_t a, lt_vec2_t b, lt_vec2_t dst) {
	dst[0] = a[0] / b[0];
	dst[1] = a[1] / b[1];
}

void lt_vec2_mul(lt_vec2_t a, lt_vec2_t b, lt_vec2_t dst) {
	dst[0] = a[0] * b[0];
	dst[1] = a[1] * b[1];
}

void lt_vec2_add_f(lt_vec2_t v, float f, lt_vec2_t dst) {
	dst[0] = v[0] + f;
	dst[1] = v[1] + f;
}

void lt_vec2_sub_f(lt_vec2_t v, float f, lt_vec2_t dst) {
	dst[0] = v[0] - f;
	dst[1] = v[1] - f;
}

void lt_vec2_div_f(lt_vec2_t v, float f, lt_vec2_t dst) {
	dst[0] = v[0] / f;
	dst[1] = v[1] / f;
}

void lt_vec2_mul_f(lt_vec2_t v, float f, lt_vec2_t dst) {
	dst[0] = v[0] * f;
	dst[1] = v[1] * f;
}

void lt_vec2_mul_mat2(lt_vec2_t v, lt_mat2_t m) {
	lt_vec2_t res, tmp;
	lt_vec2_mul_f(m[0], v[0], res);

	lt_vec2_mul_f(m[1], v[1], tmp);
	lt_vec2_add(tmp, res, res);

	memcpy(v, res, sizeof(res));
}

f32 lt_vec2_magnitude(lt_vec2_t v) {
	return sqrt(v[0] * v[0] + v[1] * v[1]);
}

void lt_vec2_normalize(lt_vec2_t v) {
	f32 m = 1.0f / lt_vec2_magnitude(v);
	v[0] *= m, v[1] *= m;
}

// ----- vec3

// void lt_vec3_add(lt_vec3_t a, lt_vec3_t b, lt_vec3_t dst) {
// 	dst[0] = a[0] + b[0];
// 	dst[1] = a[1] + b[1];
// 	dst[2] = a[2] + b[2];
// }
// 
// void lt_vec3_sub(lt_vec3_t a, lt_vec3_t b, lt_vec3_t dst) {
// 	dst[0] = a[0] - b[0];
// 	dst[1] = a[1] - b[1];
// 	dst[2] = a[2] - b[2];
// }
// 
// void lt_vec3_div(lt_vec3_t a, lt_vec3_t b, lt_vec3_t dst) {
// 	dst[0] = a[0] / b[0];
// 	dst[1] = a[1] / b[1];
// 	dst[2] = a[2] / b[2];
// }
// 
// void lt_vec3_mul(lt_vec3_t a, lt_vec3_t b, lt_vec3_t dst) {
// 	dst[0] = a[0] * b[0];
// 	dst[1] = a[1] * b[1];
// 	dst[2] = a[2] * b[2];
// }
// 
// void lt_vec3_add_f(lt_vec3_t v, float f, lt_vec3_t dst) {
// 	dst[0] = v[0] + f;
// 	dst[1] = v[1] + f;
// 	dst[2] = v[2] + f;
// }
// 
// void lt_vec3_sub_f(lt_vec3_t v, float f, lt_vec3_t dst) {
// 	dst[0] = v[0] - f;
// 	dst[1] = v[1] - f;
// 	dst[2] = v[2] - f;
// }
// 
// void lt_vec3_div_f(lt_vec3_t v, float f, lt_vec3_t dst) {
// 	dst[0] = v[0] / f;
// 	dst[1] = v[1] / f;
// 	dst[2] = v[2] / f;
// }
// 
// void lt_vec3_mul_f(lt_vec3_t v, float f, lt_vec3_t dst) {
// 	dst[0] = v[0] * f;
// 	dst[1] = v[1] * f;
// 	dst[2] = v[2] * f;
// }

void lt_vec3_mul_mat3(lt_vec3_t v, lt_mat3_t m) {
	lt_vec3_t res, tmp;
	lt_vec3_mul_f(m[0], v[0], res);

	lt_vec3_mul_f(m[1], v[1], tmp);
	lt_vec3_add(tmp, res, res);

	lt_vec3_mul_f(m[2], v[2], tmp);
	lt_vec3_add(tmp, res, res);

	memcpy(v, res, sizeof(res));
}

// f32 lt_vec3_magnitude(lt_vec3_t v) {
// 	float h = sqrt(v[0] * v[0] + v[1] * v[1]);
// 	return sqrt(v[2] * v[2] + h * h);
// }
// 
// void lt_vec3_normalize(lt_vec3_t v) {
// 	f32 m = 1.0f / lt_vec3_magnitude(v);
// 	v[0] *= m, v[1] *= m, v[2] *= m;
// }

void lt_vec3_cross(lt_vec3_t a, lt_vec3_t b, lt_vec3_t dst) {
	dst[0] = a[1] * b[2] - a[2] * b[1];
	dst[1] = a[2] * b[0] - a[0] * b[2];
	dst[2] = a[0] * b[1] - a[1] * b[0];
}

f32 lt_vec3_dot(lt_vec3_t a, lt_vec3_t b) {
	return a[0] * b[0] + a[1] * b[1] + a[2] * b[2];
}

// ----- vec4

void lt_vec4_add(lt_vec4_t a, lt_vec4_t b, lt_vec4_t dst) {
	dst[0] = a[0] + b[0];
	dst[1] = a[1] + b[1];
	dst[2] = a[2] + b[2];
	dst[3] = a[3] + b[3];
}

void lt_vec4_sub(lt_vec4_t a, lt_vec4_t b, lt_vec4_t dst) {
	dst[0] = a[0] - b[0];
	dst[1] = a[1] - b[1];
	dst[2] = a[2] - b[2];
	dst[3] = a[3] - b[3];
}

void lt_vec4_div(lt_vec4_t a, lt_vec4_t b, lt_vec4_t dst) {
	dst[0] = a[0] / b[0];
	dst[1] = a[1] / b[1];
	dst[2] = a[2] / b[2];
	dst[3] = a[3] / b[3];
}

void lt_vec4_mul(lt_vec4_t a, lt_vec4_t b, lt_vec4_t dst) {
	dst[0] = a[0] * b[0];
	dst[1] = a[1] * b[1];
	dst[2] = a[2] * b[2];
	dst[3] = a[3] * b[3];
}

void lt_vec4_add_f(lt_vec4_t v, float f, lt_vec4_t dst) {
	dst[0] = v[0] + f;
	dst[1] = v[1] + f;
	dst[2] = v[2] + f;
	dst[3] = v[3] + f;
}

void lt_vec4_sub_f(lt_vec4_t v, float f, lt_vec4_t dst) {
	dst[0] = v[0] - f;
	dst[1] = v[1] - f;
	dst[2] = v[2] - f;
	dst[3] = v[3] - f;
}

void lt_vec4_div_f(lt_vec4_t v, float f, lt_vec4_t dst) {
	dst[0] = v[0] / f;
	dst[1] = v[1] / f;
	dst[2] = v[2] / f;
	dst[3] = v[3] / f;
}

void lt_vec4_mul_f(lt_vec4_t v, float f, lt_vec4_t dst) {
	dst[0] = v[0] * f;
	dst[1] = v[1] * f;
	dst[2] = v[2] * f;
	dst[3] = v[3] * f;
}

void lt_vec4_mul_mat4(lt_vec4_t v, lt_mat4_t m) {
	lt_vec4_t res, tmp;

	lt_vec4_mul_f(m[0], v[0], res);

	lt_vec4_mul_f(m[1], v[1], tmp);
	lt_vec4_add(tmp, res, res);

	lt_vec4_mul_f(m[2], v[2], tmp);
	lt_vec4_add(tmp, res, res);

	lt_vec4_mul_f(m[3], v[3], tmp);
	lt_vec4_add(tmp, res, res);

	memcpy(v, res, sizeof(res));
}

f32 lt_vec4_magnitude(lt_vec4_t v) {
	float h = sqrt(v[0] * v[0] + v[1] * v[1]);
	float h2 = sqrt(v[2] * v[2] + h * h);
	return sqrt(v[3] * v[3] + h2 * h2);
}

void lt_vec4_normalize(lt_vec4_t v) {
	f32 m = 1.0f / lt_vec4_magnitude(v);
	v[0] *= m, v[1] *= m, v[2] *= m, v[3] *= m;
}

