#include <lt/lpc.h>
#include <lt/io.h>
#include <lt/mem.h>
#include <lt/darr.h>

lstr_t lt_lpc_expr_type_strtab[LT_LPCE_MAX] = {
	[LT_LPCE_INVAL] = CLSTR("invalid"),
	[LT_LPCE_ADD] = CLSTR("add"),
	[LT_LPCE_SUB] = CLSTR("subtract"),
	[LT_LPCE_MUL] = CLSTR("multiply"),
	[LT_LPCE_DIV] = CLSTR("divide"),
	[LT_LPCE_MOD] = CLSTR("modulo"),
	[LT_LPCE_NEG] = CLSTR("negate"),
	[LT_LPCE_OR] = CLSTR("bit-or"),
	[LT_LPCE_AND] = CLSTR("bit-and"),
	[LT_LPCE_XOR] = CLSTR("bit-xor"),
	[LT_LPCE_SHL] = CLSTR("bit-shr"),
	[LT_LPCE_SHR] = CLSTR("bit-shr"),
	[LT_LPCE_INV] = CLSTR("bit-invert"),
	[LT_LPCE_SET] = CLSTR("set"),
	[LT_LPCE_SETADD] = CLSTR("add-and-set"),
	[LT_LPCE_SETSUB] = CLSTR("subtract-and-set"),
	[LT_LPCE_SETMUL] = CLSTR("multiply-and-set"),
	[LT_LPCE_SETDIV] = CLSTR("divide-and-set"),
	[LT_LPCE_SETMOD] = CLSTR("modulo-and-set"),
	[LT_LPCE_SETOR] = CLSTR("bit-or-and-set"),
	[LT_LPCE_SETAND] = CLSTR("bit-and-and-set"),
	[LT_LPCE_SETXOR] = CLSTR("bit-xor-and-set"),
	[LT_LPCE_SETSHL] = CLSTR("bit-shl-and-set"),
	[LT_LPCE_SETSHR] = CLSTR("bit-shr-and-set"),
	[LT_LPCE_LOGEQ] = CLSTR("is-equal"),
	[LT_LPCE_LOGNEQ] = CLSTR("is-not-equal"),
	[LT_LPCE_LOGLEQ] = CLSTR("is-lesser-or-equal"),
	[LT_LPCE_LOGGEQ] = CLSTR("is-greater-or-equal"),
	[LT_LPCE_LOGLESSER] = CLSTR("is-lesser"),
	[LT_LPCE_LOGGREATER] = CLSTR("is-greater"),
	[LT_LPCE_LOGOR] = CLSTR("is-or"),
	[LT_LPCE_LOGAND] = CLSTR("is-and"),
	[LT_LPCE_LOGINV] = CLSTR("is-not"),
	[LT_LPCE_RANGE] = CLSTR("range"),
	[LT_LPCE_IDENT] = CLSTR("identifier"),
	[LT_LPCE_NUM] = CLSTR("number"),
	[LT_LPCE_STR] = CLSTR("string"),
	[LT_LPCE_PROC] = CLSTR("procedure"),
	[LT_LPCE_IF] = CLSTR("if"),
	[LT_LPCE_IMPORT] = CLSTR("import"),
	[LT_LPCE_CALL] = CLSTR("call"),
	[LT_LPCE_DEREF] = CLSTR("dereference"),
	[LT_LPCE_REF] = CLSTR("reference"),
	[LT_LPCE_SUBSCRIPT] = CLSTR("subscript"),
	[LT_LPCE_PTR] = CLSTR("pointer"),
	[LT_LPCE_ARRAY] = CLSTR("array"),
};

lstr_t lt_lpc_stmt_type_strtab[LT_LPCS_MAX] = {
	[LT_LPCS_ASSERT] = CLSTR("assert"),
	[LT_LPCS_INVAL] = CLSTR("invalid"),
	[LT_LPCS_IF] = CLSTR("if"),
	[LT_LPCS_FOR] = CLSTR("for"),
	[LT_LPCS_VAR] = CLSTR("definition"),
	[LT_LPCS_RETURN] = CLSTR("return"),
	[LT_LPCS_IMPORT] = CLSTR("import"),
	[LT_LPCS_SWITCH] = CLSTR("switch"),
	[LT_LPCS_CASE] = CLSTR("case"),
	[LT_LPCS_BREAK] = CLSTR("break"),
	[LT_LPCS_CONTINUE] = CLSTR("continue"),
	[LT_LPCS_DEFER] = CLSTR("defer"),
	[LT_LPCS_EXPR] = CLSTR("expression"),
};

#define LT_ANSI_SHORTEN_NAMES 1
#include <lt/ansi.h>

#define fail_to(lbl, x, fmt, ...) \
	do { \
		x; \
		lt_aprintf(&cx->err_str, &cx->alloc->interf, fmt, __VA_ARGS__); \
		goto lbl; \
	} while (0)

#define fail(err, fmt, ...) \
	do { \
		lt_aprintf(&cx->err_str, &cx->alloc->interf, fmt, __VA_ARGS__); \
		return err; \
	} while (0)

static LT_INLINE
lstr_t str_from_tk(lt_lpc_parse_ctx_t* cx, lt_lpc_tk_t tk) {
	return LSTR(cx->str_base + tk.str_offs, tk.len);
}

static lt_lpc_expr_t* new_expr(lt_arena_t* alloc);
static lt_lpc_stmt_t* new_stmt(lt_arena_t* alloc);

static lt_err_t parse_new_expr(lt_lpc_parse_ctx_t* cx, lt_lpc_expr_t** out);

static lt_err_t parse_new_stmt(lt_lpc_parse_ctx_t* cx, lt_lpc_stmt_t** out);
static lt_err_t parse_new_block(lt_lpc_parse_ctx_t* cx, lt_lpc_stmt_t** out);

static lt_lpc_sym_t** lt_lpc_lookup_nearest_sym(lt_lpc_sym_t** arr, usz count, lstr_t name);
static lt_err_t lt_lpc_insert_global(lt_lpc_parse_ctx_t* cx, lt_lpc_sym_t* sym);

static lt_err_t resolve_stmt(lt_lpc_parse_ctx_t* cx, lt_lpc_stmt_t* stmt);

static
lt_lpc_expr_t* new_expr(lt_arena_t* alloc) {
	lt_lpc_expr_t* new = lt_amalloc_lean(alloc, sizeof(lt_lpc_expr_t));
	if (new)
		memset(new, 0, sizeof(lt_lpc_expr_t));
	return new;
}

void lt_lpc_free_expr(lt_lpc_expr_t* expr, lt_arena_t* alloc) {
	if (expr->child0)
		lt_lpc_free_expr(expr->child0, alloc);
	if (expr->child1)
		lt_lpc_free_expr(expr->child1, alloc);
	if (expr->child2)
		lt_lpc_free_expr(expr->child2, alloc);
	if (expr->next)
		lt_lpc_free_expr(expr->next, alloc);

	if (expr->stmt)
		lt_lpc_free_stmt(expr->stmt, alloc);
}

static
lt_lpc_stmt_t* new_stmt(lt_arena_t* alloc) {
	lt_lpc_stmt_t* new = lt_amalloc_lean(alloc, sizeof(lt_lpc_stmt_t));
	if (new)
		memset(new, 0, sizeof(lt_lpc_stmt_t));
	return new;
}

void lt_lpc_free_stmt(lt_lpc_stmt_t* stmt, lt_arena_t* alloc) {
	if (stmt->next)
		lt_lpc_free_stmt(stmt->next, alloc);
	if (stmt->child0)
		lt_lpc_free_stmt(stmt->child0, alloc);
	if (stmt->child1)
		lt_lpc_free_stmt(stmt->child1, alloc);
	if (stmt->child2)
		lt_lpc_free_stmt(stmt->child2, alloc);

	if (stmt->expr0)
		lt_lpc_free_expr(stmt->expr0, alloc);
	if (stmt->expr1)
		lt_lpc_free_expr(stmt->expr1, alloc);
}


static LT_INLINE
lt_lpc_tk_type_t read_type(lt_lpc_parse_ctx_t* cx) {
	if (cx->it >= cx->end)
		return LT_LPCTK_EOF;
	return cx->it->type;
}

static LT_INLINE
lt_err_t consume(lt_lpc_parse_ctx_t* cx, lt_lpc_tk_t* out) {
	if (cx->it >= cx->end)
		fail(LT_ERR_INVALID_SYNTAX, "unexpected end of file", 0);

	if (out)
		*out = *cx->it;

	++cx->it;
	return LT_SUCCESS;
}

static LT_INLINE
lt_err_t consume_type(lt_lpc_parse_ctx_t* cx, lt_lpc_tk_type_t type, lt_lpc_tk_t* out) {
	if (cx->it >= cx->end)
		fail(LT_ERR_INVALID_SYNTAX, "unexpected end of file", 0);

	if (cx->it->type != type)
		fail(LT_ERR_INVALID_SYNTAX, "unexpected `%S`, expected `%S`", str_from_tk(cx, *cx->it), lt_lpc_tk_type_str(type));

	if (out)
		*out = *cx->it;

	++cx->it;
	return LT_SUCCESS;
}

static
lt_err_t parse_function_def(lt_lpc_parse_ctx_t* cx, void* out) {
	lt_err_t err;

	if ((err = consume_type(cx, LT_LPCTK_KWPROC, NULL)))
		return err;

	if (read_type(cx) == LT_LPCTK_LPAREN) {
		consume(cx, NULL);

		while (read_type(cx) != LT_LPCTK_RPAREN) {
			if ((err = consume_type(cx, LT_LPCTK_IDENT, NULL)))
				return err;
			while (read_type(cx) == LT_LPCTK_COMMA) {
				consume(cx, NULL);
				if ((err = consume_type(cx, LT_LPCTK_IDENT, NULL)))
					return err;
			}

			if ((err = consume_type(cx, LT_LPCTK_COLON, NULL)))
				return err;

			lt_lpc_expr_t* expr;
			if ((err = parse_new_expr(cx, &expr)))
				return err;
		}
		consume(cx, NULL);
	}

	if (read_type(cx) == LT_LPCTK_KWRETURN) {
		consume(cx, NULL);
		lt_lpc_expr_t* expr;
		if ((err = parse_new_expr(cx, &expr)))
			return err;
	}

	return LT_SUCCESS;
}

static
lt_err_t parse_new_expr_primary(lt_lpc_parse_ctx_t* cx, lt_lpc_expr_t** out) {
	lt_err_t err;

	lt_lpc_expr_t* expr = NULL;

	switch (read_type(cx)) {
	case LT_LPCTK_KWIF:
		if (!(expr = new_expr(cx->alloc)))
			fail(LT_ERR_OUT_OF_MEMORY, "out of memory", 0);
		consume(cx, NULL);
		if ((err = parse_new_expr(cx, &expr->child0)))
			goto err0;
		if ((err = consume_type(cx, LT_LPCTK_KWTHEN, NULL)))
			goto err0;
		if ((err = parse_new_expr(cx, &expr->child1)))
			goto err0;
		if ((err = consume_type(cx, LT_LPCTK_KWELSE, NULL)))
			goto err0;
		if ((err = parse_new_expr(cx, &expr->child2)))
			goto err0;
		*out = expr;
		expr->type = LT_LPCE_IF;
		return LT_SUCCESS;

	case LT_LPCTK_IDENT:
		if (!(expr = new_expr(cx->alloc)))
			fail(LT_ERR_OUT_OF_MEMORY, "out of memory", 0);
		consume(cx, NULL);
		expr->type = LT_LPCE_IDENT;
		*out = expr;
		return LT_SUCCESS;

	case LT_LPCTK_NUM:
		if (!(expr = new_expr(cx->alloc)))
			fail(LT_ERR_OUT_OF_MEMORY, "out of memory", 0);
		consume(cx, NULL);
		expr->type = LT_LPCE_NUM;
		*out = expr;
		return LT_SUCCESS;

	case LT_LPCTK_CHAR:
		if (!(expr = new_expr(cx->alloc)))
			fail(LT_ERR_OUT_OF_MEMORY, "out of memory", 0);
		consume(cx, NULL);
		expr->type = LT_LPCE_NUM;
		*out = expr;
		return LT_SUCCESS;

	case LT_LPCTK_STR:
		if (!(expr = new_expr(cx->alloc)))
			fail(LT_ERR_OUT_OF_MEMORY, "out of memory", 0);
		consume(cx, NULL);
		expr->type = LT_LPCE_STR;
		*out = expr;
		return LT_SUCCESS;

	case LT_LPCTK_LPAREN:
		consume(cx, NULL);
		if ((err = parse_new_expr(cx, &expr)))
			return err;
		if ((err = consume_type(cx, LT_LPCTK_RPAREN, NULL)))
			goto err0;
		*out = expr;
		return LT_SUCCESS;

	case LT_LPCTK_KWIMPORT:
		if (!(expr = new_expr(cx->alloc)))
			fail(LT_ERR_OUT_OF_MEMORY, "out of memory", 0);
		consume(cx, NULL);
		if ((err = parse_new_expr(cx, &expr->child0)))
			return err;
		expr->type = LT_LPCE_IMPORT;
		*out = expr;
		return LT_SUCCESS;

	case LT_LPCTK_KWPROC:
		if (!(expr = new_expr(cx->alloc)))
			fail(LT_ERR_OUT_OF_MEMORY, "out of memory", 0);
		parse_function_def(cx, NULL);
		++cx->scope_depth;
		if ((err = consume_type(cx, LT_LPCTK_KWBEGIN, NULL)))
			goto err0;
		if ((err = parse_new_block(cx, &expr->stmt)))
			goto err0;
		if ((err = consume_type(cx, LT_LPCTK_KWEND, NULL)))
			goto err0;
		--cx->scope_depth;
		expr->type = LT_LPCE_PROC;
		*out = expr;
		return LT_SUCCESS;

	default:
		fail(LT_ERR_INVALID_SYNTAX, "unexpected `%S`, expected a valid expression", str_from_tk(cx, *cx->it));
	}

err0:	lt_lpc_free_expr(expr, cx->alloc);
		return err;
}

static
lt_err_t parse_new_expr_unary(lt_lpc_parse_ctx_t* cx, lt_lpc_expr_t** out) {
	lt_err_t err;

	lt_lpc_expr_t* primary = NULL, *postfix, **current = &primary;

	for (;;) {
		switch (read_type(cx)) {
		case LT_LPCTK_MINUS:
			if (!(*current = new_expr(cx->alloc)))
				fail_to(err0, err = LT_ERR_OUT_OF_MEMORY, "out of memory", 0);
			consume(cx, NULL);
			(*current)->type = LT_LPCE_NEG;
			break;

		case LT_LPCTK_EXC:
			if (!(*current = new_expr(cx->alloc)))
				fail_to(err0, err = LT_ERR_OUT_OF_MEMORY, "out of memory", 0);
			consume(cx, NULL);
			(*current)->type = LT_LPCE_LOGINV;
			break;

		case LT_LPCTK_TILDE:
			if (!(*current = new_expr(cx->alloc)))
				fail_to(err0, err = LT_ERR_OUT_OF_MEMORY, "out of memory", 0);
			consume(cx, NULL);
			(*current)->type = LT_LPCE_INV;
			break;

		case LT_LPCTK_ASTER:
			if (!(*current = new_expr(cx->alloc)))
				fail_to(err0, err = LT_ERR_OUT_OF_MEMORY, "out of memory", 0);
			consume(cx, NULL);
			(*current)->type = LT_LPCE_PTR;
			break;

		case LT_LPCTK_LBRACKET:
			if (!(*current = new_expr(cx->alloc)))
				fail_to(err0, err = LT_ERR_OUT_OF_MEMORY, "out of memory", 0);
			consume(cx, NULL);

			if (read_type(cx) != LT_LPCTK_RBRACKET) {
				if ((err = parse_new_expr(cx, &(*current)->child1)))
					goto err0;
			}
			if ((err = consume_type(cx, LT_LPCTK_RBRACKET, NULL)))
				goto err0;
			(*current)->type = LT_LPCE_ARRAY;
			break;

		default:
			goto prefix_done;
		}

		current = &(*current)->child0;
	}

prefix_done:
	if ((err = parse_new_expr_primary(cx, &primary)))
		return err;

	for (;;) {
		switch (read_type(cx)) {
		case LT_LPCTK_LPAREN: {
			if (!(postfix = new_expr(cx->alloc)))
				fail_to(err0, err = LT_ERR_OUT_OF_MEMORY, "out of memory", 0);
			consume(cx, NULL);

			lt_lpc_expr_t** current = &postfix->child1;
			while (read_type(cx) != LT_LPCTK_RPAREN) {
				if (current != &postfix->child1 && (err = consume_type(cx, LT_LPCTK_COMMA, NULL)))
					goto err1;
				if ((err = parse_new_expr(cx, current)))
					goto err1;
				current = &(*current)->next;
			}
			consume(cx, NULL);
			postfix->type = LT_LPCE_CALL;
		}	break;

		case LT_LPCTK_LBRACKET:
			consume(cx, NULL);
			if (!(postfix = new_expr(cx->alloc)))
				fail_to(err0, err = LT_ERR_OUT_OF_MEMORY, "out of memory", 0);
			if ((err = parse_new_expr(cx, &postfix->child1)))
				goto err0;
			if ((err = consume_type(cx, LT_LPCTK_RBRACKET, NULL)))
				goto err0;
			postfix->type = LT_LPCE_SUBSCRIPT;
			break;

		case LT_LPCTK_AMP:
			consume(cx, NULL);
			if (!(postfix = new_expr(cx->alloc)))
				fail_to(err0, err = LT_ERR_OUT_OF_MEMORY, "out of memory", 0);
			postfix->type = LT_LPCE_REF;
			break;

		case LT_LPCTK_ASTER:
			consume(cx, NULL);
			if (!(postfix = new_expr(cx->alloc)))
				fail_to(err0, err = LT_ERR_OUT_OF_MEMORY, "out of memory", 0);
			postfix->type = LT_LPCE_DEREF;
			break;

		default:
			*out = primary;
			return LT_SUCCESS;
		}

		postfix->child0 = primary;
		primary = postfix;
	}

err1:	lt_lpc_free_expr(postfix, cx->alloc);
err0:	if (primary)
			lt_lpc_free_expr(primary, cx->alloc);
		return err;
}

typedef
struct operator {
	isz precedence;
	u8 expr_type;
	u8 tk_type;
	u8 right_to_left;
} operator_t;

static operator_t binary_op_tab[] = {
	{ 5, LT_LPCE_ADD, LT_LPCTK_PLUS, 0 },
	{ 5, LT_LPCE_SUB, LT_LPCTK_MINUS, 0 },
	{ 5, LT_LPCE_DIV, LT_LPCTK_SLASH, 0 },
	{ 5, LT_LPCE_MUL, LT_LPCTK_ASTER, 0 },
	{ 5, LT_LPCE_MOD, LT_LPCTK_PERCENT, 0 },
	{ 5, LT_LPCE_OR, LT_LPCTK_PIPE, 0 },
	{ 5, LT_LPCE_AND, LT_LPCTK_AMP, 0 },
	{ 5, LT_LPCE_XOR, LT_LPCTK_CARET, 0 },
	{ 5, LT_LPCE_SHL, LT_LPCTK_DLESSER, 0 },
	{ 5, LT_LPCE_SHR, LT_LPCTK_DGREATER, 0 },

	{ 9, LT_LPCE_SET, LT_LPCTK_EQ, 1 },
	{ 5, LT_LPCE_SETADD, LT_LPCTK_PLUSEQ, 0 },
	{ 5, LT_LPCE_SETSUB, LT_LPCTK_MINUSEQ, 0 },
	{ 5, LT_LPCE_SETDIV, LT_LPCTK_SLASHEQ, 0 },
	{ 5, LT_LPCE_SETMUL, LT_LPCTK_ASTEREQ, 0 },
	{ 5, LT_LPCE_SETMOD, LT_LPCTK_PERCENTEQ, 0 },
	{ 5, LT_LPCE_SETOR, LT_LPCTK_PIPEEQ, 0 },
	{ 5, LT_LPCE_SETAND, LT_LPCTK_AMPEQ, 0 },
	{ 5, LT_LPCE_SETXOR, LT_LPCTK_CARETEQ, 0 },
	{ 5, LT_LPCE_SETSHL, LT_LPCTK_DLESSEREQ, 0 },
	{ 5, LT_LPCE_SETSHR, LT_LPCTK_DGREATEREQ, 0 },

	{ 5, LT_LPCE_LOGLESSER, LT_LPCTK_LESSER, 0 },
	{ 5, LT_LPCE_LOGGREATER, LT_LPCTK_GREATER, 0 },
	{ 5, LT_LPCE_LOGLEQ, LT_LPCTK_LESSEREQ, 0 },
	{ 5, LT_LPCE_LOGGEQ, LT_LPCTK_LESSEREQ, 0 },
	{ 5, LT_LPCE_LOGEQ, LT_LPCTK_DEQ, 0 },
	{ 5, LT_LPCE_LOGNEQ, LT_LPCTK_EXCEQ, 0 },
	{ 5, LT_LPCE_LOGOR, LT_LPCTK_DPIPE, 0 },
	{ 5, LT_LPCE_LOGAND, LT_LPCTK_DAMP, 0 },

	{ 10, LT_LPCE_RANGE, LT_LPCTK_DDOT, 0 },
};

static
operator_t* find_binary_op(lt_lpc_tk_type_t tk_type) {
	for (usz i = 0; i < sizeof(binary_op_tab) / sizeof(*binary_op_tab); ++i) {
		if (binary_op_tab[i].tk_type == tk_type)
			return &binary_op_tab[i];
	}
	return NULL;
}

static
lt_err_t parse_new_expr_binary(lt_lpc_parse_ctx_t* cx, usz max_precedence, lt_lpc_expr_t** out) {
	lt_err_t err;

	lt_lpc_expr_t* left, *right;

	if ((err = parse_new_expr_unary(cx, &left)))
		return err;

	for (;;) {
		operator_t* op = find_binary_op(read_type(cx));
		if (!op || (!op->right_to_left && op->precedence >= max_precedence) || (op->right_to_left && op->precedence > max_precedence)) {
			*out = left;
			return LT_SUCCESS;
		}

		consume(cx, NULL);
		if ((err = parse_new_expr_binary(cx, op->precedence, &right)))
			goto err0;

		lt_lpc_expr_t* expr = new_expr(cx->alloc);
		if (!expr)
			fail_to(err1, err = LT_ERR_OUT_OF_MEMORY, "out of memory", 0);

		expr->type = op->expr_type;
		expr->child0 = left;
		expr->child1 = right;

		left = expr;
	}

err1:	lt_lpc_free_expr(right, cx->alloc);
err0:	lt_lpc_free_expr(left, cx->alloc);
		return err;
}

static LT_INLINE
lt_err_t parse_new_expr(lt_lpc_parse_ctx_t* cx, lt_lpc_expr_t** out) {
	return parse_new_expr_binary(cx, 999999, out);
}

static LT_INLINE
lt_err_t parse_new_block(lt_lpc_parse_ctx_t* cx, lt_lpc_stmt_t** out) {
	lt_err_t err;

	lt_lpc_stmt_t* root = NULL;
	lt_lpc_stmt_t** current = &root;

	lt_lpc_tk_type_t type = read_type(cx);
	while (type != LT_LPCTK_KWEND && type != LT_LPCTK_KWELSE && type != LT_LPCTK_KWELIF && type != LT_LPCTK_KWCASE) {
		if ((err = parse_new_stmt(cx, current)))
			goto err0;
		current = &(*current)->next;
		type = read_type(cx);
	}

	*out = root;
	return LT_SUCCESS;

err0:	if (root)
			lt_lpc_free_stmt(root, cx->alloc);
		return err;
}

static
lt_err_t parse_new_stmt(lt_lpc_parse_ctx_t* cx, lt_lpc_stmt_t** out) {
	lt_err_t err;

	lt_lpc_stmt_t* stmt = new_stmt(cx->alloc);
	if (!stmt)
		fail(LT_ERR_OUT_OF_MEMORY, "out of memory", 0);

	switch (read_type(cx)) {
	case LT_LPCTK_KWASSERT:
		consume(cx, NULL);
		if ((err = parse_new_expr(cx, &stmt->expr0)))
			goto err0;
		if ((err = consume_type(cx, LT_LPCTK_SEMICOLON, NULL)))
			goto err0;
		stmt->type = LT_LPCS_ASSERT;
		*out = stmt;
		return LT_SUCCESS;

	case LT_LPCTK_KWIF: {
		consume(cx, NULL);
		if ((err = parse_new_expr(cx, &stmt->expr0)))
			goto err0;
		if ((err = consume_type(cx, LT_LPCTK_KWDO, NULL)))
			goto err0;
		if ((err = parse_new_block(cx, &stmt->child0)))
			goto err0;

		lt_lpc_stmt_t** current = &stmt->child1;
		while (read_type(cx) == LT_LPCTK_KWELIF) {
			consume(cx, NULL);
			if (!(*current = new_stmt(cx->alloc)))
				fail_to(err0, err = LT_ERR_OUT_OF_MEMORY, "out of memory", 0);
			(*current)->type = LT_LPCS_IF;
			if ((err = parse_new_expr(cx, &(*current)->expr0)))
				goto err0;
			if ((err = consume_type(cx, LT_LPCTK_KWDO, NULL)))
				goto err0;
			if ((err = parse_new_block(cx, &(*current)->child0)))
				goto err0;
			current = &(*current)->next;
		}

		if (read_type(cx) == LT_LPCTK_KWELSE) {
			consume(cx, NULL);
			if ((err = parse_new_block(cx, &stmt->child2)))
				goto err0;
		}

		if ((err = consume_type(cx, LT_LPCTK_KWEND, NULL)))
			goto err0;

		stmt->type = LT_LPCS_IF;
		*out = stmt;
	}	return LT_SUCCESS;

	case LT_LPCTK_KWSWITCH: {
		consume(cx, NULL);

		if ((err = parse_new_expr(cx, &stmt->expr0)))
			goto err0;

		lt_lpc_stmt_t** current = &stmt->child0;
		while (read_type(cx) == LT_LPCTK_KWCASE) {
			consume(cx, NULL);

			if (!(*current = new_stmt(cx->alloc)))
				fail_to(err0, err = LT_ERR_OUT_OF_MEMORY, "out of memory", 0);
			(*current)->type = LT_LPCS_CASE;

			lt_lpc_expr_t** current_expr = &(*current)->expr0;
			if ((err = parse_new_expr(cx, current_expr)))
				goto err0;
			current_expr = &(*current_expr)->next;
			while (read_type(cx) == LT_LPCTK_COMMA) {
				consume(cx, NULL);
				if ((err = parse_new_expr(cx, current_expr)))
					goto err0;
				current_expr = &(*current_expr)->next;
			}

			if ((err = consume_type(cx, LT_LPCTK_KWDO, NULL)))
				goto err0;
			if ((err = parse_new_block(cx, &(*current)->child0)))
				goto err0;

			current = &(*current)->next;
		}

		if (read_type(cx) == LT_LPCTK_KWELSE) {
			consume(cx, NULL);
			if ((err = parse_new_block(cx, &stmt->child1)))
				goto err0;
		}

		if ((err = consume_type(cx, LT_LPCTK_KWEND, NULL)))
			goto err0;

		stmt->type = LT_LPCS_SWITCH;
		*out = stmt;
	}	return LT_SUCCESS;

	case LT_LPCTK_KWIMPORT:
		consume(cx, NULL);
		if ((err = consume_type(cx, LT_LPCTK_IDENT, NULL)))
			goto err0;
		if ((err = consume_type(cx, LT_LPCTK_SEMICOLON, NULL)))
			goto err0;
		stmt->type = LT_LPCS_IMPORT;
		*out = stmt;
		return LT_SUCCESS;

	case LT_LPCTK_KWRETURN:
		consume(cx, NULL);
		if ((err = parse_new_expr(cx, &stmt->expr0)))
			goto err0;
		if ((err = consume_type(cx, LT_LPCTK_SEMICOLON, NULL)))
			goto err0;
		stmt->type = LT_LPCS_RETURN;
		*out = stmt;
		return LT_SUCCESS;

	case LT_LPCTK_KWBREAK:
		consume(cx, NULL);
		stmt->type = LT_LPCS_BREAK;
		*out = stmt;
		return LT_SUCCESS;

	case LT_LPCTK_KWCONTINUE:
		consume(cx, NULL);
		stmt->type = LT_LPCS_CONTINUE;
		*out = stmt;
		return LT_SUCCESS;

	case LT_LPCTK_KWFOR:
		consume(cx, NULL);

		if (read_type(cx) != LT_LPCTK_KWDO) {
			if ((err = parse_new_expr(cx, &stmt->expr0)))
				goto err0;
			if (read_type(cx) == LT_LPCTK_COLON) {
				consume(cx, NULL);
				if ((err = parse_new_expr(cx, &stmt->expr1)))
					goto err0;
			}
		}
		if ((err = consume_type(cx, LT_LPCTK_KWDO, NULL)))
			goto err0;
		if ((err = parse_new_block(cx, &stmt->child0)))
			goto err0;
		if ((err = consume_type(cx, LT_LPCTK_KWEND, NULL)))
			goto err0;
		stmt->type = LT_LPCS_FOR;
		*out = stmt;
		return LT_SUCCESS;

	case LT_LPCTK_KWDEFER:
		consume(cx, NULL);
		if ((err = parse_new_expr(cx, &stmt->expr0)))
			goto err0;
		if ((err = consume_type(cx, LT_LPCTK_SEMICOLON, NULL)))
			goto err0;
		stmt->type = LT_LPCS_DEFER;
		*out = stmt;
		return LT_SUCCESS;

	case LT_LPCTK_IDENT:
		if (cx->it + 1 < cx->end && (cx->it + 1)->type == LT_LPCTK_COLON) {
			lt_lpc_tk_t ident_tk;
			consume(cx, &ident_tk);
			consume(cx, NULL);

			lt_lpc_tk_type_t type = read_type(cx);
			if (type != LT_LPCTK_COLON && type != LT_LPCTK_EQ && (err = parse_new_expr(cx, &stmt->expr0)))
				goto err0;

			type = read_type(cx);
			if (type == LT_LPCTK_COLON) {
				consume(cx, NULL);
				if ((err = parse_new_expr(cx, &stmt->expr1)))
					goto err0;
			}
			else if (type == LT_LPCTK_EQ) {
				consume(cx, NULL);
				if ((err = parse_new_expr(cx, &stmt->expr1)))
					goto err0;
			}

			if ((err = consume_type(cx, LT_LPCTK_SEMICOLON, NULL)))
				goto err0;

			lt_lpc_sym_t symbol;
			symbol.name = str_from_tk(cx, ident_tk);
			symbol.definition = stmt;

			if (cx->scope_depth == 0 && (err = lt_lpc_insert_global(cx, &symbol)))
				;//goto err0;

			stmt->type = LT_LPCS_VAR;
			*out = stmt;
			return LT_SUCCESS;
		}
	case LT_LPCTK_NUM: case LT_LPCTK_LPAREN: case LT_LPCTK_MINUS:
		if ((err = parse_new_expr(cx, &stmt->expr0)) == LT_SUCCESS) {
			if ((err = consume_type(cx, LT_LPCTK_SEMICOLON, NULL)))
				goto err0;
			stmt->type = LT_LPCS_EXPR;
			*out = stmt;
			return LT_SUCCESS;
		}
		goto err0;

	default:
		fail_to(err0, err = LT_ERR_INVALID_SYNTAX, "unexpected `%S`, expected a valid statement", str_from_tk(cx, *cx->it)); // !!
	}

err0:	lt_lpc_free_stmt(stmt, cx->alloc);
		return err;
}

#include <lt/sort.h>
#include <lt/str.h>

#define sym_is_equal(sym, key) (lt_lstr_eq((sym)->name, key))
#define sym_is_lesser(sym, key) (lt_lstr_cmp((sym)->name, key) < 0)

LT_DEFINE_BINARY_SEARCH_FUNC(lt_lpc_sym_t*, lstr_t, lt_lpc_lookup_sym, sym_is_lesser, sym_is_equal);

static
LT_DEFINE_BINARY_SEARCH_NEAREST_FUNC(lt_lpc_sym_t*, lstr_t, lt_lpc_lookup_nearest_sym, sym_is_lesser, sym_is_equal);

static
lt_err_t lt_lpc_insert_global(lt_lpc_parse_ctx_t* cx, lt_lpc_sym_t* sym) {
	usz glob_count = lt_darr_count(cx->globals);
	lt_lpc_sym_t** at = lt_lpc_lookup_nearest_sym(cx->globals, glob_count, sym->name);

	if (at < cx->globals + glob_count && sym_is_equal(*at, sym->name)) {
// 		lt_aprintf(&cx->err_str, &cx->alloc->interf, "multiple definitions of symbol '%S'", sym->name);
		return LT_ERR_REDEFINED;
	}

	lt_lpc_sym_t* new_sym = lt_amalloc_lean(cx->alloc, sizeof(lt_lpc_sym_t));
	*new_sym = *sym;

	usz idx = at - cx->globals;
	lt_darr_insert(cx->globals, idx, &new_sym, 1);
	return LT_SUCCESS;
}

lt_err_t lt_lpc_parse(lt_lpc_parse_ctx_t* cx, lt_lpc_tk_t* tokens, usz token_count, lt_arena_t* alloc) {
	lt_err_t err;

	cx->globals = lt_darr_create(lt_lpc_sym_t*, 8192, &alloc->interf);
	if (!cx->globals)
		fail(LT_ERR_OUT_OF_MEMORY, "out of memory", 0);

	cx->scope_depth = 0;

	cx->it = tokens;
	cx->end = tokens + token_count;
	cx->root = NULL;
	cx->alloc = alloc;

	lt_lpc_stmt_t** current = &cx->root;
	while (cx->it < cx->end) {
		if ((err = parse_new_stmt(cx, current)))
			goto err0;
		current = &(*current)->next;
	}

	return LT_SUCCESS;

err0:	if (cx->root)
			lt_lpc_free_stmt(cx->root, alloc);
		return err;
}

static
lt_err_t resolve_expr(lt_lpc_parse_ctx_t* cx, lt_lpc_expr_t* expr) {
	lt_err_t err;

	switch ((lt_lpc_expr_type_t)expr->type) {
	case LT_LPCE_INVAL: case LT_LPCE_MAX:
		LT_ASSERT_NOT_REACHED();

	case LT_LPCE_ADD:
	case LT_LPCE_SUB:
	case LT_LPCE_MUL:
	case LT_LPCE_DIV:
	case LT_LPCE_MOD:
	case LT_LPCE_OR:
	case LT_LPCE_AND:
	case LT_LPCE_XOR:
	case LT_LPCE_SHL:
	case LT_LPCE_SHR:
	case LT_LPCE_SET:
	case LT_LPCE_SETADD:
	case LT_LPCE_SETSUB:
	case LT_LPCE_SETMUL:
	case LT_LPCE_SETDIV:
	case LT_LPCE_SETMOD:
	case LT_LPCE_SETOR:
	case LT_LPCE_SETAND:
	case LT_LPCE_SETXOR:
	case LT_LPCE_SETSHL:
	case LT_LPCE_SETSHR:
	case LT_LPCE_LOGEQ:
	case LT_LPCE_LOGNEQ:
	case LT_LPCE_LOGLEQ:
	case LT_LPCE_LOGGEQ:
	case LT_LPCE_LOGLESSER:
	case LT_LPCE_LOGGREATER:
	case LT_LPCE_LOGOR:
	case LT_LPCE_LOGAND:
	case LT_LPCE_RANGE:
		if ((err = resolve_expr(cx, expr->child0)))
			return err;
		if ((err = resolve_expr(cx, expr->child1)))
			return err;
		break;

	case LT_LPCE_DEREF:
	case LT_LPCE_REF:
	case LT_LPCE_NEG:
	case LT_LPCE_INV:
	case LT_LPCE_LOGINV:
		if ((err = resolve_expr(cx, expr->child0)))
			return err;
		break;

	case LT_LPCE_IDENT:
		
		break;

	case LT_LPCE_NUM:
		break;

	case LT_LPCE_STR:
		break;

	case LT_LPCE_PROC:
		for (lt_lpc_stmt_t* it = expr->stmt; it; it = it->next)
			if ((err = resolve_stmt(cx, it)))
			return err;
		break;

	case LT_LPCE_IF:
		if ((err = resolve_expr(cx, expr->child0)))
			return err;
		if ((err = resolve_expr(cx, expr->child1)))
			return err;
		if ((err = resolve_expr(cx, expr->child2)))
			return err;
		break;

	case LT_LPCE_IMPORT:
		if ((err = resolve_expr(cx, expr->child0)))
			return err;
		break;

	case LT_LPCE_CALL:
		if ((err = resolve_expr(cx, expr->child0)))
			return err;
		for (lt_lpc_expr_t* it = expr->child1; it; it = it->next)
			if ((err = resolve_expr(cx, it)))
				return err;
		break;

	case LT_LPCE_SUBSCRIPT:
		if ((err = resolve_expr(cx, expr->child0)))
			return err;
		if ((err = resolve_expr(cx, expr->child1)))
			return err;
		break;

	case LT_LPCE_PTR:
	case LT_LPCE_ARRAY:
		break;
	}

	return LT_SUCCESS;
}

static
lt_err_t resolve_stmt(lt_lpc_parse_ctx_t* cx, lt_lpc_stmt_t* stmt) {
	lt_err_t err;

	stmt->state = LT_LPC_RESOLVE_STARTED;

	switch ((lt_lpc_stmt_type_t)stmt->type) {
	case LT_LPCS_INVAL: case LT_LPCS_MAX:
		LT_ASSERT_NOT_REACHED();

	case LT_LPCS_ASSERT:
		if ((err = resolve_expr(cx, stmt->expr0)))
			return err;
		break;

	case LT_LPCS_IF:
		if ((err = resolve_expr(cx, stmt->expr0)))
			return err;

		for (lt_lpc_stmt_t* it = stmt->child0; it; it = it->next)
			if ((err = resolve_stmt(cx, it)))
				return err;

		for (lt_lpc_stmt_t* it = stmt->child1; it; it = it->next) {
			if ((err = resolve_expr(cx, it->expr0)))
				return err;
			for (lt_lpc_stmt_t* it2 = it->child0; it2; it2 = it2->next)
				if ((err = resolve_stmt(cx, it2)))
					return err;
		}
		if (stmt->expr1 && (err = resolve_expr(cx, stmt->expr1)))
			return err;
		break;

	case LT_LPCS_FOR:
		if (stmt->expr0 && (err = resolve_expr(cx, stmt->expr0)))
			return err;
		if (stmt->expr1 && (err = resolve_expr(cx, stmt->expr1)))
			return err;
		for (lt_lpc_stmt_t* it = stmt->child0; it; it = it->next)
			if ((err = resolve_stmt(cx, it)))
				return err;
		break;

	case LT_LPCS_VAR:
		if (stmt->expr0 && (err = resolve_expr(cx, stmt->expr0)))
			return err;
		if (stmt->expr1 && (err = resolve_expr(cx, stmt->expr1)))
			return err;
		break;

	case LT_LPCS_RETURN:
		if (stmt->expr0 && (err = resolve_expr(cx, stmt->expr0)))
			return err;
		break;

	case LT_LPCS_IMPORT:
// 		lt_lpc_write_expr(stmt->expr0, callb, usr, indent + 1);
		break;

	case LT_LPCS_SWITCH:
		for (lt_lpc_stmt_t* it = stmt->child0; it; it = it->next)
			if ((err = resolve_stmt(cx, it)))
			return err;
		if (stmt->child1 && (err = resolve_stmt(cx, stmt->child1)))
			return err;
		break;

	case LT_LPCS_CASE:
		for (lt_lpc_expr_t* it = stmt->expr0; it; it = it->next)
			if ((err = resolve_expr(cx, it)))
			return err;
		for (lt_lpc_stmt_t* it = stmt->child0; it; it = it->next)
			if ((err = resolve_stmt(cx, stmt->child0)))
			return err;
		break;

	case LT_LPCS_BREAK:
	case LT_LPCS_CONTINUE:
		break;

	case LT_LPCS_DEFER:
		if ((err = resolve_expr(cx, stmt->expr0)))
			return err;
		break;

	case LT_LPCS_EXPR:
		if ((err = resolve_expr(cx, stmt->expr0)))
			return err;
		break;
	}

	stmt->state = LT_LPC_RESOLVE_FINISHED;
	return LT_SUCCESS;
}

lt_err_t lt_lpc_resolve_tree(lt_lpc_parse_ctx_t* cx) {
	lt_err_t err;
	for (lt_lpc_stmt_t* it = cx->root; it; it = it->next) {
		if ((err = resolve_stmt(cx, it)))
			return err;
	}

	return LT_SUCCESS;
}

#define PRINTF(...) lt_io_printf(callb, usr, __VA_ARGS__)

void lt_lpc_write_expr(lt_lpc_expr_t* expr, lt_io_callback_t callb, void* usr, usz indent) {
	PRINTF("%r|%S", indent, lt_lpc_expr_type_str(expr->type));

	switch ((lt_lpc_expr_type_t)expr->type) {
	case LT_LPCE_INVAL: case LT_LPCE_MAX:
		LT_ASSERT_NOT_REACHED();

	case LT_LPCE_ADD:
	case LT_LPCE_SUB:
	case LT_LPCE_MUL:
	case LT_LPCE_DIV:
	case LT_LPCE_MOD:
	case LT_LPCE_OR:
	case LT_LPCE_AND:
	case LT_LPCE_XOR:
	case LT_LPCE_SHL:
	case LT_LPCE_SHR:
	case LT_LPCE_SET:
	case LT_LPCE_SETADD:
	case LT_LPCE_SETSUB:
	case LT_LPCE_SETMUL:
	case LT_LPCE_SETDIV:
	case LT_LPCE_SETMOD:
	case LT_LPCE_SETOR:
	case LT_LPCE_SETAND:
	case LT_LPCE_SETXOR:
	case LT_LPCE_SETSHL:
	case LT_LPCE_SETSHR:
	case LT_LPCE_LOGEQ:
	case LT_LPCE_LOGNEQ:
	case LT_LPCE_LOGLEQ:
	case LT_LPCE_LOGGEQ:
	case LT_LPCE_LOGLESSER:
	case LT_LPCE_LOGGREATER:
	case LT_LPCE_LOGOR:
	case LT_LPCE_LOGAND:
	case LT_LPCE_RANGE:
		PRINTF("\n");
		lt_lpc_write_expr(expr->child0, callb, usr, indent + 1);
		lt_lpc_write_expr(expr->child1, callb, usr, indent + 1);
		break;

	case LT_LPCE_DEREF:
	case LT_LPCE_REF:
	case LT_LPCE_NEG:
	case LT_LPCE_INV:
	case LT_LPCE_LOGINV:
		PRINTF("\n");
		lt_lpc_write_expr(expr->child0, callb, usr, indent + 1);
		break;

	case LT_LPCE_IDENT:
		PRINTF(" <name>\n");
		break;

	case LT_LPCE_NUM:
		PRINTF(" <number>\n");
		break;

	case LT_LPCE_STR:
		PRINTF(" <string>\n");
		break;

	case LT_LPCE_PROC:
		PRINTF(" <proc>\n");
		for (lt_lpc_stmt_t* it = expr->stmt; it; it = it->next)
			lt_lpc_write_stmt(it, callb, usr, indent + 1);
		break;

	case LT_LPCE_IF:
		PRINTF("\n");
		lt_lpc_write_expr(expr->child0, callb, usr, indent + 1);
		PRINTF("%r|then\n", indent + 1);
		lt_lpc_write_expr(expr->child1, callb, usr, indent + 2);
		PRINTF("%r|else\n", indent + 1);
		lt_lpc_write_expr(expr->child2, callb, usr, indent + 2);
		break;

	case LT_LPCE_IMPORT:
		PRINTF("\n");
		lt_lpc_write_expr(expr->child0, callb, usr, indent + 1);
		break;

	case LT_LPCE_CALL:
		PRINTF("\n");
		lt_lpc_write_expr(expr->child0, callb, usr, indent + 1);
		PRINTF("%r|args\n", indent + 1);
		for (lt_lpc_expr_t* it = expr->child1; it; it = it->next)
			lt_lpc_write_expr(it, callb, usr, indent + 2);
		break;

	case LT_LPCE_SUBSCRIPT:
		PRINTF("\n");
		lt_lpc_write_expr(expr->child0, callb, usr, indent + 1);
		lt_lpc_write_expr(expr->child1, callb, usr, indent + 1);
		break;

	case LT_LPCE_PTR:
	case LT_LPCE_ARRAY:
		PRINTF("\n");
		break;
	}
}

void lt_lpc_write_stmt(lt_lpc_stmt_t* stmt, lt_io_callback_t callb, void* usr, usz indent) {
	if (stmt->type != LT_LPCS_EXPR)
		PRINTF("%r|%S", indent, lt_lpc_stmt_type_str(stmt->type));

	switch ((lt_lpc_stmt_type_t)stmt->type) {
	case LT_LPCS_INVAL: case LT_LPCS_MAX:
		LT_ASSERT_NOT_REACHED();

	case LT_LPCS_ASSERT:
		PRINTF("\n");
		lt_lpc_write_expr(stmt->expr0, callb, usr, indent + 1);
		break;

	case LT_LPCS_IF:
		PRINTF("\n");
		lt_lpc_write_expr(stmt->expr0, callb, usr, indent + 1);

		PRINTF("%r|do\n", indent + 1);
		for (lt_lpc_stmt_t* it = stmt->child0; it; it = it->next)
			lt_lpc_write_stmt(it, callb, usr, indent + 2);

		for (lt_lpc_stmt_t* it = stmt->child1; it; it = it->next) {
			PRINTF("%r|elif\n", indent + 1);
			lt_lpc_write_expr(it->expr0, callb, usr, indent + 2);
			PRINTF("%r|do\n", indent + 2);
			for (lt_lpc_stmt_t* it2 = it->child0; it2; it2 = it2->next)
				lt_lpc_write_stmt(it2, callb, usr, indent + 3);
		}

		if (stmt->expr1) {
			PRINTF("%r|else\n", indent + 1);
			lt_lpc_write_expr(stmt->expr1, callb, usr, indent + 2);
		}
		break;

	case LT_LPCS_FOR:
		PRINTF("\n");
		if (stmt->expr0)
			lt_lpc_write_expr(stmt->expr0, callb, usr, indent + 1);
		if (stmt->expr1)
			lt_lpc_write_expr(stmt->expr1, callb, usr, indent + 1);
		PRINTF("%r|do\n", indent + 1);
		for (lt_lpc_stmt_t* it = stmt->child0; it; it = it->next)
			lt_lpc_write_stmt(it, callb, usr, indent + 2);
		break;

	case LT_LPCS_VAR:
		PRINTF(" <name> : ");
		PRINTF("<inferred>\n");

		if (stmt->expr1)
			lt_lpc_write_expr(stmt->expr1, callb, usr, indent + 1);
		break;

	case LT_LPCS_RETURN:
		PRINTF("\n");
		if (stmt->expr0)
			lt_lpc_write_expr(stmt->expr0, callb, usr, indent + 1);
		break;

	case LT_LPCS_IMPORT:
		PRINTF("\n");
// 		lt_lpc_write_expr(stmt->expr0, callb, usr, indent + 1);
		break;

	case LT_LPCS_SWITCH:
		PRINTF("\n");
		for (lt_lpc_stmt_t* it = stmt->child0; it; it = it->next)
			lt_lpc_write_stmt(it, callb, usr, indent + 1);
		if (stmt->child1) {
			PRINTF("%r|else\n", indent + 1);
			lt_lpc_write_stmt(stmt->child1, callb, usr, indent + 2);
		}
		break;

	case LT_LPCS_CASE:
		PRINTF("\n");
		for (lt_lpc_expr_t* it = stmt->expr0; it; it = it->next)
			lt_lpc_write_expr(it, callb, usr, indent + 1);
		for (lt_lpc_stmt_t* it = stmt->child0; it; it = it->next)
			lt_lpc_write_stmt(it, callb, usr, indent + 2);
		break;

	case LT_LPCS_BREAK:
	case LT_LPCS_CONTINUE:
		PRINTF("\n");
		break;

	case LT_LPCS_DEFER:
		PRINTF("\n");
		lt_lpc_write_expr(stmt->expr0, callb, usr, indent + 1);
		break;

	case LT_LPCS_EXPR:
		lt_lpc_write_expr(stmt->expr0, callb, usr, indent);
		break;
	}
}
