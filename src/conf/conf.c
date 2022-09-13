#include <lt/conf.h>
#include <lt/str.h>
#include <lt/mem.h>
#include <lt/io.h>
#include <lt/ctype.h>

lstr_t conf_type_str(lt_conf_stype_t stype) {
	switch (stype) {
#define LT_CONF_OP(x) case LT_CONF_##x: return CLSTR(#x);
	LT_FOR_EACH_CONF()
#undef LT_CONF_OP
	default:
		return CLSTR("INVALID");
	}
}

typedef
struct parse_ctx {
	char* it;
	char* end;
} parse_ctx_t;

static b8 parse_val(parse_ctx_t* cx, lt_conf_t* cf);

static
b8 skip_whitespace(parse_ctx_t* cx) {
	while (cx->it < cx->end && lt_is_space(*cx->it))
		++cx->it;
	return cx->it == cx->end;
}

static
b8 consume_string(parse_ctx_t* cx, lstr_t str) {
	if (cx->end - cx->it < str.len && memcmp(cx->it, str.str, str.len) == 0)
		return 0;
	cx->it += str.len;
	return 1;
}

static
b8 parse_obj_body(parse_ctx_t* cx, lt_conf_t* cf) {
	cf->stype = LT_CONF_OBJECT;
	cf->children = NULL;
	cf->count = 0;

	for (;;) {
		if (skip_whitespace(cx) || *cx->it == '}')
			return 1;

		cf->children = realloc(cf->children, (cf->count + 1) * sizeof(lt_conf_t));
		lt_conf_t* child = &cf->children[cf->count++];

		// Consume name
		char* key_start = cx->it;
		while (cx->it < cx->end && lt_is_ident_body(*cx->it))
			++cx->it;
		lstr_t key = LSTR(key_start, cx->it - key_start);
		if (!key.len)
			goto syntax_err;
		child->key = key;

		// Parse value
		if (!parse_val(cx, child))
			goto syntax_err;
	}

syntax_err:
	cf->count--;
	lt_conf_free(cf);
	return 0;
}

static
b8 parse_arr_body(parse_ctx_t* cx, lt_conf_t* cf) {
	cf->stype = LT_CONF_ARRAY;
	cf->children = NULL;
	cf->count = 0;

	for (;;) {
		if (skip_whitespace(cx))
			return 0;
		if (*cx->it == ']')
			return 1;

		cf->children = realloc(cf->children, (cf->count + 1) * sizeof(lt_conf_t));
		lt_conf_t* child = &cf->children[cf->count++];

		// Parse value
		if (!parse_val(cx, child)) {
			cf->count--;
			lt_conf_free(cf);
			return 0;
		}
	}
}

static
b8 parse_val(parse_ctx_t* cx, lt_conf_t* cf) {
	if (skip_whitespace(cx))
		return 0;

	char c = *cx->it++;
	switch (c) {
	case '{':
		if (!parse_obj_body(cx, cf) || skip_whitespace(cx))
			return 0;
		return *cx->it++ == '}';

	case '[':
		if (!parse_arr_body(cx, cf) || skip_whitespace(cx))
			return 0;
		return *cx->it++ == ']';

	case 't':
		if (!consume_string(cx, CLSTR("rue")))
			return 0;
		cf->stype = LT_CONF_BOOL;
		cf->bool_val = 1;
		return 1;

	case 'f':
		if (!consume_string(cx, CLSTR("alse")))
			return 0;
		cf->stype = LT_CONF_BOOL;
		cf->bool_val = 0;
		return 1;

	case '"':
		char* start = cx->it;
		while (*cx->it++ != '"') {
			if (cx->it >= cx->end)
				return 0;
		}
		cf->stype = LT_CONF_STRING;
		cf->str_val = start;
		cf->count = cx->it - start - 1;
		return 1;
		return 1;

	default:
		if (lt_is_digit(c)) {
			char* start = cx->it - 1;
			while (cx->it < cx->end && lt_is_numeric_body(*cx->it))
				++cx->it;
			cf->stype = LT_CONF_INT;
			// TODO: Properly parse floats and different types of integers
			cf->int_val = lt_lstr_uint(LSTR(start, cx->it - start));
			return 1;
		}
		return 0;
	}
}

b8 lt_conf_parse(lt_conf_t* cf, lstr_t data) {
	parse_ctx_t cx;
	cx.it = data.str;
	cx.end = data.str + data.len;
	return parse_obj_body(&cx, cf);;
}

lt_conf_t* lt_conf_find(lt_conf_t* parent, lstr_t key) {
	if (!parent || parent->stype != LT_CONF_OBJECT)
		return NULL;

	for (usz i = 0; i < parent->count; ++i) {
		if (lt_lstr_eq(parent->children[i].key, key))
			return &parent->children[i];
	}
	return NULL;
}

lt_conf_t* lt_conf_find_int(lt_conf_t* cf, lstr_t key, i64* out) {
	cf = lt_conf_find(cf, key);
	if (!cf || cf->stype != LT_CONF_INT)
		return NULL;
	*out = cf->int_val;
	return cf;
}

lt_conf_t* lt_conf_find_uint(lt_conf_t* cf, lstr_t key, u64* out) {
	cf = lt_conf_find(cf, key);
	if (!cf || cf->stype != LT_CONF_INT)
		return NULL;
	*out = cf->uint_val;
	return cf;
}

lt_conf_t* lt_conf_find_bool(lt_conf_t* cf, lstr_t key, b8* out) {
	cf = lt_conf_find(cf, key);
	if (!cf || cf->stype != LT_CONF_BOOL)
		return NULL;
	*out = cf->bool_val;
	return cf;
}

lt_conf_t* lt_conf_find_str(lt_conf_t* cf, lstr_t key, lstr_t* out) {
	cf = lt_conf_find(cf, key);
	if (!cf || cf->stype != LT_CONF_STRING)
		return NULL;
	*out = LSTR(cf->str_val, cf->count);
	return cf;
}

static
void lt_conf_write_indent(lt_file_t* file, isz indent) {
	for (isz i = 0; i < indent; ++i) // TODO: Optimize this
		lt_file_printf(file, "\t");
}

static
b8 lt_conf_write_unsafe(lt_conf_t* cf, lt_file_t* file, isz indent) {
	switch (cf->stype) {
	case LT_CONF_OBJECT:
		if (indent)
			lt_file_printf(file, "{\n");
		for (usz i = 0; i < cf->count; ++i) {
			lt_conf_t* child = &cf->children[i];
			lt_conf_write_indent(file, indent);
			lt_file_printf(file, "%S ", child->key);
			lt_conf_write_unsafe(child, file, indent + 1);
		}
		if (indent) {
			lt_conf_write_indent(file, indent - 1);
			lt_file_printf(file, "}\n");
		}
		return 1;

	case LT_CONF_ARRAY:
		lt_file_printf(file, "[\n");
		for (usz i = 0; i < cf->count; ++i) {
			lt_conf_write_indent(file, indent);
			lt_conf_write_unsafe(&cf->children[i], file, 0);
		}
		lt_conf_write_indent(file, indent - 1);
		lt_file_printf(file, "]\n");
		return 1;

	case LT_CONF_INT: return lt_file_printf(file, "%iq\n", cf->int_val) != -1;
	case LT_CONF_STRING: return lt_file_printf(file, "\"%S\"\n", LSTR(cf->str_val, cf->count)) != -1;
	case LT_CONF_FLOAT: return 0; // TODO
	case LT_CONF_BOOL: return lt_file_printf(file, "%S\n", cf->bool_val ? CLSTR("true") : CLSTR("false")) != -1;
	}

	return 0;
}

b8 lt_conf_write(lt_conf_t* cf, lt_file_t* file) {
	if (!file)
		file = lt_stdout;
	return lt_conf_write_unsafe(cf, file, 0);
}

static
void lt_conf_free_children(lt_conf_t* cf) {
	for (usz i = 0; i < cf->count; ++i) {
		lt_conf_t* child = &cf->children[i];
		if (child->stype == LT_CONF_OBJECT || child->stype == LT_CONF_ARRAY)
			lt_conf_free_children(child);
	}
	if (cf->count)
		free(cf->children);
}

void lt_conf_free(lt_conf_t* cf) {
	if (!cf || !(cf->stype == LT_CONF_OBJECT || cf->stype == LT_CONF_ARRAY))
		return;
	lt_conf_free_children(cf);
}

