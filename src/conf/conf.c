#include <lt/conf.h>
#include <lt/str.h>
#include <lt/mem.h>
#include <lt/io.h>
#include <lt/ctype.h>
#include <lt/darr.h>
#include <lt/internal.h>
#include <lt/math.h>

typedef
struct parse_ctx {
	lt_alloc_t* alloc;
	lt_conf_err_info_t* err_info;
	char* begin;
	char* end;
	char* it;
} parse_ctx_t;

#define RETURN_ERROR(e, s) \
	do { \
		lt_err_t err_ = (e); \
		if (cx->err_info) \
			*cx->err_info = LT_CONF_ERR_INFO(err_, (s)); \
		return err_; \
	} while (0)

#define GOTO_ERROR(l, e, s) \
	do { \
		err = (e); \
		if (cx->err_info) \
			*cx->err_info = LT_CONF_ERR_INFO(err, (s)); \
		goto (l); \
	} while (0)

static lt_err_t parse_val(parse_ctx_t cx[static 1], lt_conf_t cf[static 1]);

static
void skip_whitespace(parse_ctx_t cx[static 1]) {
	while (cx->it < cx->end && lt_is_space(*cx->it))
		++cx->it;
}

static
lt_err_t consume_string(parse_ctx_t cx[static 1], lstr_t str) {
	char* it = str.str, *end = it + str.len;

	char* begin = cx->it;
	while (it < end) {
		if (cx->it >= cx->end || *cx->it != *it) {
			lstr_t sym = LSTR(begin, cx->it - begin + 1);
			if (cx->it >= cx->end)
				sym = CLSTR("EOF");
			lstr_t err_str;
			lt_aprintf(&err_str, cx->alloc, "expected '%S', got '%S'", str, sym);
			RETURN_ERROR(LT_ERR_INVALID_FORMAT, err_str);
		}
		++it;
		++cx->it;
	}
	return LT_SUCCESS;
}

static
lt_err_t consume_key(parse_ctx_t cx[static 1], lstr_t out[static 1]) {
	char* begin = cx->it;
	while (cx->it < cx->end && lt_is_ident_body(*cx->it))
		++cx->it;

	if (cx->it == begin) {
		lstr_t sym = LSTR(cx->it, 1);
		if (cx->it == cx->end)
			sym = CLSTR("EOF");
		lstr_t err_str;
		lt_aprintf(&err_str, cx->alloc, "expected a valid key, got '%S'", sym);
		RETURN_ERROR(LT_ERR_INVALID_FORMAT, err_str);
	}

	if (out)
		*out = LSTR(begin, cx->it - begin);
	return LT_SUCCESS;
}

static
lt_err_t parse_obj_body(parse_ctx_t cx[static 1], lt_conf_t cf[static 1]) {
	lt_err_t err;

	lt_darr(lt_conf_t) children = lt_darr_create(lt_conf_t, 16, cx->alloc);
	if (!children)
		RETURN_ERROR(LT_ERR_INVALID_FORMAT, lt_strdup(cx->alloc, CLSTR("out of memory")));

	while (cx->it < cx->end && *cx->it != '}') {
		lstr_t key;
		if ((err = consume_key(cx, &key)))
			goto err0;

		lt_conf_t child;
		skip_whitespace(cx);
		if ((err = parse_val(cx, &child)))
			goto err0;

		child.key = key;
		lt_darr_push(children, child);

		skip_whitespace(cx);
	}

	cf->stype = LT_CONF_OBJECT;
	cf->children = children;
	cf->child_count = lt_darr_count(children);
	return LT_SUCCESS;

err0:	for (lt_conf_t* it = children, *end = it + lt_darr_count(children); it < end; ++it)
			lt_conf_free(it, cx->alloc);
		lt_darr_destroy(children);
		return err;
}

static
lt_err_t parse_arr_body(parse_ctx_t cx[static 1], lt_conf_t cf[static 1]) {
	lt_err_t err;

	lt_darr(lt_conf_t) children = lt_darr_create(lt_conf_t, 16, cx->alloc);
	if (!children)
		RETURN_ERROR(LT_ERR_OUT_OF_MEMORY, lt_strdup(cx->alloc, CLSTR("out of memory")));

	while (cx->it < cx->end && *cx->it != ']') {
		lt_conf_t child;
		if ((err = parse_val(cx, &child)))
			goto err0;

		child.key = NLSTR();
		lt_darr_push(children, child);

		skip_whitespace(cx);
	}

	cf->stype = LT_CONF_ARRAY;
	cf->children = children;
	cf->child_count = lt_darr_count(children);
	return LT_SUCCESS;

err0:	for (lt_conf_t* it = children, *end = it + lt_darr_count(children); it < end; ++it)
			lt_conf_free(it, cx->alloc);
		lt_darr_destroy(children);
		return err;
}

static
lt_err_t parse_val(parse_ctx_t cx[static 1], lt_conf_t cf[static 1]) {
	lt_err_t err;

	if (cx->it >= cx->end)
		RETURN_ERROR(LT_ERR_INVALID_FORMAT, lt_strdup(cx->alloc, CLSTR("unexpected end of input")));

	switch (*cx->it) {
	case '"': {
		char* begin = ++cx->it;
		while (cx->it < cx->end && *cx->it != '"')
			++cx->it;
		if (cx->it >= cx->end)
			RETURN_ERROR(LT_ERR_INVALID_FORMAT, lt_strdup(cx->alloc, CLSTR("unterminated string")));

		cf->stype = LT_CONF_STRING;
		cf->str_val = LSTR(begin, cx->it++ - begin);
	}	return LT_SUCCESS;

	case '{':
		++cx->it;
		skip_whitespace(cx);
		if ((err = parse_obj_body(cx, cf)))
			return err;
		return consume_string(cx, CLSTR("}"));

	case '[':
		++cx->it;
		skip_whitespace(cx);
		if ((err = parse_arr_body(cx, cf)))
			return err;
		return consume_string(cx, CLSTR("]"));

	case 't':
		cf->stype = LT_CONF_BOOL;
		cf->bool_val = 1;
		return consume_string(cx, CLSTR("true"));

	case 'f':
		cf->stype = LT_CONF_BOOL;
		cf->bool_val = 0;
		return consume_string(cx, CLSTR("false"));

	default: {
		if (lt_is_numeric_head(*cx->it)) {
			char* begin = cx->it;
			while (cx->it < cx->end && lt_is_numeric_body(*cx->it))
				++cx->it;

			cf->stype = LT_CONF_INT;
			err = lt_lstoi(LSTR(begin, cx->it - begin), &cf->int_val);
			if (err == LT_ERR_OVERFLOW)
				RETURN_ERROR(LT_ERR_OVERFLOW, lt_strdup(cx->alloc, CLSTR("integer overflow")));
			else if (err != LT_SUCCESS)
				RETURN_ERROR(LT_ERR_INVALID_FORMAT, lt_strdup(cx->alloc, CLSTR("expected a valid integer")));
			return LT_SUCCESS;
		}

		lstr_t err_str;
		lt_aprintf(&err_str, cx->alloc, "unexpected character '%c', expected a value", *cx->it);
		RETURN_ERROR(LT_ERR_INVALID_FORMAT, err_str);
	}
	}
}

lt_err_t lt_conf_add_child(lt_conf_t cf[static 1], lt_conf_t child[static 1]) {
	LT_ASSERT(cf->stype == LT_CONF_OBJECT || cf->stype == LT_CONF_ARRAY);
	lt_darr_push(cf->children, *child);
	++cf->child_count;
	return LT_SUCCESS;
}

lt_err_t lt_conf_erase_str(lt_conf_t cf[static 1], lstr_t str, lt_alloc_t alloc[static 1]) {
	LT_ASSERT(cf->stype == LT_CONF_OBJECT || cf->stype == LT_CONF_ARRAY);

	for (usz i = 0; i < cf->child_count; ++i) {
		if (cf->children[i].stype == LT_CONF_STRING && !lt_lseq(cf->children[i].str_val, str))
			continue;

		lt_conf_free(&cf->children[i], alloc);
		lt_darr_erase(cf->children, i, 1);
		--cf->child_count;
		--i;
	}

	return LT_SUCCESS;
}

lt_err_t lt_conf_erase_index(lt_conf_t cf[static 1], usz index, lt_alloc_t alloc[static 1]) {
	LT_ASSERT(cf->stype == LT_CONF_OBJECT || cf->stype == LT_CONF_ARRAY);

	LT_ASSERT(index < cf->child_count);

	lt_conf_free(&cf->children[index], alloc);
	lt_darr_erase(cf->children, index, 1);
	--cf->child_count;
	return LT_SUCCESS;
}

lt_err_t lt_conf_erase_key(lt_conf_t cf[static 1], lstr_t key, lt_alloc_t alloc[static 1]) {
	LT_ASSERT(cf->stype == LT_CONF_OBJECT);

	for (usz i = 0; i < cf->child_count; ++i) {
		if (!lt_lseq(cf->children[i].key, key))
			continue;

		lt_conf_free(&cf->children[i], alloc);
		lt_darr_erase(cf->children, i, 1);
		--cf->child_count;
		return LT_SUCCESS;
	}
	return LT_ERR_NOT_FOUND;
}

lt_err_t lt_conf_parse(lt_conf_t cf[static 1], void* data, usz len, lt_conf_err_info_t* err_info, lt_alloc_t alloc[static 1]) {
	parse_ctx_t cx;
	cx.alloc = alloc;
	cx.err_info = err_info;
	cx.begin = data;
	cx.end = LT_ADDPTR(data, len);
	cx.it = data;

	skip_whitespace(&cx);
	return parse_obj_body(&cx, cf);
}

void lt_conf_free_err_info(lt_conf_err_info_t err_info[static 1], lt_alloc_t alloc[static 1]) {
	lt_mfree(alloc, err_info->err_str.str);
}

lt_conf_t* lt_conf_find(lt_conf_t parent[static 1], lstr_t key_path) {
	char* it = key_path.str, *end = it + key_path.len;
	while (it < end) {
		if (!parent || parent->stype != LT_CONF_OBJECT)
			return NULL;

		lstr_t key = lt_lssplit(LSTR(it, end - it), '.');

		for (usz i = 0; i < parent->child_count; ++i) {
			if (lt_lseq(parent->children[i].key, key)) {
				parent = &parent->children[i];
				it += key.len + 1;
				goto found;
			}
		}
		return NULL;
	found:;
	}
	return parent;
}

lt_conf_t* lt_conf_find_int(lt_conf_t parent[static 1], lstr_t key_path, i64* out) {
	lt_conf_t* cf = lt_conf_find(parent, key_path);
	if (!cf || cf->stype != LT_CONF_INT)
		return NULL;
	if (out)
		*out = cf->int_val;
	return cf;
}

lt_conf_t* lt_conf_find_uint(lt_conf_t parent[static 1], lstr_t key_path, u64* out) {
	lt_conf_t* cf = lt_conf_find(parent, key_path);
	if (!cf || cf->stype != LT_CONF_INT)
		return NULL;
	if (out)
		*out = cf->uint_val;
	return cf;
}

lt_conf_t* lt_conf_find_bool(lt_conf_t parent[static 1], lstr_t key_path, b8* out) {
	lt_conf_t* cf = lt_conf_find(parent, key_path);
	if (!cf || cf->stype != LT_CONF_BOOL)
		return NULL;
	if (out)
		*out = cf->bool_val;
	return cf;
}

lt_conf_t* lt_conf_find_str(lt_conf_t parent[static 1], lstr_t key_path, lstr_t* out) {
	lt_conf_t* cf = lt_conf_find(parent, key_path);
	if (!cf || cf->stype != LT_CONF_STRING)
		return NULL;
	if (out)
		*out = cf->str_val;
	return cf;
}

lt_conf_t* lt_conf_find_float(lt_conf_t parent[static 1], lstr_t key_path, f64* out) {
	lt_conf_t* cf = lt_conf_find(parent, key_path);
	if (!cf || cf->stype != LT_CONF_FLOAT)
		return NULL;
	if (out)
		*out = cf->float_val;
	return cf;
}

lt_conf_t* lt_conf_find_object(lt_conf_t parent[static 1], lstr_t key_path, lt_conf_t** out) {
	lt_conf_t* cf = lt_conf_find(parent, key_path);
	if (!cf || cf->stype != LT_CONF_OBJECT)
		return NULL;
	if (out)
		*out = cf;
	return cf;
}
lt_conf_t* lt_conf_find_array(lt_conf_t parent[static 1], lstr_t key_path, lt_conf_t** out) {
	lt_conf_t* cf = lt_conf_find(parent, key_path);
	if (!cf || cf->stype != LT_CONF_ARRAY)
		return NULL;
	if (out)
		*out = cf;
	return cf;
}

i64 lt_conf_int(lt_conf_t parent[static 1], lstr_t key_path) {
	lt_conf_t* cf = lt_conf_find(parent, key_path);
	if (!cf || cf->stype != LT_CONF_INT)
		lt_ferrf("missing required integer field '%S.%S'\n", parent->key, key_path);
	return cf->int_val;
}

u64 lt_conf_uint(lt_conf_t parent[static 1], lstr_t key_path) {
	lt_conf_t* cf = lt_conf_find(parent, key_path);
	if (!cf || cf->stype != LT_CONF_INT)
		lt_ferrf("missing required integer field '%S.%S'\n", parent->key, key_path);
	return cf->uint_val;
}

b8 lt_conf_bool(lt_conf_t parent[static 1], lstr_t key_path) {
	lt_conf_t* cf = lt_conf_find(parent, key_path);
	if (!cf || cf->stype != LT_CONF_BOOL)
		lt_ferrf("missing required boolean field '%S.%S'\n", parent->key, key_path);
	return cf->bool_val;
}

lstr_t lt_conf_str(lt_conf_t parent[static 1], lstr_t key_path) {
	lt_conf_t* cf = lt_conf_find(parent, key_path);
	if (!cf || cf->stype != LT_CONF_STRING)
		lt_ferrf("missing required string field '%S.%S'\n", parent->key, key_path);
	return cf->str_val;
}

f64 lt_conf_float(lt_conf_t parent[static 1], lstr_t key_path) {
	lt_conf_t* cf = lt_conf_find(parent, key_path);
	if (!cf || cf->stype != LT_CONF_FLOAT)
		lt_ferrf("missing required floating-point field '%S.%S'\n", parent->key, key_path);
	return cf->float_val;
}

lt_conf_t* lt_conf_object(lt_conf_t parent[static 1], lstr_t key_path) {
	lt_conf_t* cf = lt_conf_find(parent, key_path);
	if (!cf || cf->stype != LT_CONF_OBJECT)
		lt_ferrf("missing required object field '%S.%S'\n", parent->key, key_path);
	return cf;
}

lt_conf_t* lt_conf_array(lt_conf_t parent[static 1], lstr_t key_path)  {
	lt_conf_t* cf = lt_conf_find(parent, key_path);
	if (!cf || cf->stype != LT_CONF_ARRAY)
		lt_ferrf("missing required array field '%S.%S'\n", parent->key, key_path);
	return cf;
}

i64 lt_conf_find_int_default(lt_conf_t parent[static 1], lstr_t key_path, i64 default_) {
	lt_conf_t* cf = lt_conf_find(parent, key_path);
	if (!cf || cf->stype != LT_CONF_INT)
		return default_;
	return cf->int_val;
}

u64 lt_conf_find_uint_default(lt_conf_t parent[static 1], lstr_t key_path, u64 default_) {
	lt_conf_t* cf = lt_conf_find(parent, key_path);
	if (!cf || cf->stype != LT_CONF_INT)
		return default_;
	return cf->uint_val;
}

b8 lt_conf_find_bool_default(lt_conf_t parent[static 1], lstr_t key_path, b8 default_) {
	lt_conf_t* cf = lt_conf_find(parent, key_path);
	if (!cf || cf->stype != LT_CONF_BOOL)
		return default_;
	return cf->bool_val;
}

lstr_t lt_conf_find_str_default(lt_conf_t parent[static 1], lstr_t key_path, lstr_t default_) {
	lt_conf_t* cf = lt_conf_find(parent, key_path);
	if (!cf || cf->stype != LT_CONF_STRING)
		return default_;
	return cf->str_val;
}

f64 lt_conf_find_float_default(lt_conf_t parent[static 1], lstr_t key_path, f64 default_) {
	lt_conf_t* cf = lt_conf_find(parent, key_path);
	if (!cf || cf->stype != LT_CONF_FLOAT)
		return default_;
	return cf->float_val;
}

static
isz lt_conf_write_indented(lt_conf_t cf[static 1], lt_write_fn_t callb, void* usr, usz indent) {
	usz bytes = 0;
	isz res = 0;

	switch (cf->stype) {
	case LT_CONF_OBJECT:
		if (indent) {
			if ((res = lt_io_printf(callb, usr, "{\n")) < 0)
				return res;
			bytes += res;
		}
		for (usz i = 0; i < cf->child_count; ++i) {
			if ((res = lt_io_printf(callb, usr, "%r\t%S ", indent, cf->children[i].key)) < 0)
				return res;
			bytes += res;
			if ((res = lt_conf_write_indented(&cf->children[i], callb, usr, indent + 1)) < 0)
				return res;
			bytes += res;
		}
		if (indent) {
			if ((res = lt_io_printf(callb, usr, "%r\t", indent - 1)) < 0)
				return res;
			bytes += res;
			if ((res = lt_io_printf(callb, usr, "}\n")) < 0)
				return res;
			bytes += res;
		}
		return bytes;

	case LT_CONF_ARRAY:
		if ((res = lt_io_printf(callb, usr, "[\n")) < 0)
			return res;
		bytes += res;
		for (usz i = 0; i < cf->child_count; ++i) {
			if ((res = lt_io_printf(callb, usr, "%r\t", indent)) < 0)
				return res;
			bytes += res;
			if ((res = lt_conf_write_indented(&cf->children[i], callb, usr, indent + 1)) < 0)
				return res;
			bytes += res;
		}
		if (indent) {
			if ((res = lt_io_printf(callb, usr, "%r\t", indent - 1)) < 0)
				return res;
			bytes += res;
		}
		if ((res = lt_io_printf(callb, usr, "]\n")) < 0)
			return res;
		return bytes + res;

	case LT_CONF_INT: return lt_io_printf(callb, usr, "%iq\n", cf->int_val);
	case LT_CONF_STRING: return lt_io_printf(callb, usr, "\"%S\"\n", cf->str_val);
	case LT_CONF_FLOAT: return -LT_ERR_UNSUPPORTED;
	case LT_CONF_BOOL: return lt_io_printf(callb, usr, cf->bool_val ? "true\n" : "false\n");

	default:
		return -LT_ERR_INVALID_TYPE;
	}
}

isz lt_conf_write(lt_conf_t cf[static 1], lt_write_fn_t callb, void* usr) {
	return lt_conf_write_indented(cf, callb, usr, 0);
}

static
void lt_conf_free_children(lt_conf_t cf[static 1], lt_alloc_t alloc[static 1]) {
	for (usz i = 0; i < cf->child_count; ++i) {
		lt_conf_t* child = &cf->children[i];
		if (child->stype == LT_CONF_OBJECT || child->stype == LT_CONF_ARRAY)
			lt_conf_free_children(child, alloc);
	}

	lt_darr_destroy(cf->children);
}

void lt_conf_free(lt_conf_t cf[static 1], lt_alloc_t alloc[static 1]) {
	if (!cf || !(cf->stype == LT_CONF_OBJECT || cf->stype == LT_CONF_ARRAY))
		return;
	lt_conf_free_children(cf, alloc);
}

