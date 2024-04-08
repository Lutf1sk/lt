#include <lt/c.h>
#include <lt/str.h>
#include <lt/sort.h>
#include <lt/darr.h>
#include <lt/io.h>
#include <lt/strstream.h>
#include <lt/ctype.h>

#define fail(...) do { lt_aprintf(&cx->err_str, &cx->arena->interf, __VA_ARGS__); cx->err_file = curr_file(cx); cx->err_line = curr_line(cx); return LT_ERR_INVALID_SYNTAX; } while(0)

#define define_is_equal(def, name_) (lt_lseq((def).name, name_))
#define define_is_lesser(def, name_) (lt_lscmp((def).name, name_) < 0)

static
LT_DEFINE_BINARY_SEARCH_FUNC(lt_c_define_t, lstr_t, lookup_define, define_is_lesser, define_is_equal);

static
LT_DEFINE_BINARY_SEARCH_NEAREST_FUNC(lt_c_define_t, lstr_t, lookup_nearest_define, define_is_lesser, define_is_equal);

lt_err_t lt_c_preproc_create(lt_c_preproc_ctx_t* cx, lstr_t* include_dirs, usz include_dir_count, lt_arena_t* arena) {
	memset(cx, 0, sizeof(lt_c_preproc_ctx_t));
	cx->arena = arena;
	cx->defines = lt_darr_create(lt_c_define_t, 512, &arena->interf);
	if (!cx->defines)
		return LT_ERR_OUT_OF_MEMORY;
	cx->include_dirs = include_dirs;
	cx->include_dir_count = include_dir_count;
	cx->template = NULL;
	cx->scopes = lt_darr_create(lt_c_preproc_scope_t, 64, &arena->interf);
	cx->disallowed = lt_darr_create(lstr_t, 64, &arena->interf);

	return LT_SUCCESS;
}

lt_err_t lt_c_define(lt_c_preproc_ctx_t* cx, lstr_t name, lstr_t value, lt_darr(lstr_t) params, u8 flags) {
	value = lt_lstrim(value);
	name = lt_lstrim(name);

	usz def_count = lt_darr_count(cx->defines);
	lt_c_define_t* at = lookup_nearest_define(def_count, cx->defines, name);

	if (at < cx->defines + def_count && lt_lseq(at->name, name)) {
// 		if (!lt_lseq(at->value, value))
// 			return LT_ERR_REDEFINED;
// 		if ((!!at->params != !!params))
// 			return LT_ERR_REDEFINED;
		if (!params)
			return LT_SUCCESS;
// 		if (lt_darr_count(at->params) != lt_darr_count(params))
// 			return LT_ERR_REDEFINED;
// 		for (usz i = 0; i < lt_darr_count(params); ++i) {
// 			if (!lt_lseq(params[i], at->params[i]))
// 				return LT_ERR_REDEFINED;
// 		}
		return LT_SUCCESS;
	}

	lt_c_define_t new_define;
	new_define.name = name;
	new_define.value = value;
	new_define.params = params;
	new_define.flags = flags;

	usz idx = at - cx->defines;
	lt_darr_insert(cx->defines, idx, &new_define, 1);
	return LT_SUCCESS;
}

lt_err_t lt_c_redefine(lt_c_preproc_ctx_t* cx, lstr_t name, lstr_t value, lt_darr(lstr_t) params, u8 flags) {
	value = lt_lstrim(value);
	name = lt_lstrim(name);

	usz def_count = lt_darr_count(cx->defines);
	lt_c_define_t* at = lookup_nearest_define(def_count, cx->defines, name);

	if (at < cx->defines + def_count && lt_lseq(at->name, name)) {
		at->name = name;
		at->value = value;
		at->params = params;
		at->flags = flags;
		return LT_SUCCESS;
	}

	lt_c_define_t new_define;
	new_define.name = name;
	new_define.value = value;
	new_define.params = params;
	new_define.flags = flags;

	usz idx = at - cx->defines;
	lt_darr_insert(cx->defines, idx, &new_define, 1);
	return LT_SUCCESS;
}

lt_err_t lt_c_undefine(lt_c_preproc_ctx_t* cx, lstr_t name) {
	lt_c_define_t* at = lookup_define(lt_darr_count(cx->defines), cx->defines, name);
	if (!at)
		return LT_SUCCESS;

	usz idx = at - cx->defines;
	lt_darr_erase(cx->defines, idx, 1);
	return LT_SUCCESS;
}

lt_c_define_t* lt_c_lookup_define(lt_c_preproc_ctx_t* cx, lstr_t name) {
	return lookup_define(lt_darr_count(cx->defines), cx->defines, name);
}

static b8 str_pending_bounded(lt_c_preproc_ctx_t* cx, lstr_t str);
static b8 str_pending(lt_c_preproc_ctx_t* cx, lstr_t str);

static
lstr_t curr_file(lt_c_preproc_ctx_t* cx) {
	if (cx->template)
		return cx->template->in_file;
	return cx->file_path;
}

static
u32 curr_line(lt_c_preproc_ctx_t* cx) {
	if (cx->template)
		return cx->template->line;
	return cx->line;
}

static
b8 is_disallowed(lt_c_preproc_ctx_t* cx, lstr_t name) {
	lt_c_preproc_template_t* it = cx->template;
	while (it) {
		if (lt_lseq(it->from_ident, name))
			return 1;
		it = it->prev;
	}
	return 0;
}

static
void unravel_consumed(lt_c_preproc_ctx_t* cx) {
	while (cx->template) {
		if (cx->template->it < cx->template->end)
			return;

		lt_c_preproc_template_t* templ = cx->template;
		cx->template = cx->template->prev;

		free(templ);
	}
}

static
b8 eos(lt_c_preproc_ctx_t* cx) {
	unravel_consumed(cx);
	return cx->it >= cx->end;
}

static
u32 read_bounded(lt_c_preproc_ctx_t* cx) {
	if (cx->template) {
		if (cx->template->it >= cx->template->end)
			return 0;
		return *cx->template->it;
	}
	if (cx->it >= cx->end)
		return 0;
	return *cx->it;
}

static
u32 read(lt_c_preproc_ctx_t* cx) {
	unravel_consumed(cx);
	return read_bounded(cx);
}

static lt_err_t emitc(lt_c_preproc_ctx_t* cx, u32 c);

static
u32 consume_bounded(lt_c_preproc_ctx_t* cx) {
	u32 ret = read_bounded(cx);
	if (cx->template) {
		++cx->template->it;
		if (ret == '\n')
			cx->template->line++;
		return ret;
	}
	++cx->it;
	if (ret == '\n')
		cx->line++;
	return ret;
}

static
u32 consume(lt_c_preproc_ctx_t* cx) {
	unravel_consumed(cx);
	return consume_bounded(cx);
}

static
void consume_and_emit_whitespace(lt_c_preproc_ctx_t* cx) {
	for (;;) {
		u32 c = read(cx);
		if (!lt_is_space(c))
			return;
		emitc(cx, c);
		consume_bounded(cx);
	}
}

static
void consume_whitespace(lt_c_preproc_ctx_t* cx) {
	for (;;) {
		u32 c = read(cx);
		if (!lt_is_space(c))
			return;
		consume_bounded(cx);
	}
}

static
void consume_nonlb_whitespace(lt_c_preproc_ctx_t* cx) {
	for (;;) {
		u32 c = read(cx);
		if (c == '\\') {
			consume_bounded(cx);
			LT_ASSERT(consume(cx) == '\n'); // !!
			continue;
		}
		if (c == '\n' || !lt_is_space(c))
			return;
		consume_bounded(cx);
	}
}

static LT_INLINE
char* pos_bounded(lt_c_preproc_ctx_t* cx) {
	if (cx->template)
		return cx->template->it;
	return cx->it;
}

static
lt_err_t consume_comment(lt_c_preproc_ctx_t* cx) {
	LT_ASSERT(consume_bounded(cx) == '/');
	LT_ASSERT(consume_bounded(cx) == '/');

	for (;;) {
		u32 c = consume(cx);
		if (!c || c == '\n')
			return LT_SUCCESS;
	}
}

static
lt_err_t consume_multiline_comment(lt_c_preproc_ctx_t* cx) {
	LT_ASSERT(consume_bounded(cx) == '/');
	LT_ASSERT(consume_bounded(cx) == '*');

	for (;;) {
		u32 c = consume(cx);
		if (!c)
			fail("unterminated multiline comment");
		if (c == '*' && read(cx) == '/') {
			consume_bounded(cx);
			return LT_SUCCESS;
		}
	}
}

static
lt_err_t consume_line(lt_c_preproc_ctx_t* cx, lstr_t* out) {
	lt_err_t err;

	unravel_consumed(cx);
	char* start = pos_bounded(cx);

	for (;;) {
		if (str_pending_bounded(cx, CLSTR("//"))) {
			if ((err = consume_comment(cx)))
				return err;
			break;
		}
		if (str_pending_bounded(cx, CLSTR("/*"))) {
			if ((err = consume_multiline_comment(cx)))
				return err;
			continue;
		}

		u32 c = consume_bounded(cx);

		if (c == '\\') { // !! terrible way of doing this
			if (consume_bounded(cx) != '\n')
				fail("stray '\\'");
			pos_bounded(cx)[-2] = ' ';
			pos_bounded(cx)[-1] = ' ';
		}

		if (c == '\'') {
			for (;;) {
				u32 c = consume(cx);
				if (!c)
					fail("unterminated character literal");
				else if (c == '\\')
					consume(cx);
				else if (c == '\'')
					break;
			}
		}
		else if (c == '"') {
			for (;;) {
				u32 c = consume(cx);
				if (!c)
					fail("unterminated string literal");
				else if (c == '\\')
					consume(cx);
				else if (c == '"')
					break;
			}
		}
		else if (!c || c == '\n')
			break;
	}

	*out = lt_lsfrom_range(start, pos_bounded(cx));
	return LT_SUCCESS;
}

static
lt_err_t emitc(lt_c_preproc_ctx_t* cx, u32 c) {
	return lt_strstream_writec(cx->ss, c);
}

static
lt_err_t emitls(lt_c_preproc_ctx_t* cx, lstr_t ls) {
	return lt_strstream_writels(cx->ss, ls);
}

static
lt_err_t consume_identifier(lt_c_preproc_ctx_t* cx, lstr_t* out) {
	unravel_consumed(cx);
	char* start = pos_bounded(cx);

	u32 c = consume_bounded(cx);
	if (!lt_is_ident_head(c))
		fail("unexpected character '%c', expected identifier", c);

	for (;;) {
		c = read_bounded(cx);
		if (!lt_is_ident_body(c)) {
			*out = lt_lsfrom_range(start, pos_bounded(cx));
			return LT_SUCCESS;
		}
		consume_bounded(cx);
	}
}

static LT_INLINE
b8 str_pending_bounded(lt_c_preproc_ctx_t* cx, lstr_t str) {
	if (cx->template)
		return lt_lsprefix(lt_lsfrom_range(cx->template->it, cx->template->end), str);
	return lt_lsprefix(lt_lsfrom_range(cx->it, cx->end), str);
}

static
b8 str_pending(lt_c_preproc_ctx_t* cx, lstr_t str) {
	unravel_consumed(cx);
	return str_pending_bounded(cx, str);
}

static
void push_func_expansion(lt_c_preproc_ctx_t* cx, lstr_t ident, lt_darr(lstr_t) params, lt_darr(lstr_t) args, lstr_t str) {
	lt_c_preproc_template_t* templ = malloc(sizeof(lt_c_preproc_template_t));
	LT_ASSERT(templ);
	*templ = (lt_c_preproc_template_t) {
			.in_file = curr_file(cx),
			.line = curr_line(cx),
			.from_ident = ident,
			.params = params,
			.args = args,
			.it = str.str,
			.end = str.str + str.len,
			.prev = cx->template };
	cx->template = templ;
}

static
void push_file_expansion(lt_c_preproc_ctx_t* cx, lstr_t file_path, lstr_t str) {
	lt_c_preproc_template_t* templ = malloc(sizeof(lt_c_preproc_template_t));
	LT_ASSERT(templ);
	*templ = (lt_c_preproc_template_t) {
			.in_file = file_path,
			.it = str.str,
			.end = str.str + str.len,
			.prev = cx->template };
	cx->template = templ;
}

static
void push_expansion(lt_c_preproc_ctx_t* cx, lstr_t ident, lstr_t str) {
	push_func_expansion(cx, ident, NULL, NULL, str);
}

static lt_err_t expand_token(lt_c_preproc_ctx_t* cx);

static
lt_err_t expand_args(lt_c_preproc_ctx_t* cx, lt_c_define_t* def, lt_darr(lstr_t)* out) {
	lt_err_t err;
	LT_ASSERT(consume(cx) == '(');

	u32 variadic = def->flags & LT_CDEF_VARIADIC;
	usz arg_start = cx->ss->str.len;

	lt_c_preproc_template_t* templ = cx->template;

	usz scopes = 1;
	for (;;) {
		u32 c = read(cx);
		if (!c)
			fail("expected ')' after macro parameters");

		if (cx->template == templ && scopes == 1 && c == ',' && !(variadic && lt_darr_count(*out) + 1 >= lt_darr_count(def->params))) {
			lstr_t arg = LSTR((char*)arg_start, cx->ss->str.len - arg_start);
			lt_darr_push(*out, arg);

			consume_bounded(cx);
			emitc(cx, ',');

			arg_start = cx->ss->str.len;
			continue;
		}

		scopes += c == '(';
		scopes -= c == ')';
		if (!scopes)
			break;

		if ((err = expand_token(cx)))
			return err;
	}
	consume_bounded(cx);

	usz param_count = lt_darr_count(def->params);
	usz arg_count = lt_darr_count(*out);
	lstr_t arg = LSTR((char*)arg_start, cx->ss->str.len - arg_start);

	if (arg.len || arg_count || (param_count == 1 && arg_count == 0) || (variadic && arg_count + 1 >= param_count)) {
		lt_darr_push(*out, arg);
		arg_count++;
	}

	if (arg_count != param_count)
		fail("macro '%S' expected %uz arguments, got %uz", def->name, param_count, arg_count);

	for (usz i = 0; i < arg_count; ++i)
		(*out)[i].str += (usz)cx->ss->str.str;

	return LT_SUCCESS;
}

static
lt_err_t expand_identifier(lt_c_preproc_ctx_t* cx) {
	lt_err_t err;

	lstr_t ident;
	if ((err = consume_identifier(cx, &ident)))
		return err;

	if (cx->is_condition && lt_lseq(ident, CLSTR("defined"))) {
		consume_whitespace(cx);

		b8 paren = 0;
		if (read(cx) == '(') {
			consume_bounded(cx);
			paren = 1;
			consume_whitespace(cx);
		}

		lstr_t ident;
		if ((err = consume_identifier(cx, &ident)))
			return err;

		if (paren) {
			consume_whitespace(cx);
			if (consume(cx) != ')')
				fail("expected ')' after parenthesized 'defined' operator");
		}

		b8 defined = lt_c_lookup_define(cx, ident) != NULL;
		emitc(cx, '0' + defined);
		return LT_SUCCESS;
	}

	if (cx->template && cx->template->params) {
		for (usz i = 0; i < lt_darr_count(cx->template->params); ++i) {
			if (lt_lseq(ident, cx->template->params[i])) {
				push_expansion(cx, ident, cx->template->args[i]);
				return LT_SUCCESS;
			}
		}
	}

	lt_c_define_t* def = lt_c_lookup_define(cx, ident);
	if (!def || is_disallowed(cx, ident)) {
		emitls(cx, ident);
		return LT_SUCCESS;
	}

	if (def->params != NULL) {
		consume_whitespace(cx);
		if (read(cx) != '(') {
			emitls(cx, ident);
			if (lt_is_ident_body(read(cx)))
				emitc(cx, ' ');
			return LT_SUCCESS;
		}

		lt_strstream_t ss;
		lt_strstream_create(&ss, &cx->arena->interf);
		lt_darr(lstr_t) args = lt_darr_create(lstr_t, 8, &cx->arena->interf);

		lt_strstream_t* prev_ss = cx->ss;
		cx->ss = &ss;
		if ((err = expand_args(cx, def, &args)))
			return err;
		cx->ss = prev_ss;

		push_func_expansion(cx, ident, def->params, args, def->value);
		return LT_SUCCESS;
	}

	push_expansion(cx, ident, def->value);
	return LT_SUCCESS;
}

static
lt_err_t expand_line(lt_c_preproc_ctx_t* cx) {
	lt_err_t err;

	for (;;) {
		u32 c = read(cx);

		if (c == '\\') {
			consume_bounded(cx);
			if (consume_bounded(cx) != '\n')
				fail("stray '\\'");
			consume_bounded(cx);
			continue;
		}

		if (!c || c == '\n') {
			emitc(cx, '\n');
			consume_bounded(cx);
			return LT_SUCCESS;
		}

		if (str_pending(cx, CLSTR("//"))) {
			if ((err = consume_comment(cx)))
				return err;
			return LT_SUCCESS;
		}

		if ((err = expand_token(cx)))
			return err;
	}
}

static
lt_err_t expand_token(lt_c_preproc_ctx_t* cx) {
	lt_err_t err;

	u32 c = read(cx);
	if (lt_is_ident_head(c)) {
		if ((err = expand_identifier(cx)))
			return err;
	}
	else if (c == '"') {
		emitc(cx, '\"');
		consume_bounded(cx);
		for (;;) {
			u32 c = consume(cx);
			if (!c)
				fail("unterminated string literal");
			emitc(cx, c);

			if (c == '\\')
				emitc(cx, consume(cx));
			else if (c == '"')
				break;
		}
	}
	else if (c == '\'') {
		emitc(cx, '\'');
		consume_bounded(cx);
		for (;;) {
			u32 c = consume(cx);
			if (!c)
				fail("unterminated character literal");
			emitc(cx, c);

			if (c == '\\')
				emitc(cx, consume(cx));
			else if (c == '\'')
				break;
		}
	}
	else if (str_pending(cx, CLSTR("//"))) {
		if ((err = consume_comment(cx)))
			return err;
	}
	else if (str_pending(cx, CLSTR("/*"))) {
		if ((err = consume_multiline_comment(cx)))
			return err;
	}
	else if (str_pending(cx, CLSTR("##"))) { // !! not implemented correctly
		consume(cx);
		consume(cx);

		while (cx->ss->str.len > 0 && lt_is_space(cx->ss->str.str[cx->ss->str.len - 1]))
			--cx->ss->str.len;
		consume_whitespace(cx);
	}
	else {
		consume_bounded(cx);
		emitc(cx, c);
	}
	return LT_SUCCESS;
}

static lt_err_t eval_binary(lt_c_preproc_ctx_t* cx, usz max_precedence, char**it, char* end, i64* out);

static
lt_err_t eval_char(lt_c_preproc_ctx_t* cx, char** it, char* end, i64* out) {
	char* start = ++*it;
	for (;;) {
		if (*it >= end)
			fail("unterminated character literal");

		char c = **it;
		if (c == '\'')
			break;

		++*it;
		if (c == '\\')
			++*it;
	}
	lstr_t literal = lt_lsfrom_range(start, *it);
	++*it;

	lt_err_t err = lt_c_char_literal_value(literal, out, &cx->err_str, &cx->arena->interf);
	if (err) {
		cx->err_file = curr_file(cx);
		cx->err_line = curr_line(cx);
		return err;
	}

	return LT_SUCCESS;
}

static
lt_err_t eval_unary(lt_c_preproc_ctx_t* cx, char**it, char* end, i64* out) {
	lt_err_t err;

	while (*it < end && lt_is_space(**it))
		++*it;
	if (*it >= end)
		fail("unexpected end-of-line, expected constant expression");

	i64 v;

	if (lt_is_numeric_head(**it)) {
		char* start = *it;
		while (*it < end && lt_is_numeric_body(**it))
			++*it;
		lstr_t num = lt_lsfrom_range(start, *it);
		if (lt_lssuffix(num, CLSTR("L")))
			--num.len;

		if (lt_lsprefix(num, CLSTR("0x"))) {
			num.str += 2;
			num.len -= 2;
			if ((err = lt_lshextou(num, &v)))
				fail("invalid integer literal '%S'", num);
		}
		else if ((err = lt_lstoi(num, &v)))
			fail("invalid integer literal '%S'", num);
		*out = v;
		return LT_SUCCESS;
	}

	if (lt_is_ident_head(**it)) {
		char* start = *it;
		while (*it < end && lt_is_ident_body(**it))
			++*it;
		lstr_t ident = lt_lsfrom_range(start, *it);

		if (lt_lseq(ident, CLSTR("L")) && *it < end && **it == '\'')
			return eval_char(cx, it, end, out);
		*out = 0;
		return LT_SUCCESS;
	}

	if (**it == '\'')
		return eval_char(cx, it, end, out);

	char op = *(*it)++;
	switch (op) {
	case '+': return eval_unary(cx, it, end, out);
	case '-':
		if ((err = eval_unary(cx, it, end, &v)))
			return err;
		*out = -v;
		return LT_SUCCESS;

	case '!':
		if ((err = eval_unary(cx, it, end, &v)))
			return err;
		*out = !v;
		return LT_SUCCESS;

	case '~':
		if ((err = eval_unary(cx, it, end, &v)))
			return err;
		*out = ~v;
		return LT_SUCCESS;

	case '(':
		if ((err = eval_binary(cx, 99999, it, end, &v)))
			return err;
		*out = v;
		if (*it >= end || *(*it)++ != ')')
			fail("expected ')' after parenthesized expression");
		return LT_SUCCESS;

	default:
		fail("unexpected symbol '%c', expected unary operator", op);
	}
}

#define OP_TERNARY 1
#define OP_ADD 2
#define OP_SUB 3
#define OP_MUL 4
#define OP_DIV 5
#define OP_MOD 6
#define OP_AND 7
#define OP_LAND 8
#define OP_OR 9
#define OP_LOR 10
#define OP_XOR 11
#define OP_LT 12
#define OP_LEQ 13
#define OP_SHL 14
#define OP_GT 15
#define OP_GEQ 16
#define OP_SHR 17
#define OP_EQ 18
#define OP_NEQ 19

static
lt_err_t eval_binary(lt_c_preproc_ctx_t* cx, usz max_precedence, char**it, char* end, i64* out) {
	lt_err_t err;

	i64 l;
	if ((err = eval_unary(cx, it, end, &l)))
		return err;

	while (*it < end) {
		while (*it < end && lt_is_space(**it))
			++*it;
		if (*it >= end || **it == ')' || **it == ':')
			break;

		usz op, precd;
		char* op_start = *it;
		char opc = *(*it)++;
		switch (opc) {
		case '?': op = OP_TERNARY; precd = 13; break;
		case '+': op = OP_ADD; precd = 4; break;
		case '-': op = OP_SUB; precd = 4; break;
		case '*': op = OP_MUL; precd = 3; break;
		case '/': op = OP_DIV; precd = 3; break;
		case '%': op = OP_MOD; precd = 3; break;

		case '&':
			if (**it == '&') {
				++*it;
				op = OP_LAND;
				precd = 11;
				break;
			}
			op = OP_AND;
			precd = 8;
			break;

		case '|':
			if (**it == '|') {
				++*it;
				op = OP_LOR;
				precd = 12;
				break;
			}
			op = OP_OR;
			precd = 10;
			break;

		case '^': op = OP_XOR; precd = 9; break;

		case '<':
			if (**it == '<') {
				++*it;
				op = OP_SHL;
				precd = 5;
				break;
			}
			if (**it == '=') {
				++*it;
				op = OP_LEQ;
				precd = 6;
				break;
			}
			op = OP_LT;
			precd = 6;
			break;

		case '>':
			if (**it == '>') {
				++*it;
				op = OP_SHR;
				precd = 5;
				break;
			}
			if (**it == '=') {
				++*it;
				op = OP_GEQ;
				precd = 6;
				break;
			}
			op = OP_GT;
			precd = 6;
			break;

		case '=':
			if (**it == '=') {
				++*it;
				op = OP_EQ;
				precd = 7;
				break;
			}
			fail("operator '=', is not constant", opc);

		case '!':
			if (**it == '=') {
				++*it;
				op = OP_NEQ;
				precd = 7;
				break;
			}

		default:
			fail("unexpected symbol '%c', expected binary operator", opc);
		}

		if (precd > max_precedence) {
			*it = op_start;
			break;
		}

		i64 r;
		if ((err = eval_binary(cx, precd, it, end, &r)))
			return err;

		switch (op) {
		case OP_TERNARY: {
			i64 on_true = r, on_false;
			while (*it < end && lt_is_space(**it))
				++*it;
			if (*it >= end || **it != ':')
				fail("expected ':' after ternary expression");
			++*it;

			if ((err = eval_binary(cx, precd, it, end, &on_false)))
				return err;

			if (l)
				l = on_true;
			else
				l = on_false;
		}	break;

		case OP_ADD: l = l + r; break;
		case OP_SUB: l = l - r; break;
		case OP_MUL: l = l * r; break;
		case OP_DIV: l = l / r; break; // !! check for division by zero
		case OP_MOD: l = l % r; break; // !! check for division by zero

		case OP_AND: l = l & r; break;
		case OP_LAND: l = l && r; break;
		case OP_OR: l = l | r; break;
		case OP_LOR: l = l || r; break;
		case OP_XOR: l = l ^ r; break;

		case OP_LT: l = l < r; break;
		case OP_LEQ: l = l <= r; break;
		case OP_SHL: l = l << r; break;
		case OP_GT: l = l > r; break;
		case OP_GEQ: l = l >= r; break;
		case OP_SHR: l = l >> r; break;

		case OP_EQ: l = l == r; break;
		case OP_NEQ: l = l != r; break;

		default:
			fail("unexpected symbol '%c', expected binary operator", op);
		}
	}

	*out = l;
	return LT_SUCCESS;
}

static
lt_err_t eval_condition(lt_c_preproc_ctx_t* cx, b8* out) {
	lt_err_t err;

	lt_strstream_t ss;
	lt_strstream_create(&ss, &cx->arena->interf);

	lt_strstream_t* prev_ss = cx->ss;
	cx->ss = &ss;
	cx->is_condition = 1;
	if ((err = expand_line(cx)))
		return err;
	cx->is_condition = 0;
	cx->ss = prev_ss;

	char* it = ss.str.str, *end = it + ss.str.len;

	i64 v;
	if ((err = eval_binary(cx, 999999, &it, end, &v)))
		return err;
	*out = !!v;
	return LT_SUCCESS;
}

static LT_INLINE
lt_c_preproc_scope_t* curr_scope(lt_c_preproc_ctx_t* cx) {
	if (lt_darr_count(cx->scopes) <= 0)
		return NULL;
	return cx->scopes + lt_darr_count(cx->scopes) - 1;
}

static
b8 is_dead(lt_c_preproc_ctx_t* cx) {
	lt_c_preproc_scope_t* scope = curr_scope(cx);
	if (!scope)
		return 0;
	return scope->dead || !scope->condition;
}

static
void change_condition(lt_c_preproc_ctx_t* cx, b8 condition) {
	lt_c_preproc_scope_t* scope = curr_scope(cx);
	if (!scope)
		LT_ASSERT_NOT_REACHED();

	if (scope->condition)
		scope->dead = 1;
	scope->condition = condition;
}

static LT_INLINE
void push_scope(lt_c_preproc_ctx_t* cx, b8 condition) {
	b8 dead = is_dead(cx);
	lt_darr_push(cx->scopes, (lt_c_preproc_scope_t){ .condition = condition, .dead = dead });
}

static LT_INLINE
void pop_scope(lt_c_preproc_ctx_t* cx) {
	lt_darr_pop(cx->scopes);
}

static LT_INLINE
usz count_scopes(lt_c_preproc_ctx_t* cx) {
	return lt_darr_count(cx->scopes);
}

static
lt_err_t parse_define(lt_c_preproc_ctx_t* cx) {
	lt_err_t err;

	lt_darr(lstr_t) params = NULL;
	u8 flags = 0;

	lstr_t name;
	if ((err = consume_identifier(cx, &name)))
		fail("expected a valid identifier after '#define'");

	u32 c;

	if (!(c = read(cx)) || c != '(') {
		lstr_t replace;
		if ((err = consume_line(cx, &replace)))
			return err;
		if ((err = lt_c_define(cx, name, replace, NULL, 0))) {
			LT_ASSERT(err == LT_ERR_REDEFINED);
			fail("multiple definitions of '%S'", name);
		}
		return LT_SUCCESS;
	}
	consume_bounded(cx);

	b8 comma = 0;
	params = lt_darr_create(lstr_t, 8, &cx->arena->interf);

	while (read(cx) != ')') {
		consume_nonlb_whitespace(cx);

		c = read(cx);
		if (!c && c == ')')
			break;

		if (comma) {
			if (consume(cx) != ',')
				fail("expected ',' or ')' after macro parameter");
			consume_nonlb_whitespace(cx);
		}

		lstr_t param = NLSTR();

		if (lt_is_ident_head(read(cx))) {
			if ((err = consume_identifier(cx, &param)))
				return err;
		}

		consume_nonlb_whitespace(cx);
		if (read(cx) == '.') {
			if (consume(cx) != '.' || consume(cx) != '.' || consume(cx) != '.')
				fail("expected ',' or ')' after macro parameter");

			if (param.len == 0) {
				flags |= LT_CDEF_VARIADIC_SEPARATE;
				param = CLSTR("__VA_ARGS__");
			}
			else
				flags |= LT_CDEF_VARIADIC_LAST;

			lt_darr_push(params, param);
			break;
		}

		if (param.len == 0)
			fail("expected a valid identifier as macro parameter name");

		lt_darr_push(params, param);
		comma = 1;
	}

	consume_nonlb_whitespace(cx);
	c = consume(cx);
	if (c != ')')
		fail("expected ')' after macro parameters");

	lstr_t replace;
	if ((err = consume_line(cx, &replace)))
		return err;
	if ((err = lt_c_define(cx, name, replace, params, flags))) {
		LT_ASSERT(err == LT_ERR_REDEFINED);
		fail("multiple definitions of '%S'", name);
	}

	return LT_SUCCESS;
}

static
lt_err_t parse_include(lt_c_preproc_ctx_t* cx, lstr_t cwd) {
	lt_err_t err;

	lstr_t str;

	consume_nonlb_whitespace(cx);
	if (read(cx) == '<') {
		if ((err = consume_line(cx, &str)))
			return err;
	}
	else {
		lt_strstream_t ss;
		lt_strstream_create(&ss, &cx->arena->interf);

		lt_strstream_t* prev_ss = cx->ss;
		cx->ss = &ss;
		if ((err = expand_line(cx)))
			return err;
		cx->ss = prev_ss;

		str = ss.str;
	}

	// determine the root path of the include from the first '"' or '<'
	char end_delim = 0;
	lstr_t* dirs = cx->include_dirs;
	usz dir_count = cx->include_dir_count;

	char* it = str.str;
	char* end = it + str.len;

	for (;;) {
		if (it >= end)
			fail("expected a path after '#include'");
		if (!lt_is_space(*it))
			break;
		++it;
	}

	if (*it == '"') {
		end_delim = '"';
		dirs = &cwd;
		dir_count = 1;
	}
	else if (*it == '<') {
		end_delim = '>';
	}
	else
		fail("expected '<' or '\"' after '#include'");
	++it;
	char* path_start = it;
	while (*it != end_delim) {
		if (++it >= end)
			fail("missing closing '%c' in included path", end_delim);
	}
	lstr_t file_path = lt_lsfrom_range(path_start, it);

	// search through corresponding include directories until the file is found
	for (usz i = 0; i < dir_count; ++i) {
		lstr_t dir = dirs[i];

		char* fslash = "";
		if (dir.len > 0 && dir.str[dir.len - 1] != '/')
			fslash = "/";

		lstr_t dir_path = lt_lsbuild(&cx->arena->interf, "%S%s%S", dir, fslash, file_path);
		lstr_t raw_file;
		if ((err = lt_freadallp(dir_path, &raw_file, &cx->arena->interf)))
			continue;

		push_file_expansion(cx, file_path, raw_file);
		return LT_SUCCESS;
	}
	fail("failed to include '%S'", file_path);
}

static
lt_err_t parse_directive(lt_c_preproc_ctx_t* cx, lt_strstream_t* ss) {
	lt_err_t err;

	u32 c = consume(cx);
	if (c != '#')
		fail("expected '#' before preprocessor directive");


	consume_nonlb_whitespace(cx);
	if (read(cx) == '\n') {
		consume_bounded(cx);
		return LT_SUCCESS;
	}
	lstr_t directive;
	if ((err = consume_identifier(cx, &directive)))
		return err;
	consume_nonlb_whitespace(cx);

	if (lt_lseq(directive, CLSTR("if"))) {
		b8 val = 0;
		if (!is_dead(cx) && (err = eval_condition(cx, &val)))
			return err;
		push_scope(cx, val);
	}

	else if (lt_lseq(directive, CLSTR("ifdef"))) {
		lstr_t name;
		if ((err = consume_identifier(cx, &name)))
			return err;
		lt_c_define_t* def = lookup_define(lt_darr_count(cx->defines), cx->defines, name);
		push_scope(cx, def != NULL);
	}

	else if (lt_lseq(directive, CLSTR("ifndef"))) {
		lstr_t name;
		if ((err = consume_identifier(cx, &name)))
			return err;
		lt_c_define_t* def = lookup_define(lt_darr_count(cx->defines), cx->defines, name);
		push_scope(cx, def == NULL);
	}

	else if (lt_lseq(directive, CLSTR("elif"))) {
		if (curr_scope(cx) == NULL)
			fail("'elif' directive with no previous condition");

		b8 val = 0;
		if (!curr_scope(cx)->dead && (err = eval_condition(cx, &val)))
			return err;
		change_condition(cx, val);
	}

	else if (lt_lseq(directive, CLSTR("else"))) {
		if (curr_scope(cx) == NULL)
			fail("'else' directive with no previous condition");

		change_condition(cx, !curr_scope(cx)->condition);
	}

	else if (lt_lseq(directive, CLSTR("endif"))) {
		if (curr_scope(cx) == NULL)
			fail("'endif' directive with no previous condition");
		pop_scope(cx);
	}

	else if (!is_dead(cx) && lt_lseq(directive, CLSTR("define"))) {
		if ((err = parse_define(cx)))
			return err;
	}

	else if (!is_dead(cx) && lt_lseq(directive, CLSTR("undef"))) {
		lstr_t name;
		if ((err = consume_identifier(cx, &name)))
			return err;
		if ((err = lt_c_undefine(cx, name))) {
			LT_ASSERT_NOT_REACHED();
			return err;
		}
	}

	else if (!is_dead(cx) && lt_lseq(directive, CLSTR("include"))) {
		if ((err = parse_include(cx, lt_lsdirname(cx->file_path))))
			return err;
	}

	else if (!is_dead(cx) && lt_lseq(directive, CLSTR("error"))) {
		lstr_t str;
		if ((err = consume_line(cx, &str)))
			return err;
		if (str.len > 0 && str.str[str.len - 1] == '\n')
			str.len--;
		fail("%S", str);
	}

	else if (!is_dead(cx) && lt_lseq(directive, CLSTR("warning"))) {
		// !!
		lstr_t str;
		if ((err = consume_line(cx, &str)))
			return err;
	}

	else if (!is_dead(cx) && lt_lseq(directive, CLSTR("pragma"))) {
		// !!
		lstr_t str;
		if ((err = consume_line(cx, &str)))
			return err;
	}
	else if (!is_dead(cx) && lt_lseq(directive, CLSTR("line"))) {
		// !!
		lstr_t str;
		if ((err = consume_line(cx, &str)))
			return err;
	}

	else if (!is_dead(cx))
		fail("unknown preprocessor directive '#%S'", directive);
	else {
		lstr_t str;
		if ((err = consume_line(cx, &str)))
			return err;
	}

	return LT_SUCCESS;
}

lt_err_t lt_c_preproc(lt_c_preproc_ctx_t* cx, void* data, usz size, lstr_t file_path, lstr_t* out) {
	lt_err_t err;
	cx->line = 0;
	cx->file_path = file_path;
	cx->template = NULL;

	lt_strstream_t ss;
	if ((err = lt_strstream_create(&ss, lt_libc_heap)))
		return err;

	cx->ss = &ss;

	cx->it = data;
	cx->end = cx->it + size;

	char* line_str = lt_amalloc_lean(cx->arena, 10); // 10 = max number of digits in a u32

	while (!eos(cx)) {
		isz line_str_len = lt_sprintf(line_str, "%ud", cx->line);
		lt_c_redefine(cx, CLSTR("__LINE__"), LSTR(line_str, line_str_len), NULL, 0);
		lt_c_redefine(cx, CLSTR("__FILE__"), cx->file_path, NULL, 0);

		consume_and_emit_whitespace(cx);

		u32 c = read(cx);
		if (c != '#') {
			if (!is_dead(cx)) {
				if ((err = expand_line(cx)))
					return err;
			}
			else {
				lstr_t str;
				if ((err = consume_line(cx, &str)))
					return err;
			}
			continue;
		}

		if ((err = parse_directive(cx, &ss)))
			return err;
	}

	*out = ss.str;
	return LT_SUCCESS;
}
