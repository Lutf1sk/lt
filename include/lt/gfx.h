#ifndef LT_GFX_H
#define LT_GFX_H 1

#include <lt/err.h>
#include <lt/fwd.h>

// pipeline

typedef
struct lt_pipeline {
	u32 gl_prog;
} lt_pipeline_t;

#define LT_SHADER_FMT_GLSL 1
#define LT_SHADER_FMT_SPIRV 2

lt_err_t lt_pipeline_create(lt_gfx_t* gfx, lt_pipeline_t* pl, int source_fmt, lstr_t vert_source, lstr_t frag_source);
void lt_pipeline_destroy(lt_pipeline_t* pl);

void lt_pipeline_uniform_u32(lt_pipeline_t* pl, u32 location, u32 v);
void lt_pipeline_uniform_f32(lt_pipeline_t* pl, u32 location, f32 v);

void lt_pipeline_uniform_mat2(lt_pipeline_t* pl, u32 location, float* v);
void lt_pipeline_uniform_mat3(lt_pipeline_t* pl, u32 location, float* v);
void lt_pipeline_uniform_mat4(lt_pipeline_t* pl, u32 location, float* v);

void lt_pipeline_uniform_vec2(lt_pipeline_t* pl, u32 location, float* v);
void lt_pipeline_uniform_vec3(lt_pipeline_t* pl, u32 location, float* v);
void lt_pipeline_uniform_vec4(lt_pipeline_t* pl, u32 location, float* v);

// mesh

typedef
struct lt_model {
	usz index_count;
	u32* indices;

	usz vertex_count;
	float* uvs;
	float* positions;
	float* normals;
} lt_model_t;

typedef
struct lt_mesh {
	u32 gl_vao;
	u32 gl_idxbuf, gl_posbuf, gl_uvbuf, gl_normbuf;
	u32 index_count;
} lt_mesh_t;

lt_err_t lt_mesh_create(lt_gfx_t* gfx, lt_mesh_t* mesh, lt_model_t* model);
void lt_mesh_destroy(lt_mesh_t* mesh);

lt_err_t lt_mesh_upload(lt_gfx_t* gfx, lt_mesh_t* mesh, lt_model_t* model);

// texture

#define LT_TEXTURE_FILTERED 1

typedef
struct lt_texture {
	u32 gl_tex;
} lt_texture_t;

lt_err_t lt_texture_create(lt_gfx_t* gfx, lt_texture_t* tex, usz flags, lt_img_t* img);
void lt_texture_destroy(lt_texture_t* tex);

lt_err_t lt_texture_upload(lt_gfx_t* gfx, lt_texture_t* tex, lt_img_t* img);

// gfx

typedef
struct lt_gfx {
	lt_window_t* window;
	lt_alloc_t* alloc;
	lt_pipeline_t default_pipeline;
	lt_mesh_t rect_mesh;
	lt_texture_t white_texture;
} lt_gfx_t;

lt_err_t lt_gfx_init(void);

lt_err_t lt_gfx_create(lt_gfx_t* gfx, lt_window_t* win, lt_alloc_t* alloc);
void lt_gfx_destroy(lt_gfx_t* gfx);

lt_err_t lt_gfx_begin(lt_gfx_t* gfx);
lt_err_t lt_gfx_end(lt_gfx_t* gfx);

void lt_gfx_bind_pipeline(lt_gfx_t* gfx, lt_pipeline_t* pl);
void lt_gfx_bind_texture(lt_gfx_t* gfx, lt_texture_t* tex);
void lt_gfx_set_scissor(lt_gfx_t* gfx, isz x, isz y, isz w, isz h);

void lt_gfx_draw_mesh(lt_gfx_t* gfx, lt_mesh_t* mesh);

#define LT_DEFAULT_DEPTH 0
#define LT_MAX_DEPTH 65535

void lt_gfx_draw_rectctd(lt_gfx_t* gfx, isz x, isz y, isz w, isz h, u32 color, lt_texture_t* tex, u32 depth);
void lt_gfx_draw_rectcd(lt_gfx_t* gfx, isz x, isz y, isz w, isz h, u32 color, u32 depth);
#define lt_gfx_draw_recttd(gfx, x, y, w, h, tex, depth) lt_gfx_draw_rectctd((gfx), (x), (y), (w), (h), 0xFFFFFFFF, (tex), (depth))

#define lt_gfx_draw_rectct(gfx, x, y, w, h, clr, tex) lt_gfx_draw_rectctd((gfx), (x), (y), (w), (h), (clr), (tex), LT_DEFAULT_DEPTH)
#define lt_gfx_draw_rectc(gfx, x, y, w, h, clr) lt_gfx_draw_rectcd((gfx), (x), (y), (w), (h), (clr), LT_DEFAULT_DEPTH)
#define lt_gfx_draw_rectt(gfx, x, y, w, h, tex) lt_gfx_draw_rectctd((gfx), (x), (y), (w), (h), 0xFFFFFFFF, (tex), LT_DEFAULT_DEPTH)

lt_err_t lt_gfx_render_text(lt_gfx_t* gfx, lstr_t text, lt_font_t* font, usz flags, lt_texture_t* out_tex);

void lt_gfx_wait_idle(lt_gfx_t* gfx);

#endif
