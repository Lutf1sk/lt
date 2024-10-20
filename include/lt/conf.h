#ifndef LT_CONF_H
#define LT_CONF_H 1

#include <lt/fwd.h>
#include <lt/err.h>

#define LT_FOR_EACH_CONF() \
	LT_CONF_OP(BOOL) \
	LT_CONF_OP(INT) \
	LT_CONF_OP(FLOAT) \
	LT_CONF_OP(STRING) \
	LT_CONF_OP(OBJECT) \
	LT_CONF_OP(ARRAY)

typedef
enum lt_conf_stype {
#define LT_CONF_OP(x) LT_CONF_##x,
	LT_FOR_EACH_CONF()
#undef LT_CONF_OP
} lt_conf_stype_t;

typedef
struct lt_conf {
	lt_conf_stype_t stype;
	lstr_t key;

	union {
		i64 int_val;
		u64 uint_val;
		f64 float_val;
		b8 bool_val;
		lstr_t str_val;

		struct {
			lt_conf_t* children;
			usz child_count;
		};
	};
} lt_conf_t;

typedef
struct lt_conf_err_info {
	lt_err_t err;
	lstr_t err_str;
} lt_conf_err_info_t;

#define LT_CONF_ERR_INFO_INIT(e, s) { (e), (s) }
#define LT_CONF_ERR_INFO(e, s) ((lt_conf_err_info_t)LT_CONF_ERR_INFO_INIT((e), (s)))

#define LT_CONF_INIT(stype, key, ...) { (stype), (key), __VA_ARGS__ }
#define LT_CONF(stype, key, ...) ((lt_conf_t)LT_CONF_INIT(stype, key, __VA_ARGS__))

lt_err_t lt_conf_add_child(lt_conf_t cf[static 1], lt_conf_t child[static 1]);

lt_err_t lt_conf_erase_str(lt_conf_t cf[static 1], lstr_t str, lt_alloc_t alloc[static 1]);
lt_err_t lt_conf_erase_index(lt_conf_t cf[static 1], usz index, lt_alloc_t alloc[static 1]);
lt_err_t lt_conf_erase_child(lt_conf_t cf[static 1], lstr_t key, lt_alloc_t alloc[static 1]);

lt_err_t lt_conf_parse(lt_conf_t cf[static 1], void* data, usz len, lt_conf_err_info_t* err_str, lt_alloc_t alloc[static 1]);

void lt_conf_free_err_info(lt_conf_err_info_t err_info[static 1], lt_alloc_t alloc[static 1]);

lt_conf_t* lt_conf_find(lt_conf_t parent[static 1], lstr_t key_path);

lt_conf_t* lt_conf_find_int(lt_conf_t parent[static 1], lstr_t key_path, i64* out);
lt_conf_t* lt_conf_find_uint(lt_conf_t parent[static 1], lstr_t key_path, u64* out);
lt_conf_t* lt_conf_find_bool(lt_conf_t parent[static 1], lstr_t key_path, b8* out);
lt_conf_t* lt_conf_find_str(lt_conf_t parent[static 1], lstr_t key_path, lstr_t* out);
lt_conf_t* lt_conf_find_float(lt_conf_t parent[static 1], lstr_t key_path, f64* out);
lt_conf_t* lt_conf_find_object(lt_conf_t parent[static 1], lstr_t key_path, lt_conf_t** out);
lt_conf_t* lt_conf_find_array(lt_conf_t parent[static 1], lstr_t key_path, lt_conf_t** out);

i64 lt_conf_int(lt_conf_t parent[static 1], lstr_t key_path);
u64 lt_conf_uint(lt_conf_t parent[static 1], lstr_t key_path);
b8 lt_conf_bool(lt_conf_t parent[static 1], lstr_t key_path);
lstr_t lt_conf_str(lt_conf_t parent[static 1], lstr_t key_path);
f64 lt_conf_float(lt_conf_t parent[static 1], lstr_t key_path);
lt_conf_t* lt_conf_object(lt_conf_t parent[static 1], lstr_t key_path);
lt_conf_t* lt_conf_array(lt_conf_t parent[static 1], lstr_t key_path);

i64 lt_conf_find_int_default(lt_conf_t parent[static 1], lstr_t key_path, i64 default_);
u64 lt_conf_find_uint_default(lt_conf_t parent[static 1], lstr_t key_path, u64 default_);
b8 lt_conf_find_bool_default(lt_conf_t parent[static 1], lstr_t key_path, b8 default_);
lstr_t lt_conf_find_str_default(lt_conf_t parent[static 1], lstr_t key_path, lstr_t default_);
f64 lt_conf_find_float_default(lt_conf_t parent[static 1], lstr_t key_path, f64 default_);

isz lt_conf_write(lt_conf_t cf[static 1], lt_write_fn_t callb, void* usr);

void lt_conf_free(lt_conf_t cf[static 1], lt_alloc_t alloc[static 1]);

#endif
