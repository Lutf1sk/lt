#ifndef LT_C_H
#define LT_C_H 1

#include <lt/fwd.h>
#include <lt/err.h>

// preprocess.c

#define LT_CDEF_VARIADIC_SEPARATE	0x01
#define LT_CDEF_VARIADIC_LAST		0x02
#define LT_CDEF_VARIADIC			0x03

typedef
struct lt_c_define {
	u8 flags;
	lstr_t name;
	lstr_t value;
	lt_darr(lstr_t) params;
} lt_c_define_t;

typedef
struct lt_c_preproc_template {
	char* it;
	char* end;
	struct lt_c_preproc_template* prev;
} lt_c_preproc_template_t;

typedef
struct lt_c_preproc_scope {
	b8 condition;
	b8 dead;
} lt_c_preproc_scope_t;

typedef
struct lt_c_preproc_ctx {
	lt_arena_t* arena;

	lt_darr(lt_c_define_t) defines;
	lstr_t* include_dirs;
	usz include_dir_count;

	lt_c_preproc_template_t* template;

	lt_darr(lt_c_preproc_scope_t) scopes;

	lt_darr(lstr_t) disallowed;

	u32 err_line;
	lstr_t err_str;
} lt_c_preproc_ctx_t;

lt_err_t lt_c_preproc_create(lt_c_preproc_ctx_t* cx, lstr_t* include_dirs, usz include_dir_count, lt_arena_t* arena);

lt_err_t lt_c_define(lt_c_preproc_ctx_t* cx, lstr_t name, lstr_t value, lt_darr(lstr_t) params, u8 flags);
lt_err_t lt_c_undefine(lt_c_preproc_ctx_t* cx, lstr_t name);

lt_c_define_t* lt_c_lookup_define(lt_c_preproc_ctx_t* cx, lstr_t name);

lt_err_t lt_c_preproc_file(lt_c_preproc_ctx_t* cx, void* data, usz size, lstr_t file_path, lstr_t* out);

#endif