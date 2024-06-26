#include <lt/json.h>
#include <lt/mem.h>
#include <lt/str.h>
#include <lt/io.h>
#include <lt/ctype.h>
#include <lt/strstream.h>

typedef
struct parse_ctx {
	const char* data;
	usz len;
	usz it;
	lt_alloc_t* alloc;
} parse_ctx_t;

static
void skip_whitespace(parse_ctx_t cx[static 1]) {
	while (cx->it < cx->len && lt_is_space(cx->data[cx->it])) {
		++cx->it;
	}
}

static
lstr_t consume_string(parse_ctx_t cx[static 1]) {
	++cx->it; // "
	const char* start_it = &cx->data[cx->it];
	// TODO: This loop does not handle \\" correctly.
	while (cx->it < cx->len && cx->data[cx->it] != '"') {
		if (cx->data[cx->it++] == '\\')
			++cx->it;
	}
	++cx->it; // "
	return LSTR((char*)start_it, &cx->data[cx->it] - start_it - 1);
}

static
lstr_t consume_number(parse_ctx_t cx[static 1]) {
	const char* start_it = &cx->data[cx->it];
	while (cx->it < cx->len && (lt_is_digit(cx->data[cx->it]) || cx->data[cx->it] == '.' || cx->data[cx->it] == '-'))
		++cx->it;
	return LSTR((char*)start_it, &cx->data[cx->it] - start_it);
}

static LT_INLINE
lt_json_t json_make(lt_json_stype_t stype) {
	lt_json_t json;
	memset(&json, 0, sizeof(json));
	json.stype = stype;
	return json;
}

static lt_json_t* json_parse_object(parse_ctx_t cx[static 1]);
static lt_json_t* json_parse_value(parse_ctx_t cx[static 1]);

static
lt_json_t* json_parse_array(parse_ctx_t cx[static 1]) {
	lt_json_t* arr = lt_malloc(cx->alloc, sizeof(lt_json_t));
	LT_ASSERT(arr);
	*arr = json_make(LT_JSON_ARRAY);
	lt_json_t** current = &arr->child;

	++cx->it; // [
	skip_whitespace(cx);
	while (cx->it < cx->len && cx->data[cx->it] != ']') {
		lt_json_t* child = json_parse_value(cx);
		LT_ASSERT(child);
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
lt_json_t* json_parse_value(parse_ctx_t cx[static 1]) {
	char c = cx->data[cx->it];
	switch (c) {
	case '{':
		return json_parse_object(cx);

	case '[':
		return json_parse_array(cx);

	case '"': {
		lt_json_t* new = lt_malloc(cx->alloc, sizeof(lt_json_t));
		LT_ASSERT(new);
		*new = json_make(LT_JSON_STRING);
		new->str_val = consume_string(cx);
		return new;
	}

	case 't': {
		lstr_t expect = CLSTR("true");
		if (cx->it + expect.len > cx->len || !lt_lseq(expect, LSTR((char*)&cx->data[cx->it], expect.len)))
			return NULL;
		cx->it += expect.len;
		lt_json_t* new = lt_malloc(cx->alloc, sizeof(lt_json_t));
		LT_ASSERT(new);
		*new = json_make(LT_JSON_BOOL);
		new->str_val = expect;
		return new;
	}

	case 'f': {
		lstr_t expect = CLSTR("false");
		if (cx->it + expect.len > cx->len || !lt_lseq(expect, LSTR((char*)&cx->data[cx->it], expect.len)))
			return NULL;
		cx->it += expect.len;
		lt_json_t* new = lt_malloc(cx->alloc, sizeof(lt_json_t));
		LT_ASSERT(new);
		*new = json_make(LT_JSON_BOOL);
		new->str_val = expect;
		return new;
	}

	case 'n': {
		lstr_t expect = CLSTR("null");
		if (cx->it + expect.len > cx->len || !lt_lseq(expect, LSTR((char*)&cx->data[cx->it], expect.len)))
			return NULL;
		cx->it += expect.len;
		lt_json_t* new = lt_malloc(cx->alloc, sizeof(lt_json_t));
		LT_ASSERT(new);
		*new = json_make(LT_JSON_NULL);
		new->str_val = expect;
		return new;
	}

	default:
		if (lt_is_digit(c) || c == '-') {
			lt_json_t* new = lt_malloc(cx->alloc, sizeof(lt_json_t));
			LT_ASSERT(new);
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
lt_json_t* json_parse_entry(parse_ctx_t cx[static 1]) {
	skip_whitespace(cx);
	lstr_t key = consume_string(cx);
	skip_whitespace(cx);
	++cx->it; // :
	skip_whitespace(cx);

	lt_json_t* val = json_parse_value(cx);
	if (!val)
		lt_werrf("failed to parse '%S'\n", LSTR((char*)&cx->data[cx->it], cx->len - cx->it));
	val->key = key;
	return val;
}

static
lt_json_t* json_parse_object(parse_ctx_t cx[static 1]) {
	++cx->it; // {

	lt_json_t* obj = lt_malloc(cx->alloc, sizeof(lt_json_t));
	LT_ASSERT(obj);
	*obj = json_make(LT_JSON_OBJECT);
	lt_json_t** current = &obj->child;

	skip_whitespace(cx);
	while (cx->it < cx->len && cx->data[cx->it] != '}') {
		lt_json_t* new = json_parse_entry(cx);
		*current = new;
		current = &new->next;

		++obj->child_count;

		skip_whitespace(cx);
		if (cx->data[cx->it] == ',') {
			++cx->it;
			skip_whitespace(cx);
		}
	}

	++cx->it; // }
	return obj;
}

lt_json_t* lt_json_parse(lt_alloc_t alloc[static 1], const char* data, usz len) {
	parse_ctx_t cx;
	cx.data = data;
	cx.len = len;
	cx.alloc = alloc;
	cx.it = 0;

	skip_whitespace(&cx);

	return json_parse_value(&cx);
}

static
void print_indent(lt_file_t* file, int indent) {
	for (int i = 0; i < indent; ++i)
		lt_fprintf(file, "\t");
}

static
void json_print_recursive(lt_file_t* file, const lt_json_t json[static 1], int indent) {
	switch (json->stype) {
	case LT_JSON_ARRAY: {
		lt_fprintf(file, "[\n");
		lt_json_t* it = json->child;
		while (it) {
			print_indent(file, indent + 1);
			json_print_recursive(file, it, indent + 1);
			it = it->next;
			if (it)
				lt_fprintf(file, ",");
			lt_fprintf(file, "\n");
		}
		print_indent(file, indent);
		lt_fprintf(file, "]");
	}	break;

	case LT_JSON_OBJECT: {
		lt_fprintf(file, "{\n");
		lt_json_t* it = json->child;
		while (it) {
			print_indent(file, indent + 1);
			lt_fprintf(file, "%S: ", it->key);
			json_print_recursive(file, it, indent + 1);
			it = it->next;
			if (it)
				lt_fprintf(file, ",");
			lt_fprintf(file, "\n");
		}
		print_indent(file, indent);
		lt_fprintf(file, "}");
	}	break;

	case LT_JSON_STRING:
		lt_fprintf(file, "\"%S\"", json->str_val);
		break;

	case LT_JSON_NUMBER: case LT_JSON_BOOL: case LT_JSON_NULL:
		lt_fprintf(file, "%S", json->str_val);
		break;
	}
}

void lt_json_print(lt_file_t* file, const lt_json_t json[static 1]) {
	json_print_recursive(file, json, 0);
	lt_fprintf(file, "\n");
}

lt_json_t* lt_json_find_child(lt_json_t json[static 1], lstr_t key) {
	LT_ASSERT(json->stype == LT_JSON_OBJECT);
	if (json->stype != LT_JSON_OBJECT)
		return NULL;

	lt_json_t* it = json->child;
	while (it) {
		if (lt_lseq(it->key, key))
			return it;
		it = it->next;
	}
	return NULL;
}

const lt_json_t* lt_json_find_const_child(const lt_json_t json[static 1], lstr_t key) {
	LT_ASSERT(json->stype == LT_JSON_OBJECT);
	if (json->stype != LT_JSON_OBJECT)
		return NULL;

	const lt_json_t* it = json->child;
	while (it) {
		if (lt_lseq(it->key, key))
			return it;
		it = it->next;
	}
	return NULL;
}

static LT_INLINE
const lt_json_t* find_child_of_object(const lt_json_t* parent, lstr_t key) {
	if (!parent || parent->stype != LT_JSON_OBJECT) {
		return NULL;
	}
	return lt_json_find_const_child(parent, key);
}

u64 lt_json_uint_or_default(const lt_json_t* parent, lstr_t key, u64 otherwise) {
	const lt_json_t* child = find_child_of_object(parent, key);
	u64 u;
	if (!child || child->stype != LT_JSON_NUMBER || lt_lstou(child->str_val, &u) != LT_SUCCESS) {
		return otherwise;
	}
	return u;
}

i64 lt_json_int_or_default(const lt_json_t* parent, lstr_t key, i64 otherwise) {
	const lt_json_t* child = find_child_of_object(parent, key);
	i64 i;
	if (!child || child->stype != LT_JSON_NUMBER || lt_lstoi(child->str_val, &i) != LT_SUCCESS) {
		return otherwise;
	}
	return i;
}

f64 lt_json_float_or_default(const lt_json_t* parent, lstr_t key, f64 otherwise) {
	const lt_json_t* child = find_child_of_object(parent, key);
	f64 f;
	if (!child || child->stype != LT_JSON_NUMBER || lt_lstof(child->str_val, &f) != LT_SUCCESS) {
		return otherwise;
	}
	return f;
}

b8 lt_json_bool_or_default(const lt_json_t* parent, lstr_t key, b8 otherwise) {
	const lt_json_t* child = find_child_of_object(parent, key);
	if (!child || child->stype != LT_JSON_BOOL) {
		return otherwise;
	}
	return lt_lseq(child->str_val, CLSTR("true"));
}

lstr_t lt_json_str_or_default(const lt_json_t* parent, lstr_t key, lstr_t otherwise) {
	const lt_json_t* child = find_child_of_object(parent, key);
	if (!child || child->stype != LT_JSON_STRING) {
		return otherwise;
	}
	return child->str_val;
}

lstr_t lt_json_str_val_or_default(const lt_json_t* parent, lstr_t key, lstr_t otherwise) {
	const lt_json_t* child = find_child_of_object(parent, key);
	if (!child) {
		return otherwise;
	}
	return child->str_val;
}

lstr_t lt_json_escape_str(lstr_t src, lt_alloc_t alloc[static 1]) {
	lt_strstream_t ss;
	LT_ASSERT(lt_strstream_create(&ss, alloc) == LT_SUCCESS);

	char* it = src.str, *end = it + src.len;
	while (it < end) {
		char c = *it++;
		switch (c) {
		case '\"': lt_strstream_writels(&ss, CLSTR("\\\"")); break;
		case '\\': lt_strstream_writels(&ss, CLSTR("\\\\")); break;
		case '/': lt_strstream_writels(&ss, CLSTR("\\/")); break;
		case '\b': lt_strstream_writels(&ss, CLSTR("\\b")); break;
		case '\f': lt_strstream_writels(&ss, CLSTR("\\f")); break;
		case '\n': lt_strstream_writels(&ss, CLSTR("\\n")); break;
		case '\r': lt_strstream_writels(&ss, CLSTR("\\r")); break;
		case '\t': lt_strstream_writels(&ss, CLSTR("\\t")); break;
		case '\v': lt_strstream_writels(&ss, CLSTR("\\v")); break;
		default: lt_strstream_writec(&ss, c); break;
		}
	}

	return ss.str;
}

lstr_t lt_json_unescape_str(lstr_t src, lt_alloc_t alloc[static 1]) {
	lt_strstream_t ss;
	LT_ASSERT(lt_strstream_create(&ss, alloc) == LT_SUCCESS);

	char* it = src.str, *end = it + src.len;
	while (it < end) {
		char c = *it++;
		if (c == '\\') {
			char esc = *it++;
			switch (esc) {
			case '\"': lt_strstream_writec(&ss, '\"'); break;
			case '\\': lt_strstream_writec(&ss, '\\'); break;
			case '/': lt_strstream_writec(&ss, '/'); break;
			case 'b': lt_strstream_writec(&ss, '\b'); break;
			case 'f': lt_strstream_writec(&ss, '\f'); break;
			case 'n': lt_strstream_writec(&ss, '\n'); break;
			case 'r': lt_strstream_writec(&ss, '\r'); break;
			case 't': lt_strstream_writec(&ss, '\t'); break;
			case 'v': lt_strstream_writec(&ss, '\v'); break;
			default: lt_werrf("unknown escape sequence '\%c'\n", esc); break;
			}
		}
		else
			lt_strstream_writec(&ss, c);
	}

	return ss.str;
}

void lt_json_free(const lt_json_t json[static 1], lt_alloc_t alloc[static 1]) {
	for (const lt_json_t* it = json, *next; it; it = next) {
		if (it->stype == LT_JSON_OBJECT || it->stype == LT_JSON_ARRAY) {
			lt_json_free(it->child, alloc);
		}

		next = it->next;
		lt_mfree(alloc, it);
	}
}
