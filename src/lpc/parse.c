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

#define fail(...) \
	do { \
		lt_aprintf(&cx->err_str, &cx->alloc->interf, __VA_ARGS__); \
		return LT_ERR_INVALID_SYNTAX; \
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

static
lt_lpc_stmt_t* new_stmt(lt_arena_t* alloc) {
	lt_lpc_stmt_t* new = lt_amalloc_lean(alloc, sizeof(lt_lpc_stmt_t));
	if (new)
		memset(new, 0, sizeof(lt_lpc_stmt_t));
	return new;
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
		fail("unexpected end of file");

	if (out)
		*out = *cx->it;

	++cx->it;
	return LT_SUCCESS;
}

static LT_INLINE
lt_err_t consume_type(lt_lpc_parse_ctx_t* cx, lt_lpc_tk_type_t type, lt_lpc_tk_t* out) {
	if (cx->it >= cx->end)
		fail("unexpected end of file");

	if (cx->it->type != type)
		fail("unexpected `%S`, expected `%S`", str_from_tk(cx, *cx->it), lt_lpc_tk_type_str(type));

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
			return LT_ERR_OUT_OF_MEMORY;
		consume(cx, NULL);
		if ((err = parse_new_expr(cx, &expr->if_.cond)))
			return err;
		if ((err = consume_type(cx, LT_LPCTK_KWTHEN, NULL)))
			return err;
		if ((err = parse_new_expr(cx, &expr->if_.then)))
			return err;
		if ((err = consume_type(cx, LT_LPCTK_KWELSE, NULL)))
			return err;
		if ((err = parse_new_expr(cx, &expr->if_.else_)))
			return err;
		*out = expr;
		expr->type = LT_LPCE_IF;
		return LT_SUCCESS;

	case LT_LPCTK_IDENT:
		if (!(expr = new_expr(cx->alloc)))
			return LT_ERR_OUT_OF_MEMORY;
		consume(cx, NULL);
		expr->type = LT_LPCE_IDENT;
		*out = expr;
		return LT_SUCCESS;

	case LT_LPCTK_NUM:
		if (!(expr = new_expr(cx->alloc)))
			return LT_ERR_OUT_OF_MEMORY;
		consume(cx, NULL);
		expr->type = LT_LPCE_NUM;
		*out = expr;
		return LT_SUCCESS;

	case LT_LPCTK_CHAR:
		if (!(expr = new_expr(cx->alloc)))
			return LT_ERR_OUT_OF_MEMORY;
		consume(cx, NULL);
		expr->type = LT_LPCE_NUM;
		*out = expr;
		return LT_SUCCESS;

	case LT_LPCTK_STR:
		if (!(expr = new_expr(cx->alloc)))
			return LT_ERR_OUT_OF_MEMORY;
		consume(cx, NULL);
		expr->type = LT_LPCE_STR;
		*out = expr;
		return LT_SUCCESS;

	case LT_LPCTK_LPAREN:
		consume(cx, NULL);
		if ((err = parse_new_expr(cx, &expr)))
			return err;
		if ((err = consume_type(cx, LT_LPCTK_RPAREN, NULL)))
			return err;
		*out = expr;
		return LT_SUCCESS;

	case LT_LPCTK_KWIMPORT:
		if (!(expr = new_expr(cx->alloc)))
			return LT_ERR_OUT_OF_MEMORY;
		consume(cx, NULL);
		if ((err = parse_new_expr(cx, &expr->unary.child)))
			return err;
		expr->type = LT_LPCE_IMPORT;
		*out = expr;
		return LT_SUCCESS;

	case LT_LPCTK_KWPROC:
		if (!(expr = new_expr(cx->alloc)))
			return LT_ERR_OUT_OF_MEMORY;
		parse_function_def(cx, NULL);
		++cx->scope_depth;
		if ((err = consume_type(cx, LT_LPCTK_KWBEGIN, NULL)))
			return err;
		if ((err = parse_new_block(cx, &expr->proc.body)))
			return err;
		if ((err = consume_type(cx, LT_LPCTK_KWEND, NULL)))
			return err;
		--cx->scope_depth;
		expr->type = LT_LPCE_PROC;
		*out = expr;
		return LT_SUCCESS;

	default:
		fail("unexpected `%S`, expected a valid expression", str_from_tk(cx, *cx->it));
	}

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
				return LT_ERR_OUT_OF_MEMORY;
			consume(cx, NULL);
			(*current)->type = LT_LPCE_NEG;
			current = &(*current)->unary.child;
			break;

		case LT_LPCTK_EXC:
			if (!(*current = new_expr(cx->alloc)))
				return LT_ERR_OUT_OF_MEMORY;
			consume(cx, NULL);
			(*current)->type = LT_LPCE_LOGINV;
			current = &(*current)->unary.child;
			break;

		case LT_LPCTK_TILDE:
			if (!(*current = new_expr(cx->alloc)))
				return LT_ERR_OUT_OF_MEMORY;
			consume(cx, NULL);
			(*current)->type = LT_LPCE_INV;
			current = &(*current)->unary.child;
			break;

		case LT_LPCTK_ASTER:
			if (!(*current = new_expr(cx->alloc)))
				return LT_ERR_OUT_OF_MEMORY;
			consume(cx, NULL);
			(*current)->type = LT_LPCE_PTR;
			current = &(*current)->unary.child;
			break;

		case LT_LPCTK_LBRACKET:
			if (!(*current = new_expr(cx->alloc)))
				return LT_ERR_OUT_OF_MEMORY;
			consume(cx, NULL);

			if (read_type(cx) != LT_LPCTK_RBRACKET) {
				if ((err = parse_new_expr(cx, &(*current)->binary.right))) // !!
					return err;
			}
			if ((err = consume_type(cx, LT_LPCTK_RBRACKET, NULL)))
				return err;
			(*current)->type = LT_LPCE_ARRAY;
			current = &(*current)->array.type;
			break;

		default:
			goto prefix_done;
		}
	}

prefix_done:
	if ((err = parse_new_expr_primary(cx, &primary)))
		return err;

	for (;;) {
		switch (read_type(cx)) {
		case LT_LPCTK_LPAREN: {
			if (!(postfix = new_expr(cx->alloc)))
				return LT_ERR_OUT_OF_MEMORY;
			consume(cx, NULL);

			lt_lpc_expr_t** current = &postfix->call.args;
			while (read_type(cx) != LT_LPCTK_RPAREN) {
				if (current != &postfix->call.args && (err = consume_type(cx, LT_LPCTK_COMMA, NULL)))
					return err;
				if ((err = parse_new_expr(cx, current)))
					return err;
				current = &(*current)->next;
			}
			consume(cx, NULL);
			postfix->type = LT_LPCE_CALL;
			postfix->call.func = primary;
		}	break;

		case LT_LPCTK_LBRACKET:
			consume(cx, NULL);
			if (!(postfix = new_expr(cx->alloc)))
				return LT_ERR_OUT_OF_MEMORY;
			if ((err = parse_new_expr(cx, &postfix->subscript.index)))
				return err;
			if ((err = consume_type(cx, LT_LPCTK_RBRACKET, NULL)))
				return err;
			postfix->type = LT_LPCE_SUBSCRIPT;
			postfix->subscript.array = primary;
			break;

		case LT_LPCTK_AMP:
			consume(cx, NULL);
			if (!(postfix = new_expr(cx->alloc)))
				return LT_ERR_OUT_OF_MEMORY;
			postfix->type = LT_LPCE_REF;
			postfix->unary.child = primary;
			break;

		case LT_LPCTK_ASTER:
			consume(cx, NULL);
			if (!(postfix = new_expr(cx->alloc)))
				return LT_ERR_OUT_OF_MEMORY;
			postfix->type = LT_LPCE_DEREF;
			postfix->unary.child = primary;
			break;

		default:
			*out = primary;
			return LT_SUCCESS;
		}
		primary = postfix;
	}
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
			return err;

		lt_lpc_expr_t* expr = new_expr(cx->alloc);
		if (!expr)
			return LT_ERR_OUT_OF_MEMORY;

		expr->type = op->expr_type;
		expr->binary.left = left;
		expr->binary.right = right;

		left = expr;
	}
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
			return err;
		current = &(*current)->next;
		type = read_type(cx);
	}

	*out = root;
	return LT_SUCCESS;
}

static
lt_err_t parse_new_stmt(lt_lpc_parse_ctx_t* cx, lt_lpc_stmt_t** out) {
	lt_err_t err;

	lt_lpc_stmt_t* stmt = new_stmt(cx->alloc);
	if (!stmt)
		return LT_ERR_OUT_OF_MEMORY;

	switch (read_type(cx)) {
	case LT_LPCTK_KWASSERT:
		consume(cx, NULL);
		if ((err = parse_new_expr(cx, &stmt->assert.cond)))
			return err;
		if ((err = consume_type(cx, LT_LPCTK_SEMICOLON, NULL)))
			return err;
		stmt->type = LT_LPCS_ASSERT;
		*out = stmt;
		return LT_SUCCESS;

	case LT_LPCTK_KWIF: {
		consume(cx, NULL);
		if ((err = parse_new_expr(cx, &stmt->if_.cond)))
			return err;
		if ((err = consume_type(cx, LT_LPCTK_KWDO, NULL)))
			return err;
		if ((err = parse_new_block(cx, &stmt->if_.then)))
			return err;

		lt_lpc_stmt_t** current = &stmt->if_.else_;
		while (read_type(cx) == LT_LPCTK_KWELIF) {
			consume(cx, NULL);
			if (!(*current = new_stmt(cx->alloc)))
				return LT_ERR_OUT_OF_MEMORY;
			(*current)->type = LT_LPCS_IF;
			if ((err = parse_new_expr(cx, &(*current)->if_.cond)))
				return err;
			if ((err = consume_type(cx, LT_LPCTK_KWDO, NULL)))
				return err;
			if ((err = parse_new_block(cx, &(*current)->if_.then)))
				return err;
			current = &(*current)->if_.else_;
		}

		if (read_type(cx) == LT_LPCTK_KWELSE) {
			consume(cx, NULL);
			if ((err = parse_new_block(cx, current)))
				return err;
		}

		if ((err = consume_type(cx, LT_LPCTK_KWEND, NULL)))
			return err;

		stmt->type = LT_LPCS_IF;
		*out = stmt;
	}	return LT_SUCCESS;

	case LT_LPCTK_KWSWITCH: {
		consume(cx, NULL);

		if ((err = parse_new_expr(cx, &stmt->switch_.value)))
			return err;

		lt_lpc_stmt_t** current = &stmt->switch_.cases;
		while (read_type(cx) == LT_LPCTK_KWCASE) {
			consume(cx, NULL);

			if (!(*current = new_stmt(cx->alloc)))
				return LT_ERR_OUT_OF_MEMORY;
			(*current)->type = LT_LPCS_CASE;

			lt_lpc_expr_t** current_expr = &(*current)->case_.values;
			if ((err = parse_new_expr(cx, current_expr)))
				return err;
			current_expr = &(*current_expr)->next;
			while (read_type(cx) == LT_LPCTK_COMMA) {
				consume(cx, NULL);
				if ((err = parse_new_expr(cx, current_expr)))
					return err;
				current_expr = &(*current_expr)->next;
			}

			if ((err = consume_type(cx, LT_LPCTK_KWDO, NULL)))
				return err;
			if ((err = parse_new_block(cx, &(*current)->case_.then)))
				return err;

			current = &(*current)->next;
		}

		if (read_type(cx) == LT_LPCTK_KWELSE) {
			consume(cx, NULL);
			if ((err = parse_new_block(cx, &stmt->switch_.else_)))
				return err;
		}

		if ((err = consume_type(cx, LT_LPCTK_KWEND, NULL)))
			return err;

		stmt->type = LT_LPCS_SWITCH;
		*out = stmt;
	}	return LT_SUCCESS;

	case LT_LPCTK_KWIMPORT:
		consume(cx, NULL);
		if ((err = consume_type(cx, LT_LPCTK_IDENT, NULL)))
			return err;
		if ((err = consume_type(cx, LT_LPCTK_SEMICOLON, NULL)))
			return err;
		stmt->type = LT_LPCS_IMPORT;
		*out = stmt;
		return LT_SUCCESS;

	case LT_LPCTK_KWRETURN:
		consume(cx, NULL);
		if ((err = parse_new_expr(cx, &stmt->return_.value)))
			return err;
		if ((err = consume_type(cx, LT_LPCTK_SEMICOLON, NULL)))
			return err;
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
			if ((err = parse_new_expr(cx, &stmt->for_.range))) // !!
				return err;
			if (read_type(cx) == LT_LPCTK_COLON) {
				consume(cx, NULL);
				if ((err = parse_new_expr(cx, &stmt->for_.range)))
					return err;
			}
		}
		if ((err = consume_type(cx, LT_LPCTK_KWDO, NULL)))
			return err;
		if ((err = parse_new_block(cx, &stmt->for_.do_)))
			return err;
		if ((err = consume_type(cx, LT_LPCTK_KWEND, NULL)))
			return err;
		stmt->type = LT_LPCS_FOR;
		*out = stmt;
		return LT_SUCCESS;

	case LT_LPCTK_KWDEFER:
		consume(cx, NULL);
		if ((err = parse_new_expr(cx, &stmt->defer.expr)))
			return err;
		if ((err = consume_type(cx, LT_LPCTK_SEMICOLON, NULL)))
			return err;
		stmt->type = LT_LPCS_DEFER;
		*out = stmt;
		return LT_SUCCESS;

	case LT_LPCTK_IDENT:
		if (cx->it + 1 < cx->end && (cx->it + 1)->type == LT_LPCTK_COLON) {
			lt_lpc_tk_t ident_tk;
			consume(cx, &ident_tk);
			consume(cx, NULL);

			lt_lpc_tk_type_t type = read_type(cx);
			if (type != LT_LPCTK_COLON && type != LT_LPCTK_EQ && (err = parse_new_expr(cx, &stmt->var.type)))
				return err;

			type = read_type(cx);
			if (type == LT_LPCTK_COLON) {
				consume(cx, NULL);
				if ((err = parse_new_expr(cx, &stmt->var.init)))
					return err;
			}
			else if (type == LT_LPCTK_EQ) {
				consume(cx, NULL);
				if ((err = parse_new_expr(cx, &stmt->var.init)))
					return err;
			}

			if ((err = consume_type(cx, LT_LPCTK_SEMICOLON, NULL)))
				return err;

			lt_lpc_sym_t symbol;
			symbol.name = str_from_tk(cx, ident_tk);
			symbol.definition = stmt;

			if (cx->scope_depth == 0 && (err = lt_lpc_insert_global(cx, &symbol)))
				return err;

			stmt->type = LT_LPCS_VAR;
			*out = stmt;
			return LT_SUCCESS;
		}
	case LT_LPCTK_NUM: case LT_LPCTK_LPAREN: case LT_LPCTK_MINUS:
		if ((err = parse_new_expr(cx, &stmt->expr.expr)) == LT_SUCCESS) {
			if ((err = consume_type(cx, LT_LPCTK_SEMICOLON, NULL)))
				return err;
			stmt->type = LT_LPCS_EXPR;
			*out = stmt;
			return LT_SUCCESS;
		}
		return err;

	default:
		fail("unexpected `%S`, expected a valid statement", str_from_tk(cx, *cx->it)); // !!
	}
}

#include <lt/sort.h>
#include <lt/str.h>

#define sym_is_equal(sym, key) (lt_lseq((sym)->name, key))
#define sym_is_lesser(sym, key) (lt_lscmp((sym)->name, key) < 0)

LT_DEFINE_BINARY_SEARCH_FUNC(lt_lpc_sym_t*, lstr_t, lt_lpc_lookup_sym, sym_is_lesser, sym_is_equal);

static
LT_DEFINE_BINARY_SEARCH_NEAREST_FUNC(lt_lpc_sym_t*, lstr_t, lt_lpc_lookup_nearest_sym, sym_is_lesser, sym_is_equal);

static
lt_err_t lt_lpc_insert_global(lt_lpc_parse_ctx_t* cx, lt_lpc_sym_t* sym) {
	usz glob_count = lt_darr_count(cx->globals);
	lt_lpc_sym_t** at = lt_lpc_lookup_nearest_sym(cx->globals, glob_count, sym->name);

	if (at < cx->globals + glob_count && sym_is_equal(*at, sym->name))
		return LT_SUCCESS;//fail("multiple definitions of symbol '%S'", sym->name);

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
		return LT_ERR_OUT_OF_MEMORY;

	cx->scope_depth = 0;

	cx->it = tokens;
	cx->end = tokens + token_count;
	cx->root = NULL;
	cx->alloc = alloc;

	lt_lpc_stmt_t** current = &cx->root;
	while (cx->it < cx->end) {
		if ((err = parse_new_stmt(cx, current)))
			return err;
		current = &(*current)->next;
	}

	return LT_SUCCESS;
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
		if ((err = resolve_expr(cx, expr->binary.left)))
			return err;
		if ((err = resolve_expr(cx, expr->binary.right)))
			return err;
		break;

	case LT_LPCE_IMPORT:
	case LT_LPCE_DEREF:
	case LT_LPCE_REF:
	case LT_LPCE_NEG:
	case LT_LPCE_INV:
	case LT_LPCE_LOGINV:
		if ((err = resolve_expr(cx, expr->unary.child)))
			return err;
		break;

	case LT_LPCE_IDENT:
		
		break;

	case LT_LPCE_NUM:
		break;

	case LT_LPCE_STR:
		break;

	case LT_LPCE_PROC:
		for (lt_lpc_stmt_t* it = expr->proc.body; it; it = it->next)
			if ((err = resolve_stmt(cx, it)))
				return err;
		break;

	case LT_LPCE_IF:
		if ((err = resolve_expr(cx, expr->if_.cond)))
			return err;
		if ((err = resolve_expr(cx, expr->if_.then)))
			return err;
		if ((err = resolve_expr(cx, expr->if_.else_)))
			return err;
		break;

	case LT_LPCE_CALL:
		if ((err = resolve_expr(cx, expr->call.func)))
			return err;
		for (lt_lpc_expr_t* it = expr->call.args; it; it = it->next)
			if ((err = resolve_expr(cx, it)))
				return err;
		break;

	case LT_LPCE_SUBSCRIPT:
		if ((err = resolve_expr(cx, expr->subscript.array)))
			return err;
		if ((err = resolve_expr(cx, expr->subscript.index)))
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
		if ((err = resolve_expr(cx, stmt->assert.cond)))
			return err;
		break;

	case LT_LPCS_IF:
		if ((err = resolve_expr(cx, stmt->if_.cond)))
			return err;

		for (lt_lpc_stmt_t* it = stmt->if_.then; it; it = it->next)
			if ((err = resolve_stmt(cx, it)))
				return err;

		for (lt_lpc_stmt_t* it = stmt->if_.else_; it; it = it->next)
			if ((err = resolve_stmt(cx, it)))
				return err;
		break;

	case LT_LPCS_FOR:
		if (stmt->for_.range && (err = resolve_expr(cx, stmt->for_.range)))
			return err;
// 		if (stmt->expr1 && (err = resolve_expr(cx, stmt->expr1)))
// 			return err;
		for (lt_lpc_stmt_t* it = stmt->for_.do_; it; it = it->next)
			if ((err = resolve_stmt(cx, it)))
				return err;
		break;

	case LT_LPCS_VAR:
		if (stmt->var.type && (err = resolve_expr(cx, stmt->var.type)))
			return err;
		if (stmt->var.init && (err = resolve_expr(cx, stmt->var.init)))
			return err;
		break;

	case LT_LPCS_RETURN:
		if (stmt->return_.value && (err = resolve_expr(cx, stmt->return_.value)))
			return err;
		break;

	case LT_LPCS_IMPORT:
// 		lt_lpc_write_expr(stmt->expr0, callb, usr, indent + 1);
		break;

	case LT_LPCS_SWITCH:
		if ((err = resolve_expr(cx, stmt->switch_.value)))
			return err;
		for (lt_lpc_stmt_t* it = stmt->switch_.cases; it; it = it->next)
			if ((err = resolve_stmt(cx, it)))
				return err;
		for (lt_lpc_stmt_t* it = stmt->switch_.else_; it; it = it->next)
			if ((err = resolve_stmt(cx, it)))
				return err;
		break;

	case LT_LPCS_CASE:
		for (lt_lpc_expr_t* it = stmt->case_.values; it; it = it->next)
			if ((err = resolve_expr(cx, it)))
			return err;
		for (lt_lpc_stmt_t* it = stmt->case_.then; it; it = it->next)
			if ((err = resolve_stmt(cx, it)))
			return err;
		break;

	case LT_LPCS_BREAK:
	case LT_LPCS_CONTINUE:
		break;

	case LT_LPCS_DEFER:
		if ((err = resolve_expr(cx, stmt->defer.expr)))
			return err;
		break;

	case LT_LPCS_EXPR:
		if ((err = resolve_expr(cx, stmt->expr.expr)))
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
		lt_lpc_write_expr(expr->binary.left, callb, usr, indent + 1);
		lt_lpc_write_expr(expr->binary.right, callb, usr, indent + 1);
		break;

	case LT_LPCE_DEREF:
	case LT_LPCE_REF:
	case LT_LPCE_NEG:
	case LT_LPCE_INV:
	case LT_LPCE_LOGINV:
	case LT_LPCE_IMPORT:
		PRINTF("\n");
		lt_lpc_write_expr(expr->unary.child, callb, usr, indent + 1);
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

	case LT_LPCE_PROC: // !! print args and return type
		PRINTF(" <proc>\n");
		for (lt_lpc_stmt_t* it = expr->proc.body; it; it = it->next)
			lt_lpc_write_stmt(it, callb, usr, indent + 1);
		break;

	case LT_LPCE_IF:
		PRINTF("\n");
		lt_lpc_write_expr(expr->if_.cond, callb, usr, indent + 1);
		PRINTF("%r|then\n", indent + 1);
		lt_lpc_write_expr(expr->if_.then, callb, usr, indent + 2);
		PRINTF("%r|else\n", indent + 1);
		lt_lpc_write_expr(expr->if_.else_, callb, usr, indent + 2);
		break;

	case LT_LPCE_CALL:
		PRINTF("\n");
		lt_lpc_write_expr(expr->call.func, callb, usr, indent + 1);
		PRINTF("%r|args\n", indent + 1);
		for (lt_lpc_expr_t* it = expr->call.args; it; it = it->next)
			lt_lpc_write_expr(it, callb, usr, indent + 2);
		break;

	case LT_LPCE_SUBSCRIPT:
		PRINTF("\n");
		lt_lpc_write_expr(expr->subscript.array, callb, usr, indent + 1);
		lt_lpc_write_expr(expr->subscript.index, callb, usr, indent + 1);
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
		lt_lpc_write_expr(stmt->assert.cond, callb, usr, indent + 1);
		break;

	case LT_LPCS_IF:
		PRINTF("\n");
		lt_lpc_write_expr(stmt->if_.cond, callb, usr, indent + 1);

		PRINTF("%r|do\n", indent + 1);
		for (lt_lpc_stmt_t* it = stmt->if_.then; it; it = it->next)
			lt_lpc_write_stmt(it, callb, usr, indent + 2);

		PRINTF("%r|else\n", indent + 1);
		for (lt_lpc_stmt_t* it = stmt->if_.else_; it; it = it->next)
			lt_lpc_write_stmt(it, callb, usr, indent + 2);
		break;

	case LT_LPCS_FOR:
		PRINTF("\n");
		if (stmt->for_.range)
			lt_lpc_write_expr(stmt->for_.range, callb, usr, indent + 1);
// 		if (stmt->expr1)
// 			lt_lpc_write_expr(stmt->expr1, callb, usr, indent + 1);
		PRINTF("%r|do\n", indent + 1);
		for (lt_lpc_stmt_t* it = stmt->for_.do_; it; it = it->next)
			lt_lpc_write_stmt(it, callb, usr, indent + 2);
		break;

	case LT_LPCS_VAR:
		PRINTF(" <name> : ");
		PRINTF("<inferred>\n");

// 		if (stmt->var.type)
// 			lt_lpc_write_expr(stmt->var.type, callb, usr, indent + 1);
		if (stmt->var.init)
			lt_lpc_write_expr(stmt->var.init, callb, usr, indent + 1);
		break;

	case LT_LPCS_RETURN:
		PRINTF("\n");
		if (stmt->return_.value)
			lt_lpc_write_expr(stmt->return_.value, callb, usr, indent + 1);
		break;

	case LT_LPCS_IMPORT:
		PRINTF("\n");
// 		lt_lpc_write_expr(stmt->expr0, callb, usr, indent + 1);
		break;

	case LT_LPCS_SWITCH:
		PRINTF("\n");
		lt_lpc_write_expr(stmt->switch_.value, callb, usr, indent + 1);
		for (lt_lpc_stmt_t* it = stmt->switch_.cases; it; it = it->next)
			lt_lpc_write_stmt(it, callb, usr, indent + 1);
		if (stmt->switch_.else_) {
			PRINTF("%r|else\n", indent + 1);
			lt_lpc_write_stmt(stmt->switch_.else_, callb, usr, indent + 2);
		}
		break;

	case LT_LPCS_CASE:
		PRINTF("\n");
		for (lt_lpc_expr_t* it = stmt->case_.values; it; it = it->next)
			lt_lpc_write_expr(it, callb, usr, indent + 1);
		for (lt_lpc_stmt_t* it = stmt->case_.then; it; it = it->next)
			lt_lpc_write_stmt(it, callb, usr, indent + 2);
		break;

	case LT_LPCS_BREAK:
	case LT_LPCS_CONTINUE:
		PRINTF("\n");
		break;

	case LT_LPCS_DEFER:
		PRINTF("\n");
		lt_lpc_write_expr(stmt->defer.expr, callb, usr, indent + 1);
		break;

	case LT_LPCS_EXPR:
		lt_lpc_write_expr(stmt->expr.expr, callb, usr, indent);
		break;
	}
}
