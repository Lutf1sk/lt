#include <lt/json.h>
#include <lt/mem.h>
#include <lt/str.h>
#include <lt/io.h>
#include <lt/ctype.h>

typedef
struct parse_ctx {
	char* data;
	usz len;
	usz it;
	lt_arena_t* arena;
} parse_ctx_t;

static
void skip_whitespace(parse_ctx_t* cx) {
	while (cx->it < cx->len && lt_is_space(cx->data[cx->it])) {
		++cx->it;
	}
}

static
lstr_t consume_string(parse_ctx_t* cx) {
	++cx->it; // "
	char* start_it = &cx->data[cx->it];
	// TODO: This loop does not handle \\" correctly.
	while (cx->it < cx->len && (cx->data[cx->it] != '"' && (cx->it != 0 || cx->data[cx->it - 1] == '\\'))) {
		++cx->it;
	}
	++cx->it; // "
	return LSTR(start_it, &cx->data[cx->it] - start_it - 1);
}

static
lstr_t consume_number(parse_ctx_t* cx) {
	char* start_it = &cx->data[cx->it];
	while (cx->it < cx->len && (lt_is_digit(cx->data[cx->it]) || cx->data[cx->it] == '.'))
		++cx->it;
	return LSTR(start_it, &cx->data[cx->it] - start_it);
}

static LT_INLINE
lt_json_t json_make(lt_json_stype_t stype) {
	lt_json_t json;
	memset(&json, 0, sizeof(json));
	json.stype = stype;
	return json;
}

static lt_json_t* json_parse_object(parse_ctx_t* cx);
static lt_json_t* json_parse_value(parse_ctx_t* cx);

static
lt_json_t* json_parse_array(parse_ctx_t* cx) {
	lt_json_t* arr = lt_arena_reserve(cx->arena, sizeof(lt_json_t));
	*arr = json_make(LT_JSON_ARRAY);
	lt_json_t** current = &arr->child;

	++cx->it; // [
	skip_whitespace(cx);
	while (cx->it < cx->len && cx->data[cx->it] != ']') {
		lt_json_t* child = json_parse_value(cx);
		*current = child;
		current = &child->next;
		++arr->child_count;

		skip_whitespace(cx);
		if (cx->data[cx->it] == ',') {
			++cx->it;
			skip_whitespace(cx);
		}
	}

	++cx->it; // ]

	return arr;
}

static
lt_json_t* json_parse_value(parse_ctx_t* cx) {
	char c = cx->data[cx->it];
	switch (c) {
	case '{':
		return json_parse_object(cx);

	case '[':
		return json_parse_array(cx);

	case '"': {
		lt_json_t* new = lt_arena_reserve(cx->arena, sizeof(lt_json_t));
		*new = json_make(LT_JSON_STRING);
		new->str_val = consume_string(cx);
		return new;
	}

	case 't': {
		lstr_t expect = CLSTR("true");
		if (cx->it + expect.len > cx->len || !lt_lstr_eq(expect, LSTR(&cx->data[cx->it], expect.len)))
			return NULL;
		cx->it += expect.len;
		lt_json_t* new = lt_arena_reserve(cx->arena, sizeof(lt_json_t));
		*new = json_make(LT_JSON_BOOL);
		new->str_val = expect;
		return new;
	}

	case 'f': {
		lstr_t expect = CLSTR("false");
		if (cx->it + expect.len > cx->len || !lt_lstr_eq(expect, LSTR(&cx->data[cx->it], expect.len)))
			return NULL;
		cx->it += expect.len;
		lt_json_t* new = lt_arena_reserve(cx->arena, sizeof(lt_json_t));
		*new = json_make(LT_JSON_BOOL);
		new->str_val = expect;
		return new;
	}

	case 'n': {
		lstr_t expect = CLSTR("null");
		if (cx->it + expect.len > cx->len || !lt_lstr_eq(expect, LSTR(&cx->data[cx->it], expect.len)))
			return NULL;
		cx->it += expect.len;
		lt_json_t* new = lt_arena_reserve(cx->arena, sizeof(lt_json_t));
		*new = json_make(LT_JSON_NULL);
		new->str_val = expect;
		return new;
	}

	default:
		if (lt_is_digit(c)) {
			lt_json_t* new = lt_arena_reserve(cx->arena, sizeof(lt_json_t));
			*new = json_make(LT_JSON_NUMBER);
			new->str_val = consume_number(cx);
			return new;
		}
		break;
	}

	//lt_ferrf("Unexpected character '%c' (%p)\n", c, (void*)(usz)c);
	return NULL;
}

static
lt_json_t* json_parse_entry(parse_ctx_t* cx) {
	skip_whitespace(cx);
	lstr_t key = consume_string(cx);
	skip_whitespace(cx);
	++cx->it; // :
	skip_whitespace(cx);

	lt_json_t* val = json_parse_value(cx);

	val->key = key;
	return val;
}

static
lt_json_t* json_parse_object(parse_ctx_t* cx) {
	++cx->it; // {

	lt_json_t* obj = lt_arena_reserve(cx->arena, sizeof(lt_json_t));
	*obj = json_make(LT_JSON_OBJECT);
	lt_json_t** current = &obj->child;

	skip_whitespace(cx);
	while (cx->it < cx->len && cx->data[cx->it] != '}') {
		lt_json_t* new = json_parse_entry(cx);
		*current = new;
		current = &new->next;

		skip_whitespace(cx);
		if (cx->data[cx->it] == ',') {
			++cx->it;
			skip_whitespace(cx);
		}
	}

	++cx->it; // }
	return obj;
}

lt_json_t* lt_json_parse(lt_arena_t* arena, char* data, usz len) {
	parse_ctx_t cx;
	cx.data = data;
	cx.len = len;
	cx.arena = arena;
	cx.it = 0;

	skip_whitespace(&cx);

	return json_parse_value(&cx);
}

static
void print_indent(lt_file_t* file, int indent) {
	for (int i = 0; i < indent; ++i)
		lt_file_printc(file, '\t');
}

static
void json_print_recursive(lt_file_t* file, lt_json_t* json, int indent) {
	switch (json->stype) {
	case LT_JSON_ARRAY: {
		lt_file_printls(file, CLSTR("[\n"));
		lt_json_t* it = json->child;
		while (it) {
			print_indent(file, indent + 1);
			json_print_recursive(file, it, indent + 1);
			it = it->next;
		}
		print_indent(file, indent);
		lt_file_printls(file, CLSTR("]\n"));
	}	break;

	case LT_JSON_OBJECT: {
		lt_file_printls(file, CLSTR("{\n"));
		lt_json_t* it = json->child;
		while (it) {
			print_indent(file, indent + 1);
			lt_file_printf(file, "%S: ", it->key);
			json_print_recursive(file, it, indent + 1);
			it = it->next;
		}
		print_indent(file, indent);
		lt_file_printls(file, CLSTR("}\n"));
	}	break;

	case LT_JSON_STRING: case LT_JSON_NUMBER: case LT_JSON_BOOL: case LT_JSON_NULL: {
		lt_file_printf(file, "%S\n", json->str_val);
	}	break;
	}
}

void lt_json_print(lt_file_t* file, lt_json_t* json) {
	json_print_recursive(file, json, 0);
}

lt_json_t* lt_json_find_child(lt_json_t* json, lstr_t key) {
	lt_json_t* it = json->child;
	while (it) {
		if (lt_lstr_eq(it->key, key))
			return it;
		it = it->next;
	}
	return NULL;
}

u64 lt_json_uint_val(lt_json_t* json) {
	return lt_lstr_uint(json->str_val);
}

i64 lt_json_int_val(lt_json_t* json) {
	return lt_lstr_int(json->str_val);
}

f64 lt_json_float_val(lt_json_t* json) {
	return lt_lstr_float(json->str_val);
}

b8 lt_json_bool_val(lt_json_t* json) {
	return lt_lstr_eq(json->str_val, CLSTR("true"));
}

