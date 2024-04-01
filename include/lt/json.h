#ifndef LT_JSON_H
#define LT_JSON_H 1

#include <lt/lt.h>
#include <lt/fwd.h>

// TODO: Rewrite JSON parser

typedef
enum lt_json_stype {
	LT_JSON_ARRAY,
	LT_JSON_OBJECT,
	LT_JSON_STRING,
	LT_JSON_NUMBER,
	LT_JSON_BOOL,
	LT_JSON_NULL,
} lt_json_stype_t;

typedef
struct lt_json {
	lt_json_stype_t stype;
	struct lt_json* next;

	union {
		lstr_t str_val;
		struct {
			struct lt_json* child;
			usz child_count;
		};
	};

	lstr_t key;
} lt_json_t;

#define lt_foreach_json_child(it, parent) \
		for (lt_json_t* it = (parent)->child; it; it = it->next)

lt_json_t* lt_json_parse(lt_alloc_t* alloc, char* data, usz len);

void lt_json_print(lt_file_t* file, lt_json_t* json);

lt_json_t* lt_json_find_child(lt_json_t* json, lstr_t key);

u64 lt_json_uint_or_default(lt_json_t* parent, lstr_t key, u64 otherwise);
i64 lt_json_int_or_default(lt_json_t* parent, lstr_t key, i64 otherwise);
f64 lt_json_float_or_default(lt_json_t* parent, lstr_t key, f64 otherwise);
b8 lt_json_bool_or_default(lt_json_t* parent, lstr_t key, b8 otherwise);
lstr_t lt_json_str_or_default(lt_json_t* parent, lstr_t key, lstr_t otherwise);
lstr_t lt_json_str_val_or_default(lt_json_t* parent, lstr_t key, lstr_t otherwise);

lstr_t lt_json_escape_str(lstr_t src, lt_alloc_t* alloc);
lstr_t lt_json_unescape_str(lstr_t src, lt_alloc_t* alloc);

void lt_json_free(lt_json_t* json, lt_alloc_t* alloc);

#endif
