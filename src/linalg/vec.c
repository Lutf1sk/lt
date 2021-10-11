#include <lt/linalg.h>

// ----- vec2

void lt_vec2_add(lt_vec2_t* dst, lt_vec2_t* src) {
	dst->x += src->x;
	dst->y += src->y;
}

void lt_vec2_sub(lt_vec2_t* dst, lt_vec2_t* src) {
	dst->x -= src->x;
	dst->y -= src->y;
}

void lt_vec2_div(lt_vec2_t* dst, lt_vec2_t* src) {
	dst->x /= src->x;
	dst->y /= src->y;
}

void lt_vec2_mul(lt_vec2_t* dst, lt_vec2_t* src) {
	dst->x *= src->x;
	dst->y *= src->y;
}

void lt_vec2_add_f(lt_vec2_t* v, float f) {
	v->x += f, v->y += f;
}

void lt_vec2_sub_f(lt_vec2_t* v, float f) {
	v->x -= f, v->y -= f;
}

void lt_vec2_div_f(lt_vec2_t* v, float f) {
	v->x /= f, v->y /= f;
}

void lt_vec2_mul_f(lt_vec2_t* v, float f) {
	v->x *= f, v->y *= f;
}

void lt_vec2_mul_mat2(lt_vec2_t* v, lt_mat2_t* m) {
	lt_vec2_t i = m->i, j = m->j;

	lt_vec2_mul_f(&i, v->x);
	lt_vec2_mul_f(&j, v->y);

	*v = i;
	lt_vec2_add(v, &j);
}

// ----- vec3

void lt_vec3_add(lt_vec3_t* dst, lt_vec3_t* src) {
	dst->x += src->x;
	dst->y += src->y;
	dst->z += src->z;
}

void lt_vec3_sub(lt_vec3_t* dst, lt_vec3_t* src) {
	dst->x -= src->x;
	dst->y -= src->y;
	dst->z -= src->z;
}

void lt_vec3_div(lt_vec3_t* dst, lt_vec3_t* src) {
	dst->x /= src->x;
	dst->y /= src->y;
	dst->z /= src->z;
}

void lt_vec3_mul(lt_vec3_t* dst, lt_vec3_t* src) {
	dst->x *= src->x;
	dst->y *= src->y;
	dst->z *= src->z;
}

void lt_vec3_add_f(lt_vec3_t* v, float f) {
	v->x += f, v->y += f, v->z += f;
}

void lt_vec3_sub_f(lt_vec3_t* v, float f) {
	v->x -= f, v->y -= f, v->z -= f;
}

void lt_vec3_div_f(lt_vec3_t* v, float f) {
	v->x /= f, v->y /= f, v->z /= f;
}

void lt_vec3_mul_f(lt_vec3_t* v, float f) {
	v->x *= f, v->y *= f, v->z *= f;
}

void lt_vec3_mul_mat3(lt_vec3_t* v, lt_mat3_t* m) {
	lt_vec3_t i = m->i, j = m->j, k = m->k;

	lt_vec3_mul_f(&i, v->x);
	lt_vec3_mul_f(&j, v->y);
	lt_vec3_mul_f(&k, v->z);

	*v = i;
	lt_vec3_add(v, &j);
	lt_vec3_add(v, &k);
}

// ----- vec4

void lt_vec4_add(lt_vec4_t* dst, lt_vec4_t* src) {
	dst->x += src->x;
	dst->y += src->y;
	dst->z += src->z;
	dst->w += src->w;
}

void lt_vec4_sub(lt_vec4_t* dst, lt_vec4_t* src) {
	dst->x -= src->x;
	dst->y -= src->y;
	dst->z -= src->z;
	dst->w -= src->w;
}

void lt_vec4_div(lt_vec4_t* dst, lt_vec4_t* src) {
	dst->x /= src->x;
	dst->y /= src->y;
	dst->z /= src->z;
	dst->w /= src->w;
}

void lt_vec4_mul(lt_vec4_t* dst, lt_vec4_t* src) {
	dst->x *= src->x;
	dst->y *= src->y;
	dst->z *= src->z;
	dst->w *= src->w;
}

void lt_vec4_add_f(lt_vec4_t* v, float f) {
	v->x += f, v->y += f, v->z += f, v->w += f;
}

void lt_vec4_sub_f(lt_vec4_t* v, float f) {
	v->x -= f, v->y -= f, v->z -= f, v->w -= f;
}

void lt_vec4_div_f(lt_vec4_t* v, float f) {
	v->x /= f, v->y /= f, v->z /= f, v->w /= f;
}

void lt_vec4_mul_f(lt_vec4_t* v, float f) {
	v->x *= f, v->y *= f, v->z *= f, v->w *= f;
}

void lt_vec4_mul_mat4(lt_vec4_t* v, lt_mat4_t* m) {
	lt_vec4_t i = m->i, j = m->j, k = m->k, l = m->l;

	lt_vec4_mul_f(&i, v->x);
	lt_vec4_mul_f(&j, v->y);
	lt_vec4_mul_f(&k, v->z);
	lt_vec4_mul_f(&l, v->w);

	*v = i;
	lt_vec4_add(v, &j);
	lt_vec4_add(v, &k);
	lt_vec4_add(v, &l);
}

