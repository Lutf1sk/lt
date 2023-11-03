#include <lt/c.h>
#include <lt/str.h>
#include <lt/sort.h>
#include <lt/darr.h>
#include <lt/io.h>
#include <lt/strstream.h>
#include <lt/ctype.h>

#define fail(e, ...) do { lt_aprintf(&cx->err_str, &cx->arena->interf, __VA_ARGS__); cx->err_line = cx->line; return (e); } while(0)

#define define_is_equal(def, name_) (lt_lstr_eq((def).name, name_))
#define define_is_lesser(def, name_) (lt_lstr_cmp((def).name, name_) < 0)

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
	value = lt_lstr_trim(value);
	name = lt_lstr_trim(name);

	usz def_count = lt_darr_count(cx->defines);
	lt_c_define_t* at = lookup_nearest_define(cx->defines, def_count, name);

	// !! missing important parameter checks	
	if (at < cx->defines + def_count && lt_lstr_eq(at->name, name)) {
		if (!lt_lstr_eq(at->value, value))
			fail(LT_ERR_REDEFINED, "attempting to redefine '%S'", name);
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
	value = lt_lstr_trim(value);
	name = lt_lstr_trim(name);

	usz def_count = lt_darr_count(cx->defines);
	lt_c_define_t* at = lookup_nearest_define(cx->defines, def_count, name);

	if (at < cx->defines + def_count && lt_lstr_eq(at->name, name)) {
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
	lt_c_define_t* at = lookup_define(cx->defines, lt_darr_count(cx->defines), name);
	if (!at)
		return LT_SUCCESS;

	usz idx = at - cx->defines;
	lt_darr_erase(cx->defines, idx, 1);
	return LT_SUCCESS;
}

lt_c_define_t* lt_c_lookup_define(lt_c_preproc_ctx_t* cx, lstr_t name) {
	return lookup_define(cx->defines, lt_darr_count(cx->defines), name);
}

static
char template_read(lt_c_preproc_ctx_t* cx) {
	lt_c_preproc_template_t* template = cx->template;

	while (template->it >= template->end) {
		if (!template->prev)
			return 0;
		template = template->prev;
	}
	return *template->it;
}

static
char template_consume(lt_c_preproc_ctx_t* cx, lt_strstream_t* ss) {
	while (cx->template->it >= cx->template->end) {
		if (!cx->template->prev)
			return 0;
		cx->template = cx->template->prev;
	}
	char c = *cx->template->it++;
	if (ss)
		lt_strstream_writec(ss, c);
	return c;
}

static
b8 template_consume_identifier(lt_c_preproc_ctx_t* cx, lt_strstream_t* ss) {
	if (!lt_is_ident_head(template_read(cx)))
		return 0;
	template_consume(cx, ss);
	while (lt_is_ident_body(template_read(cx)))
		template_consume(cx, ss);
	return 1;
}

static
void template_consume_whitespace(lt_c_preproc_ctx_t* cx) {
	while (lt_is_space(template_read(cx)))
		template_consume(cx, NULL);
}

static
b8 is_disallowed(lt_c_preproc_ctx_t* cx, lstr_t name) {
	for (usz i = 0; i < lt_darr_count(cx->disallowed); ++i) {
		if (lt_lstr_eq(cx->disallowed[i], name))
			return 1;
	}
	return 0;
}

static LT_INLINE
void push_disallowed(lt_c_preproc_ctx_t* cx, lstr_t name) {
	lt_darr_push(cx->disallowed, name);
}

static LT_INLINE
void pop_disallowed(lt_c_preproc_ctx_t* cx) {
	lt_darr_pop(cx->disallowed);
}

static
lt_err_t write_escaped_str(lstr_t str, lt_strstream_t* ss) {
	lt_strstream_writec(ss, '"');

	for (char* it = str.str, *end = it + str.len; it < end; ++it) {
		switch (*it) {
		case '\n': lt_strstream_writels(ss, CLSTR("\\n")); break;
		case '"': lt_strstream_writels(ss, CLSTR("\\\"")); break;
		default: lt_strstream_writec(ss, *it); break;
		}
	}

	lt_strstream_writec(ss, '"');
	return LT_SUCCESS;
}

static
lt_err_t gen_template(lt_c_preproc_ctx_t* cx, lstr_t str, lt_darr(lstr_t) params, lt_darr(lstr_t) args, lt_strstream_t* ss) {
	lt_err_t err;

	lt_c_preproc_template_t template;
	template.it = str.str;
	template.end = str.str + str.len;
	template.params = params;
	template.args = args;
	template.prev = cx->template;

	cx->template = &template;

	lt_strstream_t is;
	if ((err = lt_strstream_create(&is, &cx->arena->interf)))
		fail(err, "failed to create string stream");

	isz prefix_start = -1;

	while (template.it < template.end || cx->template->it < cx->template->end) {
	next:
		if (prefix_start != -1) {
			char* pstart = &ss->str.str[prefix_start];
			char* pend = ss->str.str + ss->str.len;

			usz len = pend - pstart;

			char* str = lt_amalloc_lean(cx->arena, len);
			memcpy(str, pstart, len);

			lt_c_preproc_template_t* templ = lt_amalloc_lean(cx->arena, sizeof(lt_c_preproc_template_t));
			templ->it = str;
			templ->end = str + len;
			templ->params = cx->template->params;
			templ->args = cx->template->args;
			templ->prev = cx->template;

			cx->template = templ;

			ss->str.len = prefix_start;
			prefix_start = -1;
			continue;
		}

		char fc = template_read(cx);

		if (fc == '#') {
			if (template_read(cx) != '#') {
				lt_strstream_clear(&is);
				template_consume_identifier(cx, &is);
				lstr_t ident = is.str;

				if (!cx->template->params)
					fail(LT_ERR_INVALID_SYNTAX, "stray '#'", ident);

				for (usz i = 0; i < lt_darr_count(cx->template->params); ++i) {
					if (!lt_lstr_eq(ident, cx->template->params[i]))
						continue;

					if ((err = write_escaped_str(cx->template->args[i], ss)))
						return err;
					goto next;
				}

				fail(LT_ERR_INVALID_SYNTAX, "'%S' is not a macro argument", ident);
			}
			else {
				template_consume(cx, NULL);

				isz i = ss->str.len;
				while (i > 0 && lt_is_space(ss->str.str[i - 1]))
					--i;

				isz i_end = i;
				while (i > 0 && lt_is_ident_body(ss->str.str[i - 1]))
					--i;

				prefix_start = i;
				ss->str.len = i_end;

				template_consume(cx, NULL);
				template_consume_whitespace(cx);
			}
		}
		else if (fc == '"') {
			template_consume(cx, ss);
			do {
				if (fc == '\\')
					template_consume(cx, ss);
				fc = template_consume(cx, ss);
				if (fc == 0)
					fail(LT_ERR_INVALID_SYNTAX, "expected '\"' after string literal");
			} while (fc != '"');
		}
		else if (fc == '\'') {
			template_consume(cx, ss);
			do {
				if (fc == '\\')
					template_consume(cx, ss);
				fc = template_consume(cx, ss);
				if (fc == 0)
					fail(LT_ERR_INVALID_SYNTAX, "expected \' after character literal");
			} while (fc != '\'');
		}
		else if (!lt_is_ident_head(fc)) {
			template_consume(cx, ss);
			continue;
		}

		lt_strstream_clear(&is);
		template_consume_identifier(cx, &is);
		lstr_t ident = is.str;

		if (cx->template->params) {
			for (usz i = 0; i < lt_darr_count(cx->template->params); ++i) {
				if (!lt_lstr_eq(ident, cx->template->params[i]))
					continue;

				if ((err = gen_template(cx, cx->template->args[i], NULL, NULL, ss)))
					return err;
				goto next;
			}
		}

		lt_c_define_t* def = NULL;
		if (!is_disallowed(cx, ident))
			def = lookup_define(cx->defines, lt_darr_count(cx->defines), ident);

		if (!def) {
			lt_strstream_writels(ss, ident);
			continue;
		}

		if (!def->params) {
			push_disallowed(cx, def->name);
			if ((err = gen_template(cx, def->value, NULL, NULL, ss)))
				return err;
			pop_disallowed(cx);
			continue;
		}

		template_consume_whitespace(cx);
		if (template_read(cx) != '(') {
			lt_strstream_writels(ss, ident);
			continue;
		}

		lt_strstream_t as;
		if ((err = lt_strstream_create(&as, &cx->arena->interf)))
			fail(err, "failed to create string stream");

		char c;
		usz scopes = 0;
		do {
			if (!(c = template_consume(cx, &as)))
				fail(LT_ERR_INVALID_SYNTAX, "expected ')' after macro arguments");
			scopes += c == '(';
			scopes -= c == ')';
		} while (scopes);
		lstr_t arg_template = as.str;
		arg_template.str++;
		arg_template.len -= 2;

		if ((err = lt_strstream_create(&as, &cx->arena->interf)))
			fail(err, "failed to create string stream");

		lt_c_preproc_template_t* templ_old = cx->template;
		cx->template = NULL;
		if ((err = gen_template(cx, arg_template, templ_old->params, templ_old->args, &as)))
			return err;
		cx->template = templ_old;

		lt_darr(lstr_t) gen_args = lt_darr_create(lstr_t, 8, &cx->arena->interf);
		lt_darr(lstr_t) gen_params = def->params;

		char* it = as.str.str, *end = it + as.str.len, *arg_start = it;
		while (it < end) {
			if (*it == '(') {
				usz scopes = 0;
				do {
					if (it >= end)
						fail(LT_ERR_INVALID_SYNTAX, "expected ')' in macro argument");
					scopes += *it == '(';
					scopes -= *it == ')';
					++it;
				} while (scopes > 0);
			}

			if ((def->flags & LT_CDEF_VARIADIC) && lt_darr_count(gen_args) + 1 >= lt_darr_count(gen_params)) {
				it = end;
				break;
			}

			if (*it == ',') {
				lt_darr_push(gen_args, lt_lstr_trim(lt_lstr_from_range(arg_start, it)));
				arg_start = ++it;
			}
			else
				++it;
		}
		lstr_t arg = lt_lstr_from_range(arg_start, it);
		if ((lt_darr_count(gen_params) == 1 && !lt_darr_count(gen_args)) || arg.len || lt_darr_count(gen_args) || ((def->flags & LT_CDEF_VARIADIC) && lt_darr_count(gen_args) + 1 >= lt_darr_count(gen_params))) {
			lt_darr_push(gen_args, lt_lstr_trim(lt_lstr_from_range(arg_start, it)));
		}

		if (lt_darr_count(gen_params) != lt_darr_count(gen_args))
			fail(LT_ERR_INVALID_SYNTAX, "invalid number of arguments to macro '%S', expected %uz, got %uz", def->name, lt_darr_count(gen_params), lt_darr_count(gen_args));

		push_disallowed(cx, def->name);
		if ((err = gen_template(cx, def->value, gen_params, gen_args, ss)))
			return err;
		pop_disallowed(cx);
	}

	cx->template = template.prev;
	return LT_SUCCESS;
}

lt_err_t eval_binary(lt_c_preproc_ctx_t* cx, char** it, char* end, i64* out);

lt_err_t eval_unary(lt_c_preproc_ctx_t* cx, char** it, char* end, i64* out) {
	lt_err_t err;

	while (*it < end && lt_is_space(**it))
		++*it;

	i64 v;

	if (**it == '!') {
		++*it;
		if ((err = eval_unary(cx, it, end, &v)))
			return err;
		*out = !v;
	}
	else if (**it == '(') {
		++*it;
		if ((err = eval_binary(cx, it, end, &v)))
			return err;
		while (*it < end && **it != ')')
			++*it;
		if (*it >= end)
			fail(LT_ERR_INVALID_SYNTAX, "expected ')' after parenthesized expression, got 'end-of-file'");
		if (**it != ')')
			fail(LT_ERR_INVALID_SYNTAX, "expected ')' after parenthesized expression, got '%c'", **it);
		++*it;
		*out = v;
	}
	else if (**it == '~') {
		++*it;
		if ((err = eval_unary(cx, it, end, &v)))
			return err;
		*out = ~v;
	}
	else if (**it == '-') {
		++*it;
		if ((err = eval_unary(cx, it, end, &v)))
			return err;
		*out = -v;
	}
	else if (lt_is_numeric_head(**it)) {
		char* start = (*it)++;
		while (*it < end && lt_is_numeric_body(**it))
			++*it;

		lstr_t num = lt_lstr_from_range(start, *it);

		if (num.len > 0 && num.str[num.len - 1] == 'L')
			num.len--;

		if ((err = lt_lstr_int(num, &v)))
			fail(LT_ERR_INVALID_SYNTAX, "invalid integer");
		*out = v;
	}
	else if (lt_is_ident_head(**it)) {
		char* start = (*it)++;
		while (*it < end && lt_is_ident_body(**it))
			++*it;
		lstr_t ident0 = lt_lstr_from_range(start, *it);

		if (!lt_lstr_eq(ident0, CLSTR("defined"))) {
			lt_c_define_t* def = lookup_define(cx->defines, lt_darr_count(cx->defines), ident0);
			if (is_disallowed(cx, ident0) || !def) {
				*out = 0;
				return LT_SUCCESS;
			}

			lstr_t template = ident0;

			if (def->params) {
				while (*it < end && lt_is_space(**it))
					++*it;

				if (*it >= end || **it != '(')
					fail(LT_ERR_INVALID_SYNTAX, "expected '(' after function macro");

				usz scopes = 0;
				do {
					if (*it >= end)
						fail(LT_ERR_INVALID_SYNTAX, "expected ')' after macro arguments");
					scopes += **it == '(';
					scopes -= **it == ')';
					++*it;
				} while (scopes > 0);

				template = lt_lstr_from_range(start, *it);
			}

			lt_strstream_t ss;
			if ((err = lt_strstream_create(&ss, &cx->arena->interf)))
				fail(err, "failed to create string stream");

			if ((err = gen_template(cx, template, NULL, NULL, &ss)))
				return err;

			char* new_it = ss.str.str, *new_end = new_it + ss.str.len;
			if ((err = eval_binary(cx, &new_it, new_end, &v)))
				return err;
			*out = v;

			return LT_SUCCESS;
		}
		else {
			while (*it < end && lt_is_space(**it))
				++*it;

			u8 paren = 0;
			if (*it < end && **it == '(') {
				paren = 1;
				++*it;
				while (*it < end && lt_is_space(**it))
					++*it;
			}

			start = *it;
			while (*it < end && lt_is_ident_body(**it))
				++*it;
			lstr_t ident1 = lt_lstr_from_range(start, *it);
			if (ident1.len <= 0)
				fail(LT_ERR_INVALID_SYNTAX, "expected a valid identifier after 'defined'");

			while (*it < end && lt_is_space(**it))
				++*it;

			if (paren) {
				if (*it >= end || **it != ')')
					fail(LT_ERR_INVALID_SYNTAX, "expected ')' after parenthesized 'defined' operator");
				++*it;
			}

			lt_c_define_t* def = lookup_define(cx->defines, lt_darr_count(cx->defines), ident1);
			*out = def != NULL;
			return LT_SUCCESS;
		}
	}
	else
		fail(LT_ERR_INVALID_SYNTAX, "unexpected '%c'", **it);

	return LT_SUCCESS;
}

lt_err_t eval_binary(lt_c_preproc_ctx_t* cx, char** it, char* end, i64* out) {
	lt_err_t err;

	i64 l;
	if ((err = eval_unary(cx, it, end, &l)))
		return err;

	for (;;) {
		while (*it < end && lt_is_space(**it))
			++*it;

		if (*it >= end) {
			*out = l;
			return LT_SUCCESS;
		}

		i64 r;

		if (**it == '&') {
			++*it;
			if (*it < end && **it == '&') {
				++*it;
				if ((err = eval_unary(cx, it, end, &r)))
					return err;
				l = l && r;
			}
			else {
				if ((err = eval_unary(cx, it, end, &r)))
					return err;
				l &= r;
			}
		}

		else if (**it == '|') {
			++*it;
			if (*it < end && **it == '|') {
				++*it;
				if ((err = eval_unary(cx, it, end, &r)))
					return err;
				l = l || r;
			}
			else {
				if ((err = eval_unary(cx, it, end, &r)))
					return err;
				l |= r;
			}
		}

		else if (**it == '<') {
			++*it;
			if (*it < end && **it == '<') {
				++*it;
				if ((err = eval_unary(cx, it, end, &r)))
					return err;
				l <<= r;
			}
			else if (*it < end && **it == '=') {
				++*it;
				if ((err = eval_unary(cx, it, end, &r)))
					return err;
				l = l <= r;
			}
			else {
				if ((err = eval_unary(cx, it, end, &r)))
					return err;
				l = l < r;
			}
		}

		else if (**it == '>') {
			++*it;
			if (*it < end && **it == '>') {
				++*it;
				if ((err = eval_unary(cx, it, end, &r)))
					return err;
				l >>= r;
			}
			else if (*it < end && **it == '=') {
				++*it;
				if ((err = eval_unary(cx, it, end, &r)))
					return err;
				l = l >= r;
			}
			else {
				if ((err = eval_unary(cx, it, end, &r)))
					return err;
				l = l > r;
			}
		}

		else if (**it == '=') {
			++*it;
			if (*it >= end || **it != '=') {
				fail(LT_ERR_INVALID_SYNTAX, "expected a second '=' after constant assignment operator, got '%c'", **it);
			}

			++*it;
			if ((err = eval_unary(cx, it, end, &r)))
				return err;
			l = l == r;
		}

		else if (**it == '!') {
			++*it;
			if (*it >= end || **it != '=') {
				fail(LT_ERR_INVALID_SYNTAX, "expected '=' after binary '!', got '%c'", **it);
			}

			++*it;
			if ((err = eval_unary(cx, it, end, &r)))
				return err;
			l = l != r;
		}

		else if (**it == '+') {
			++*it;
			if ((err = eval_unary(cx, it, end, &r)))
				return err;
			l += r;
		}

		else if (**it == '-') {
			++*it;
			if ((err = eval_unary(cx, it, end, &r)))
				return err;
			l -= r;
		}

		else if (**it == '*') {
			++*it;
			if ((err = eval_unary(cx, it, end, &r)))
				return err;
			l *= r;
		}

		else if (**it == '/') {
			++*it;
			if ((err = eval_unary(cx, it, end, &r)))
				return err;
			l /= r;
		}

		else if (**it == '%') {
			++*it;
			if ((err = eval_unary(cx, it, end, &r)))
				return err;
			l %= r;
		}

		else if (**it == '?') {
			++*it;
			if (l) {
				if ((err = eval_unary(cx, it, end, &r)))
					return err;
				l = r;
				while (*it < end && **it != ':')
					++*it;
				++*it;
				if ((err = eval_unary(cx, it, end, &r)))
					return err;
			}
			else {
				if ((err = eval_unary(cx, it, end, &r)))
					return err;
				while (*it < end && **it != ':')
					++*it;
				++*it;
				if ((err = eval_unary(cx, it, end, &r)))
					return err;
				l = r;
			}
		}

		else {
			*out = l;
			return LT_SUCCESS;
		}
	}
}

lt_err_t eval_condition(lt_c_preproc_ctx_t* cx, char** it, char* end, b8* out) {
	lt_err_t err;

	i64 v;
	if ((err = eval_binary(cx, it, end, &v)))
		return err;
	*out = !!v;
	return LT_SUCCESS;
}

static
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

static
void push_scope(lt_c_preproc_ctx_t* cx, b8 condition) {
	b8 dead = is_dead(cx);
	lt_darr_push(cx->scopes, (lt_c_preproc_scope_t){ .condition = condition, .dead = dead });
}

static
void pop_scope(lt_c_preproc_ctx_t* cx) {
	lt_darr_pop(cx->scopes);
}

static
usz count_scopes(lt_c_preproc_ctx_t* cx) {
	return lt_darr_count(cx->scopes);
}

static LT_INLINE
char consume(char** it, char* end) {
	if (*it >= end)
		return 0;
	return *(*it)++;
}

static LT_INLINE
char read(char** it, char* end) {
	if (*it >= end)
		return 0;
	return **it;
}

static LT_INLINE
void consume_whitespace(char** it, char* end) {
	while (*it < end && lt_is_space(**it))
		++*it;
}

static
lstr_t consume_identifier(char** it, char* end) {
	if (!lt_is_ident_head(**it))
		return NLSTR();

	char* start = *it;
	while (*it < end && lt_is_ident_body(**it))
		++*it;
	return lt_lstr_from_range(start, *it);
}

static
lt_err_t filter_special(lt_c_preproc_ctx_t* cx, lstr_t str, lt_strstream_t* ss) {
	char* it = str.str, *end = it + str.len;

	b8 comment = 0;

	while (it < end) {
		char c = *it++;

		if (it < end) {
			if (c == '/' && *it == '*') {
				comment = 1;
				++it;
				continue;
			}
			else if (c == '*' && *it == '/') {
				comment = 0;
				++it;
				continue;
			}
			else if (!comment && c == '/' && *it == '/') {
				while (it < end && *it++ != '\n')
					;
				continue;
			}
		}

		if (comment)
			continue;

		if (c == '"') {
			lt_strstream_writec(ss, '"');
			while (it < end) {
				if (c != '\\' && *it == '"')
					break;
				c = *it++;
				lt_strstream_writec(ss, c);
			}
			if (it >= end)
				fail(LT_ERR_INVALID_SYNTAX, "expected '\"' after string literal");
			lt_strstream_writec(ss, '"');
			++it;
			continue;
		}

		if (c == '\'') {
			lt_strstream_writec(ss, '\'');
			while (it < end) {
				if (c != '\\' && *it == '\'')
					break;
				c = *it++;
				lt_strstream_writec(ss, c);
			}
			if (it >= end)
				fail(LT_ERR_INVALID_SYNTAX, "expected \' after character literal");
			lt_strstream_writec(ss, '\'');
			++it;
			continue;
		}

		if (c != '\\') {
			lt_strstream_writec(ss, c);
			continue;
		}

		c = *it++;
		if (c != '\n')
			fail(LT_ERR_INVALID_SYNTAX, "stray '\\'");
	}

	return LT_SUCCESS;
}

static
lt_err_t parse_define(lt_c_preproc_ctx_t* cx, char* start, char* end) {
	lt_err_t err;

	char* it = start;

	lt_darr(lstr_t) params = NULL;
	u8 flags = 0;

	lstr_t name = consume_identifier(&it, end);
	if (name.len == 0)
		fail(LT_ERR_INVALID_SYNTAX, "expected a valid identifier after '#define'");

	if (it >= end || *it != '(') {
		lstr_t replace = lt_lstr_from_range(it, end);
		if ((err = lt_c_define(cx, name, replace, NULL, 0)))
			return err;
		return LT_SUCCESS;
	}
	++it;

	b8 comma = 0;
	params = lt_darr_create(lstr_t, 8, &cx->arena->interf);

	while (it < end && *it != ')') {
		consume_whitespace(&it, end);

		if (it >= end || *it == ')')
			break;

		if (comma) {
			if (consume(&it, end) != ',')
				fail(LT_ERR_INVALID_SYNTAX, "expected ',' or ')' after macro parameter");
			consume_whitespace(&it, end);
		}

		lstr_t param = consume_identifier(&it, end);

		consume_whitespace(&it, end);
		if (read(&it, end) == '.') {
			if (consume(&it, end) != '.' || consume(&it, end) != '.' || consume(&it, end) != '.')
				fail(LT_ERR_INVALID_SYNTAX, "expected either ',' or ')' after macro parameter");

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
			fail(LT_ERR_INVALID_SYNTAX, "expected a valid identifier as macro parameter name");

		lt_darr_push(params, param);
		comma = 1;
	}

	consume_whitespace(&it, end);
	if (it >= end || *it != ')')
		fail(LT_ERR_INVALID_SYNTAX, "expected ')' after macro parameters");
	++it;

	lstr_t replace = lt_lstr_from_range(it, end);

	if ((err = lt_c_define(cx, name, replace, params, flags)))
		return err;

	return LT_SUCCESS;
}

static
lt_err_t parse_include(lt_c_preproc_ctx_t* cx, char* start, char* end, lstr_t cwd, lstr_t* out) {
	lt_err_t err;

	lt_strstream_t ss;
	lt_strstream_create(&ss, &cx->arena->interf);
	if ((err = gen_template(cx, lt_lstr_from_range(start, end), NULL, NULL, &ss)))
		return err;

	// determine the root path of the include from the first '"' or '<'
	char end_delim = 0;
	lstr_t* dirs = cx->include_dirs;
	usz dir_count = cx->include_dir_count;

	char* it = start;
	consume_whitespace(&it, end);
	if (it >= end)
		fail(LT_ERR_INVALID_SYNTAX, "expected a valid path after '#include'");

	if (*it == '"') {
		end_delim = '"';
		dirs = &cwd;
		dir_count = 1;
	}
	else if (*it == '<') {
		end_delim = '>';
	}
	else
		fail(LT_ERR_INVALID_SYNTAX, "expected '<' or '\"' after '#include'");
	++it;
	char* path_start = it;
	while (*it != end_delim) {
		if (++it >= end)
			fail(LT_ERR_INVALID_SYNTAX, "missing closing '%c' in included path", end_delim);
	}
	lstr_t file_path = lt_lstr_from_range(path_start, it);

	// search through corresponding include directories until the file is found
	for (usz i = 0; i < dir_count; ++i) {
		lstr_t dir = dirs[i];

		char* fslash = "";
		if (dir.len > 0 && dir.str[dir.len - 1] != '/')
			fslash = "/";

		lstr_t dir_path = lt_lstr_build(&cx->arena->interf, "%S%s%S", dir, fslash, file_path);
		lstr_t raw_file;
		if ((err = lt_file_read_entire(dir_path, &raw_file, &cx->arena->interf)))
			continue;

		lstr_t old_file = cx->file_path;
		usz old_line = cx->line;
		if ((err = lt_c_preproc(cx, raw_file.str, raw_file.len, dir_path, out)))
			return err;
		cx->file_path = old_file;
		cx->line = old_line;
		return LT_SUCCESS;
	}
	fail(LT_ERR_NOT_FOUND, "failed to include '%S'", file_path);
}

static
lt_err_t parse_directive(lt_c_preproc_ctx_t* cx, lstr_t str, lt_strstream_t* ss) {
	lt_err_t err;
	char* it = str.str, *end = it + str.len;

	consume_whitespace(&it, end);
	if (it >= end || *it++ != '#')
		fail(LT_ERR_INVALID_SYNTAX, "expected '#' before preprocessor directive");

	consume_whitespace(&it, end);
	lstr_t directive = consume_identifier(&it, end);
	consume_whitespace(&it, end);

	if (lt_lstr_eq(directive, CLSTR("if"))) {
		b8 val = 0;
		if (!is_dead(cx) && (err = eval_condition(cx, &it, end, &val)))
			return err;
		push_scope(cx, val);
	}

	else if (lt_lstr_eq(directive, CLSTR("ifdef"))) {
		lstr_t name = consume_identifier(&it, end);
		lt_c_define_t* def = lookup_define(cx->defines, lt_darr_count(cx->defines), name);
		push_scope(cx, def != NULL);
	}

	else if (lt_lstr_eq(directive, CLSTR("ifndef"))) {
		lstr_t name = consume_identifier(&it, end);
		lt_c_define_t* def = lookup_define(cx->defines, lt_darr_count(cx->defines), name);
		push_scope(cx, def == NULL);
	}

	else if (lt_lstr_eq(directive, CLSTR("elif"))) {
		if (curr_scope(cx) == NULL)
			fail(LT_ERR_INVALID_SYNTAX, "'elif' directive with no previous condition");

		b8 val = 0;
		if (!curr_scope(cx)->dead && (err = eval_condition(cx, &it, end, &val)))
			return err;
		change_condition(cx, val);
	}

	else if (lt_lstr_eq(directive, CLSTR("else"))) {
		if (curr_scope(cx) == NULL)
			fail(LT_ERR_INVALID_SYNTAX, "'else' directive with no previous condition");

		change_condition(cx, !curr_scope(cx)->condition);
	}

	else if (lt_lstr_eq(directive, CLSTR("endif"))) {
		if (curr_scope(cx) == NULL)
			fail(LT_ERR_INVALID_SYNTAX, "'endif' directive with no previous condition");
		pop_scope(cx);
	}

	else if (!is_dead(cx) && lt_lstr_eq(directive, CLSTR("define"))) {
		if ((err = parse_define(cx, it, end)))
			return err;
	}

	else if (!is_dead(cx) && lt_lstr_eq(directive, CLSTR("undef"))) {
		char* name_start = it;
		while (it < end && lt_is_ident_body(*it))
			++it;
		if ((err = lt_c_undefine(cx, lt_lstr_from_range(name_start, it))))
			return err;
	}

	else if (!is_dead(cx) && lt_lstr_eq(directive, CLSTR("include"))) {
		lstr_t parsed_str;
		if ((err = parse_include(cx, it, end, lt_lstr_path_dir(cx->file_path), &parsed_str)))
			return err;
		lt_strstream_writels(ss, parsed_str);
	}

	else if (!is_dead(cx) && lt_lstr_eq(directive, CLSTR("error"))) {
		fail(LT_ERR_INVALID_SYNTAX, "%S", lt_lstr_from_range(it, end));
	}

	else if (!is_dead(cx) && lt_lstr_eq(directive, CLSTR("warning"))) {
		// !!
	}

	else if (!is_dead(cx) && lt_lstr_eq(directive, CLSTR("pragma"))) {
		// !!
	}

	else if (!is_dead(cx))
		fail(LT_ERR_INVALID_SYNTAX, "unknown preprocessor directive '#%S'", directive);

	return LT_SUCCESS;
}

lt_err_t lt_c_preproc(lt_c_preproc_ctx_t* cx, void* data, usz size, lstr_t file_path, lstr_t* out) {
	lt_err_t err;
	cx->line = 0;
	cx->file_path = file_path;

	lt_strstream_t ss;
	if ((err = lt_strstream_create(&ss, &cx->arena->interf)))
		return err;

	if ((err = filter_special(cx, LSTR(data, size), &ss)))
		return err;

	char* it = ss.str.str;
	char* end = it + ss.str.len;

	if ((err = lt_strstream_create(&ss, &cx->arena->interf)))
		return err;

	while (it < end) {
		char* line_start = it;
		for (;;) {
			if (it >= end) {
				++cx->line;
				break;
			}

			if (*it == '\n') {
				++cx->line;
				++it;
				break;
			}
			++it;
		}
		char* line_end = it;
		lstr_t line = lt_lstr_trim(lt_lstr_from_range(line_start, line_end));
		if (!line.len)
			continue;

		char* line_str = lt_amalloc_lean(cx->arena, 10); // 10 = max number of digits in a u32
		isz line_str_len = lt_sprintf(line_str, "%ud", cx->line);
		lt_c_redefine(cx, CLSTR("__LINE__"), LSTR(line_str, line_str_len), NULL, 0);

		lt_c_redefine(cx, CLSTR("__FILE__"), cx->file_path, NULL, 0);

		if (line.str[0] != '#') {
			if (!is_dead(cx)) {
				if ((err = gen_template(cx, line, NULL, NULL, &ss)))
					return err;
				lt_strstream_writec(&ss, '\n');
			}
			continue;
		}

		if ((err = parse_directive(cx, line, &ss)))
			return err;
	}

	*out = ss.str;
	return LT_SUCCESS;
}
