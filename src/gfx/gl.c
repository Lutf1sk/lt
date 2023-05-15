#include <lt/gfx.h>
#include <lt/window.h>
#include <lt/gl.h>
#include <lt/mem.h>
#include <lt/img.h>
#include <lt/font.h>
#include <lt/utf8.h>

#define LT_LINALG_SHORTEN_NAMES
#include <lt/linalg.h>

#include <lt/internal.h>

// gfx

static lstr_t vert_src = CLSTR(
	"#version 440\n"
	"layout(location = 0) in vec3 in_pos;"
	"layout(location = 1) in vec2 in_uv;"
	"layout(location = 2) in vec3 in_norm;"
	""
	"layout(location = 0) uniform mat4 proj;"
	"layout(location = 1) uniform mat4 model;"
	""
	"layout(location = 0) out vec2 out_uv;"
	"layout(location = 1) out vec3 out_norm;"
	""
	"void main() {"
	"	gl_Position = proj * model * vec4(in_pos, 1.0f);"
	"	out_uv = in_uv;"
	"	out_norm = in_norm;"
	"}"
);

static lstr_t frag_src = CLSTR(
	"#version 440\n"
	"layout(location = 0) in vec2 in_uv;"
	"layout(location = 0) out vec4 out_color;"
	"layout(location = 2) uniform sampler2D tex;"
	"layout(location = 3) uniform vec4 color;"
	"void main() {"
	"	out_color = color * texture(tex, in_uv);"
	"}"
);

#define DEFAULT_LOCATION_PROJ 0
#define DEFAULT_LOCATION_MODEL 1
#define DEFAULT_LOCATION_TEX 2
#define DEFAULT_LOCATION_COLOR 3

lt_err_t lt_gfx_init(void) {
	if (!lt_gl_initialize_loader())
		return LT_ERR_UNKNOWN; // !!
	return LT_SUCCESS;
}

lt_err_t lt_gfx_create(lt_gfx_t* gfx, lt_window_t* win, lt_alloc_t* alloc) {
	lt_err_t err;

	gfx->window = win;
	gfx->alloc = alloc;

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glEnable(GL_SCISSOR_TEST);

	lt_img_t white_img;
	white_img.format = LT_IMG_RGBA;
	white_img.flags = 0;
	white_img.width = 1;
	white_img.height = 1;
	white_img.data = (u32[]) { 0xFFFFFFFF };

	if ((err = lt_texture_create(gfx, &gfx->white_texture, 0, &white_img)))
		goto err0;

	lt_model_t rect_model;
	rect_model.vertex_count = 4;
	rect_model.positions = (float[]) {
		0.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		1.0f, 1.0f, 0.0f,
	};
	rect_model.uvs = (float[]) {
		0.0f, 0.0f,
		0.0f, 1.0f,
		1.0f, 0.0f,
		1.0f, 1.0f,
	};
	rect_model.index_count = 6;
	rect_model.indices = (u32[]) { 0, 1, 2, 3, 2, 1 };

	if ((err = lt_mesh_create(gfx, &gfx->rect_mesh, &rect_model)))
		goto err1;

	if ((err = lt_pipeline_create(gfx, &gfx->default_pipeline, LT_SHADER_FMT_GLSL, vert_src, frag_src)))
		goto err2;

	lt_pipeline_uniform_u32(&gfx->default_pipeline, DEFAULT_LOCATION_TEX, 0);
	return LT_SUCCESS;

err2:	lt_mesh_destroy(&gfx->rect_mesh);
err1:	lt_texture_destroy(&gfx->white_texture);
err0:	return err;
}

void lt_gfx_destroy(lt_gfx_t* gfx) {
	lt_texture_destroy(&gfx->white_texture);
	lt_mesh_destroy(&gfx->rect_mesh);
	lt_pipeline_destroy(&gfx->default_pipeline);
}

lt_err_t lt_gfx_begin(lt_gfx_t* gfx) {
	int w, h;
	lt_window_get_size(gfx->window, &w, &h);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glViewport(0, 0, w, h);
	glScissor(0, 0, w, h);

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	mat4 proj = lt_m4ortho(0.0f, w, h, 0.0f, 0.0f, 1.0f);
	lt_pipeline_uniform_mat4(&gfx->default_pipeline, DEFAULT_LOCATION_PROJ, (float*)&proj);

	return LT_SUCCESS;
}

lt_err_t lt_gfx_end(lt_gfx_t* gfx) {
	lt_window_gl_swap_buffers(gfx->window);

	return LT_SUCCESS;
}

void lt_gfx_bind_pipeline(lt_gfx_t* gfx, lt_pipeline_t* pl) {
	glUseProgram(pl->gl_prog);
}

void lt_gfx_bind_texture(lt_gfx_t* gfx, lt_texture_t* tex) {
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, tex->gl_tex);
}

void lt_gfx_set_scissor(lt_gfx_t* gfx, isz x, isz y, isz w, isz h) {
	// Flip the Y axis, since OpenGL's origin is in the bottom left
	int winw, winh;
	lt_window_get_size(gfx->window, &winw, &winh);
	glScissor(x, winh - (y + h), w, h);
}

void lt_gfx_draw_mesh(lt_gfx_t* gfx, lt_mesh_t* mesh) {
	glBindVertexArray(mesh->gl_vao);
	glDrawElements(GL_TRIANGLES, mesh->index_count, GL_UNSIGNED_INT, NULL);
}

void lt_gfx_draw_rectctd(lt_gfx_t* gfx, isz x, isz y, isz w, isz h, u32 color, lt_texture_t* tex, u32 depth) {
	lt_gfx_bind_pipeline(gfx, &gfx->default_pipeline);
	lt_gfx_bind_texture(gfx, tex);

	float fcolor[4] = {
		(float)(color >> 24) / (float)0xFF,
		(float)((color >> 16) & 0xFF) / (float)0xFF,
		(float)((color >> 8) & 0xFF) / (float)0xFF,
		(float)(color & 0xFF) / (float)0xFF,
	};
	lt_pipeline_uniform_vec4(&gfx->default_pipeline, DEFAULT_LOCATION_COLOR, fcolor);

	mat4 model = LT_MAT4(
		w, 0, 0, 0,
		0, h, 0, 0,
		0, 0, 1, 0,
		x, y, (float)depth / (float)LT_MAX_DEPTH, 1
	);
	lt_pipeline_uniform_mat4(&gfx->default_pipeline, DEFAULT_LOCATION_MODEL, (float*)&model);

	lt_gfx_draw_mesh(gfx, &gfx->rect_mesh);
}

void lt_gfx_draw_rectcd(lt_gfx_t* gfx, isz x, isz y, isz w, isz h, u32 color, u32 depth) {
	lt_gfx_draw_rectctd(gfx, x, y, w, h, color, &gfx->white_texture, depth);
}

lt_err_t lt_gfx_render_text(lt_gfx_t* gfx, lstr_t text, lt_font_t* font, usz flags, lt_texture_t* out_tex) {
	lt_img_t img;
	img.format = LT_IMG_RGBA;
	img.flags = 0;
	img.width = font->width * lt_utf8_glyph_count(text);
	img.height = font->height;
	img.data = lt_malloc(gfx->alloc, img.width * img.height * sizeof(u32));
	if (!img.data)
		return LT_ERR_OUT_OF_MEMORY;

	lt_font_render(font, text, img.data);
	lt_err_t err = lt_texture_create(gfx, out_tex, flags, &img);
	lt_mfree(gfx->alloc, img.data);
	return err;
}

void lt_gfx_wait_idle(lt_gfx_t* gfx) {
	glFinish();
}

// pipeline

static
b8 check_shader_compile_error(int shader, lt_alloc_t* alloc) {
	int compiled = 0;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
	if (compiled == GL_TRUE)
		return 1;

	int log_len = 0;
	glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &log_len);
	char* log = lt_malloc(alloc, log_len);
	if (!log)
		return 0;
	glGetShaderInfoLog(shader, log_len, &log_len, log);
	lt_werrf("shader compilation failed: '%s'\n", log);
	lt_mfree(alloc, log);
	return 0;
}

static
b8 check_program_link_error(int prog, lt_alloc_t* alloc) {
	int compiled = 0;
	glGetProgramiv(prog, GL_LINK_STATUS, &compiled);
	if (compiled == GL_TRUE)
		return 1;

	int log_len = 0;
	glGetProgramiv(prog, GL_INFO_LOG_LENGTH, &log_len);
	char* log = lt_malloc(alloc, log_len);
	if (!log)
		return 0;
	glGetProgramInfoLog(prog, log_len, &log_len, log);
	lt_werrf("failed to link shader program: '%s'\n", log);
	lt_mfree(alloc, log);
	return 0;
}

lt_err_t lt_pipeline_create(lt_gfx_t* gfx, lt_pipeline_t* pl, int source_fmt, lstr_t vert_source, lstr_t frag_source) {
	lt_err_t err;

	if (source_fmt != LT_SHADER_FMT_GLSL)
		fail_to(err = LT_ERR_UNSUPPORTED, err0);

	int vert_shader = glCreateShader(GL_VERTEX_SHADER);
	int len = vert_source.len;
	glShaderSource(vert_shader, 1, (const char* const*)&vert_source.str, &len);
	glCompileShader(vert_shader);
	if (!check_shader_compile_error(vert_shader, gfx->alloc))
		fail_to(err = LT_ERR_INVALID_SYNTAX, err1);

	int frag_shader = glCreateShader(GL_FRAGMENT_SHADER);
	len = frag_source.len;
	glShaderSource(frag_shader, 1, (const char* const*)&frag_source.str, &len);
	glCompileShader(frag_shader);
	if (!check_shader_compile_error(frag_shader, gfx->alloc))
		fail_to(err = LT_ERR_INVALID_SYNTAX, err2);

	pl->gl_prog = glCreateProgram();
	glAttachShader(pl->gl_prog, vert_shader);
	glAttachShader(pl->gl_prog, frag_shader);

	glLinkProgram(pl->gl_prog);
	if (!check_program_link_error(pl->gl_prog, gfx->alloc))
		fail_to(err = LT_ERR_INVALID_SYNTAX, err3);

	glDeleteShader(vert_shader);
	glDeleteShader(frag_shader);
	return LT_SUCCESS;

err3:	glDeleteProgram(pl->gl_prog);
err2:	glDeleteShader(frag_shader);
err1:	glDeleteShader(vert_shader);
err0:	return err;
}

void lt_pipeline_destroy(lt_pipeline_t* pl) {
	glDeleteProgram(pl->gl_prog);
}

void lt_pipeline_uniform_u32(lt_pipeline_t* pl, u32 location, u32 v) {
	glUseProgram(pl->gl_prog);
	glUniform1i(location, v);
}

void lt_pipeline_uniform_f32(lt_pipeline_t* pl, u32 location, f32 v) {
	glUseProgram(pl->gl_prog);
	glUniform1f(location, v);
}

void lt_pipeline_uniform_mat2(lt_pipeline_t* pl, u32 location, float* v) {
	glUseProgram(pl->gl_prog);
	glUniformMatrix2fv(location, 1, GL_FALSE, v);
}

void lt_pipeline_uniform_mat3(lt_pipeline_t* pl, u32 location, float* v) {
	glUseProgram(pl->gl_prog);
	glUniformMatrix3fv(location, 1, GL_FALSE, v);
}

void lt_pipeline_uniform_mat4(lt_pipeline_t* pl, u32 location, float* v) {
	glUseProgram(pl->gl_prog);
	glUniformMatrix4fv(location, 1, GL_FALSE, v);
}

void lt_pipeline_uniform_vec2(lt_pipeline_t* pl, u32 location, float* v) {
	glUseProgram(pl->gl_prog);
	glUniform2fv(location, 1, v);
}

void lt_pipeline_uniform_vec3(lt_pipeline_t* pl, u32 location, float* v) {
	glUseProgram(pl->gl_prog);
	glUniform3fv(location, 1, v);
}

void lt_pipeline_uniform_vec4(lt_pipeline_t* pl, u32 location, float* v) {
	glUseProgram(pl->gl_prog);
	glUniform4fv(location, 1, v);
}

// mesh

lt_err_t lt_mesh_create(lt_gfx_t* gfx, lt_mesh_t* mesh, lt_model_t* model) {
	glGenVertexArrays(1, &mesh->gl_vao);
	glBindVertexArray(mesh->gl_vao);

	glGenBuffers(1, &mesh->gl_idxbuf);
	glGenBuffers(1, &mesh->gl_posbuf);
	glGenBuffers(1, &mesh->gl_uvbuf);
	glGenBuffers(1, &mesh->gl_normbuf);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->gl_idxbuf);

	glBindBuffer(GL_ARRAY_BUFFER, mesh->gl_posbuf);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), NULL);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, mesh->gl_uvbuf);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), NULL);
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, mesh->gl_normbuf);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), NULL);
	glEnableVertexAttribArray(2);

	lt_err_t err;
	if (model && (err = lt_mesh_upload(gfx, mesh, model))) {
		lt_mesh_destroy(mesh);
		return err;
	}
	return LT_SUCCESS;
}

void lt_mesh_destroy(lt_mesh_t* mesh) {
	glDeleteBuffers(1, &mesh->gl_idxbuf);
	glDeleteBuffers(1, &mesh->gl_posbuf);
	glDeleteBuffers(1, &mesh->gl_uvbuf);
	glDeleteBuffers(1, &mesh->gl_normbuf);
	glDeleteVertexArrays(1, &mesh->gl_vao);
}

lt_err_t lt_mesh_upload(lt_gfx_t* gfx, lt_mesh_t* mesh, lt_model_t* model) {
	glBindVertexArray(mesh->gl_vao);

	if (model->indices) {
		mesh->index_count = model->index_count;
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->gl_idxbuf);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, model->index_count * sizeof(u32), model->indices, GL_STATIC_DRAW);
	}
	if (model->positions) {
		glBindBuffer(GL_ARRAY_BUFFER, mesh->gl_posbuf);
		glBufferData(GL_ARRAY_BUFFER, model->vertex_count * sizeof(float) * 3, model->positions, GL_STATIC_DRAW);
	}
	if (model->uvs) {
		glBindBuffer(GL_ARRAY_BUFFER, mesh->gl_uvbuf);
		glBufferData(GL_ARRAY_BUFFER, model->vertex_count * sizeof(float) * 2, model->uvs, GL_STATIC_DRAW);
	}
	if (model->normals) {
		glBindBuffer(GL_ARRAY_BUFFER, mesh->gl_normbuf);
		glBufferData(GL_ARRAY_BUFFER, model->vertex_count * sizeof(float) * 3, model->normals, GL_STATIC_DRAW);
	}

	return LT_SUCCESS;
}

// texture

lt_err_t lt_texture_create(lt_gfx_t* gfx, lt_texture_t* tex, usz flags, lt_img_t* img) {
	glActiveTexture(GL_TEXTURE0);
	glGenTextures(1, &tex->gl_tex);

	glBindTexture(GL_TEXTURE_2D, tex->gl_tex);
	if (flags & LT_TEXTURE_FILTERED) {
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	}
	else {
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	}

	lt_err_t err;
	if (img && (err = lt_texture_upload(gfx, tex, img))) {
		glDeleteTextures(1, &tex->gl_tex);
		return err;
	}
	return LT_SUCCESS;
}

void lt_texture_destroy(lt_texture_t* tex) {
	glDeleteTextures(1, &tex->gl_tex);
}

lt_err_t lt_texture_upload(lt_gfx_t* gfx, lt_texture_t* tex, lt_img_t* img) {
	if (img->format != LT_IMG_RGBA)
		return LT_ERR_UNSUPPORTED;
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, img->width, img->height, 0, GL_RGBA, GL_UNSIGNED_BYTE, img->data);
	glGenerateMipmap(GL_TEXTURE_2D);
	return LT_SUCCESS;
}

