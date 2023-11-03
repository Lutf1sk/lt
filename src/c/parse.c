#include <lt/c.h>
#include <lt/mem.h>
#include <lt/io.h>
#include <lt/darr.h>
#include <lt/sort.h>
#include <lt/str.h>

lstr_t lt_c_expr_type_strtab[LT_CE_MAX] = {
	[LT_CE_INVAL] = CLSTRI("invalid"),
	[LT_CE_ADD] = CLSTRI("add"),
	[LT_CE_SUB] = CLSTRI("subtract"),
	[LT_CE_MUL] = CLSTRI("multiply"),
	[LT_CE_DIV] = CLSTRI("divide"),
	[LT_CE_MOD] = CLSTRI("modulo"),
	[LT_CE_NEG] = CLSTRI("negate"),
	[LT_CE_UPLUS] = CLSTRI("unary-plus"),
	[LT_CE_OR] = CLSTRI("bit-or"),
	[LT_CE_AND] = CLSTRI("bit-and"),
	[LT_CE_XOR] = CLSTRI("bit-xor"),
	[LT_CE_SHL] = CLSTRI("bit-shr"),
	[LT_CE_SHR] = CLSTRI("bit-shr"),
	[LT_CE_INV] = CLSTRI("bit-invert"),
	[LT_CE_PFX_INC] = CLSTRI("prefix-increment"),
	[LT_CE_SFX_INC] = CLSTRI("suffix-increment"),
	[LT_CE_PFX_DEC] = CLSTRI("prefix-decrement"),
	[LT_CE_SFX_DEC] = CLSTRI("suffix-decrement"),
	[LT_CE_SET] = CLSTRI("set"),
	[LT_CE_SETADD] = CLSTRI("add-and-set"),
	[LT_CE_SETSUB] = CLSTRI("subtract-and-set"),
	[LT_CE_SETMUL] = CLSTRI("multiply-and-set"),
	[LT_CE_SETDIV] = CLSTRI("divide-and-set"),
	[LT_CE_SETMOD] = CLSTRI("modulo-and-set"),
	[LT_CE_SETOR] = CLSTRI("bit-or-and-set"),
	[LT_CE_SETAND] = CLSTRI("bit-and-and-set"),
	[LT_CE_SETXOR] = CLSTRI("bit-xor-and-set"),
	[LT_CE_SETSHL] = CLSTRI("bit-shl-and-set"),
	[LT_CE_SETSHR] = CLSTRI("bit-shr-and-set"),
	[LT_CE_LOGEQ] = CLSTRI("is-equal"),
	[LT_CE_LOGNEQ] = CLSTRI("is-not-equal"),
	[LT_CE_LOGLEQ] = CLSTRI("is-lesser-or-equal"),
	[LT_CE_LOGGEQ] = CLSTRI("is-greater-or-equal"),
	[LT_CE_LOGLESSER] = CLSTRI("is-lesser"),
	[LT_CE_LOGGREATER] = CLSTRI("is-greater"),
	[LT_CE_LOGOR] = CLSTRI("is-or"),
	[LT_CE_LOGAND] = CLSTRI("is-and"),
	[LT_CE_LOGINV] = CLSTRI("is-not"),
	[LT_CE_IDENT] = CLSTRI("identifier"),
	[LT_CE_NUM] = CLSTRI("number-literal"),
	[LT_CE_STR] = CLSTRI("string-literal"),
	[LT_CE_TERNARY] = CLSTRI("if"),
	[LT_CE_COMMA] = CLSTRI("comma"),
	[LT_CE_CAST] = CLSTRI("type-cast"),
	[LT_CE_SIZEOF] = CLSTRI("sizeof"),
	[LT_CE_ALIGNOF] = CLSTRI("alignof"),
	[LT_CE_COMPOUND] = CLSTRI("compound-literal"),
	[LT_CE_MEMBER] = CLSTRI("member"),
	[LT_CE_CALL] = CLSTRI("call"),
	[LT_CE_DEREF] = CLSTRI("dereference"),
	[LT_CE_REF] = CLSTRI("reference"),
	[LT_CE_SUBSCRIPT] = CLSTRI("subscript"),
};

lstr_t lt_c_stmt_type_strtab[LT_CS_MAX] = {
	[LT_CS_DECL] = CLSTRI("declaration"),
	[LT_CS_IF] = CLSTRI("if"),
	[LT_CS_FOR] = CLSTRI("for"),
	[LT_CS_WHILE] = CLSTRI("while"),
	[LT_CS_DO_WHILE] = CLSTRI("do-while"),
	[LT_CS_RETURN] = CLSTRI("return"),
	[LT_CS_SWITCH] = CLSTRI("switch"),
	[LT_CS_GOTO] = CLSTRI("goto"),
	[LT_CS_CASE] = CLSTRI("case"),
	[LT_CS_BREAK] = CLSTRI("break"),
	[LT_CS_CONTINUE] = CLSTRI("continue"),
	[LT_CS_EXPR] = CLSTRI("expression"),
	[LT_CS_BLOCK] = CLSTRI("block"),
	[LT_CS_LABEL] = CLSTRI("label"),
	[LT_CS_EMPTY] = CLSTRI("empty"),
};

#define PUSH_SCOPE() \
	lt_c_symtab_t symtab = (lt_c_symtab_t){ .parent = cx->syms }; \
	cx->syms = &symtab;

#define POP_SCOPE() \
	cx->syms = cx->syms->parent;

#define fail(...) \
	do { \
		lt_aprintf(&cx->err_str, &cx->alloc->interf, __VA_ARGS__); \
		return LT_ERR_INVALID_SYNTAX; \
	} while (0)

static LT_INLINE
lstr_t str_from_tk(lt_c_parse_ctx_t* cx, lt_c_tk_t tk) {
	return LSTR(cx->str_base + tk.str_offs, tk.len);
}

static LT_INLINE
lt_c_tk_type_t read_type(lt_c_parse_ctx_t* cx) {
	if (cx->it >= cx->end)
		return LT_CTK_EOF;
	return cx->it->type;
}

static LT_INLINE
lt_err_t consume(lt_c_parse_ctx_t* cx, lt_c_tk_t* out) {
	if (cx->it >= cx->end)
		fail("unexpected end of file", 0);

	if (out)
		*out = *cx->it;

	++cx->it;
	return LT_SUCCESS;
}

static LT_INLINE
lt_err_t consume_type(lt_c_parse_ctx_t* cx, lt_c_tk_type_t type, lt_c_tk_t* out) {
	if (cx->it >= cx->end)
		fail("unexpected end of file, expected `%S`", lt_c_tk_type_str(type));

	if (cx->it->type != type)
		fail("unexpected `%S`, expected `%S`", str_from_tk(cx, *cx->it), lt_c_tk_type_str(type));

	if (out)
		*out = *cx->it;

	++cx->it;
	return LT_SUCCESS;
}

static LT_INLINE
b8 scope_is_global(lt_c_parse_ctx_t* cx) {
	return cx->syms->parent == NULL;
}

static
b8 struct_is_complete(lt_c_type_t* type) {
	return type->struct_.member_types != NULL;
}

static
b8 type_eq(lt_c_type_t* type1, lt_c_type_t* type2) {
	LT_ASSERT(type1 && type2);

	while (type1->type == LT_CT_TYPEDEF)
		type1 = type1->typedef_.child;

	while (type2->type == LT_CT_TYPEDEF)
		type2 = type2->typedef_.child;

	if (type1->type != type2->type)
		return 0;

	switch ((lt_c_data_type_t)type1->type) {
	case LT_CT_INVAL: case LT_CT_TYPEDEF:
		LT_ASSERT_NOT_REACHED();
		break;

	case LT_CT_PTR:
		return type_eq(type1->ptr.child, type2->ptr.child);

	case LT_CT_ARRAY:
		return type_eq(type1->array.elem_type, type2->array.elem_type); // !! check for size

	case LT_CT_FUNC:
		if (lt_darr_count(type1->func.param_types) != lt_darr_count(type2->func.param_types))
			return 0;
		for (usz i = 0; i < lt_darr_count(type1->func.param_types); ++i) {
			if (!type_eq(type1->func.param_types[i], type2->func.param_types[i]))
				return 0;
		}
		return type_eq(type1->func.return_type, type2->func.return_type);

	case LT_CT_ENUM:
		return lt_lstr_eq(type1->enum_.tag, type2->enum_.tag) && type_eq(type1->enum_.type, type2->enum_.type); // !! check for incomplete

	case LT_CT_STRUCT:
	case LT_CT_UNION:
		if (!lt_lstr_eq(type1->struct_.tag, type2->struct_.tag))
			return 0;

		if (struct_is_complete(type1) != struct_is_complete(type2))
			return 1;

		if (lt_darr_count(type1->struct_.member_types) != lt_darr_count(type2->struct_.member_types))
			return 0;
		for (usz i = 0; i < lt_darr_count(type1->struct_.member_types); ++i) {
			if (!type_eq(type1->struct_.member_types[i], type2->struct_.member_types[i]))
				return 0;
			if (!lt_lstr_eq(type1->struct_.member_names[i], type2->struct_.member_names[i]))
				return 0;
		}
		return 1;

	case LT_CT_CHAR:
	case LT_CT_UCHAR:
	case LT_CT_SHORT:
	case LT_CT_USHORT:
	case LT_CT_INT:
	case LT_CT_UINT:
	case LT_CT_LONG:
	case LT_CT_ULONG:
	case LT_CT_FLOAT:
	case LT_CT_DOUBLE:
	case LT_CT_BOOL:
	case LT_CT_VOID:
		return 1;
	}

	return 0;
}

#define sym_is_equal(sym, key) (lt_lstr_eq((sym)->name, key))
#define sym_is_lesser(sym, key) (lt_lstr_cmp((sym)->name, key) < 0)

static
LT_DEFINE_BINARY_SEARCH_FUNC(lt_c_sym_t*, lstr_t, lookup_sym, sym_is_lesser, sym_is_equal);

static
LT_DEFINE_BINARY_SEARCH_NEAREST_FUNC(lt_c_sym_t*, lstr_t, lookup_nearest_sym, sym_is_lesser, sym_is_equal);

static
b8 syms_compatible(lt_c_sym_t* sym1, lt_c_sym_t* sym2) {
	if (sym1->type != LT_CSYM_TYPEDEF || sym2->type != LT_CSYM_TYPEDEF)
		return 0;
	return type_eq(sym1->typedef_.type, sym2->typedef_.type);
}

static
lt_err_t insert_sym(lt_c_parse_ctx_t* cx, lt_darr(lt_c_sym_t*)* arr, lt_c_sym_t* sym) {
	if (*arr == NULL)
		*arr = lt_darr_create(lt_c_sym_t*, 4, &cx->alloc->interf);

	usz glob_count = lt_darr_count(*arr);
	lt_c_sym_t** at = lookup_nearest_sym(*arr, glob_count, sym->name);

	if (at < *arr + glob_count && sym_is_equal(*at, sym->name)) {
		if (syms_compatible(sym, *at))
			return LT_SUCCESS;
		fail("multiple definitions of `%S`", sym->name);
	}

	usz idx = at - *arr;
	lt_darr_insert(*arr, idx, &sym, 1);
	return LT_SUCCESS;
}

static
lt_err_t insert_struct_incomplete(lt_c_parse_ctx_t* cx, lstr_t tag, lt_c_sym_t** out_sym) {
	if (cx->syms->structs == NULL)
		cx->syms->structs = lt_darr_create(lt_c_sym_t*, 4, &cx->alloc->interf);

	usz glob_count = lt_darr_count(cx->syms->structs);
	lt_c_sym_t** at = lookup_nearest_sym(cx->syms->structs, glob_count, tag);

	if (at < cx->syms->structs + glob_count && sym_is_equal(*at, tag)) {
		*out_sym = *at;
		return LT_SUCCESS;
	}

	lt_c_type_t* type = lt_amalloc_lean(cx->alloc, sizeof(lt_c_type_t));
	if (!type)
		return LT_ERR_OUT_OF_MEMORY;
	*type = (lt_c_type_t) {
			.type = LT_CT_STRUCT,
			.struct_.tag = tag };

	lt_c_sym_t* sym = lt_amalloc_lean(cx->alloc, sizeof(lt_c_sym_t));
	if (!sym)
		return LT_ERR_OUT_OF_MEMORY;
	*sym = (lt_c_sym_t) {
			.type = LT_CSYM_TAG,
			.name = tag,
			.tag.type = type };

	usz idx = at - cx->syms->structs;
	lt_darr_insert(cx->syms->structs, idx, &sym, 1);
	*out_sym = sym;
	return LT_SUCCESS;
}

static
lt_err_t insert_union_incomplete(lt_c_parse_ctx_t* cx, lstr_t tag, lt_c_sym_t** out_sym) {
	if (cx->syms->unions == NULL)
		cx->syms->unions = lt_darr_create(lt_c_sym_t*, 4, &cx->alloc->interf);

	usz glob_count = lt_darr_count(cx->syms->unions);
	lt_c_sym_t** at = lookup_nearest_sym(cx->syms->unions, glob_count, tag);

	if (at < cx->syms->unions + glob_count && sym_is_equal(*at, tag)) {
		*out_sym = *at;
		return LT_SUCCESS;
	}

	lt_c_type_t* type = lt_amalloc_lean(cx->alloc, sizeof(lt_c_type_t));
	if (!type)
		return LT_ERR_OUT_OF_MEMORY;
	*type = (lt_c_type_t) {
			.type = LT_CT_UNION,
			.struct_.tag = tag };

	lt_c_sym_t* sym = lt_amalloc_lean(cx->alloc, sizeof(lt_c_sym_t));
	if (!sym)
		return LT_ERR_OUT_OF_MEMORY;
	*sym = (lt_c_sym_t) {
			.type = LT_CSYM_TAG,
			.name = tag,
			.tag.type = type };

	usz idx = at - cx->syms->unions;
	lt_darr_insert(cx->syms->unions, idx, &sym, 1);
	*out_sym = sym;
	return LT_SUCCESS;
}

static
lt_err_t insert_enum_incomplete(lt_c_parse_ctx_t* cx, lstr_t tag, lt_c_sym_t** out_sym) {
	if (cx->syms->enums == NULL)
		cx->syms->enums = lt_darr_create(lt_c_sym_t*, 4, &cx->alloc->interf);

	usz glob_count = lt_darr_count(cx->syms->enums);
	lt_c_sym_t** at = lookup_nearest_sym(cx->syms->enums, glob_count, tag);

	if (at < cx->syms->enums + glob_count && sym_is_equal(*at, tag)) {
		*out_sym = *at;
		return LT_SUCCESS;
	}

	lt_c_type_t* type = lt_amalloc_lean(cx->alloc, sizeof(lt_c_type_t));
	if (!type)
		return LT_ERR_OUT_OF_MEMORY;
	*type = (lt_c_type_t) {
			.type = LT_CT_ENUM,
			.enum_.tag = tag };

	lt_c_sym_t* sym = lt_amalloc_lean(cx->alloc, sizeof(lt_c_sym_t));
	if (!sym)
		return LT_ERR_OUT_OF_MEMORY;
	*sym = (lt_c_sym_t) {
			.type = LT_CSYM_TAG,
			.name = tag,
			.tag.type = type };

	usz idx = at - cx->syms->enums;
	lt_darr_insert(cx->syms->enums, idx, &sym, 1);
	*out_sym = sym;
	return LT_SUCCESS;
}

static
b8 enum_is_complete(lt_c_type_t* type) {
	return type->enum_.type != NULL;
}

static
lt_err_t insert_func_incomplete(lt_c_parse_ctx_t* cx, lstr_t name, lt_c_type_t* type, lt_c_sym_t** out_sym) {
	if (cx->syms->symbols == NULL)
		cx->syms->symbols = lt_darr_create(lt_c_sym_t*, 4, &cx->alloc->interf);

	usz glob_count = lt_darr_count(cx->syms->symbols);
	lt_c_sym_t** at = lookup_nearest_sym(cx->syms->symbols, glob_count, name);

	if (at < cx->syms->symbols + glob_count && sym_is_equal(*at, name)) {
		if ((*at)->type != LT_CSYM_FUNC || !type_eq((*at)->func.type, type))
			fail("multiple definitions of `%S`", name);
		*out_sym = *at;
		return LT_SUCCESS;
	}

	lt_c_sym_t* sym = lt_amalloc_lean(cx->alloc, sizeof(lt_c_sym_t));
	if (!sym)
		return LT_ERR_OUT_OF_MEMORY;
	*sym = (lt_c_sym_t) {
			.type = LT_CSYM_FUNC,
			.name = name,
			.func.type = type };

	usz idx = at - cx->syms->symbols;
	lt_darr_insert(cx->syms->symbols, idx, &sym, 1);
	*out_sym = sym;
	return LT_SUCCESS;
}

static
b8 func_is_complete(lt_c_sym_t* sym) {
	return sym->func.body != NULL;
}

static
lt_c_sym_t* find_sym(lt_c_symtab_t* symtab, lstr_t name) {
	for (; symtab != NULL; symtab = symtab->parent) {
		if (symtab->symbols == NULL)
			continue;

		lt_c_sym_t** psym = lookup_sym(symtab->symbols, lt_darr_count(symtab->symbols), name);
		if (psym)
			return *psym;
	}
	return NULL;
}

static
lt_c_sym_t* find_union(lt_c_symtab_t* symtab, lstr_t name) {
	for (; symtab != NULL; symtab = symtab->parent) {
		if (symtab->unions == NULL)
			continue;

		lt_c_sym_t** psym = lookup_sym(symtab->unions, lt_darr_count(symtab->unions), name);
		if (psym)
			return *psym;
	}
	return NULL;
}

static
lt_c_sym_t* find_struct(lt_c_symtab_t* symtab, lstr_t name) {
	for (; symtab != NULL; symtab = symtab->parent) {
		if (symtab->structs == NULL)
			continue;

		lt_c_sym_t** psym = lookup_sym(symtab->structs, lt_darr_count(symtab->structs), name);
		if (psym)
			return *psym;
	}
	return NULL;
}

static
lt_c_sym_t* find_enum(lt_c_symtab_t* symtab, lstr_t name) {
	for (; symtab != NULL; symtab = symtab->parent) {
		if (symtab->enums == NULL)
			continue;

		lt_c_sym_t** psym = lookup_sym(symtab->enums, lt_darr_count(symtab->enums), name);
		if (psym)
			return *psym;
	}
	return NULL;
}

static lt_err_t parse_new_stmt(lt_c_parse_ctx_t* cx, lt_c_stmt_t** out);
static lt_err_t parse_new_expr(lt_c_parse_ctx_t* cx, b8 allow_comma, lt_c_expr_t** out);
static lt_err_t parse_decl(lt_c_parse_ctx_t* cx, u8 decl_type, lt_darr(lt_c_type_t*)* out_types, lt_darr(lstr_t)* out_names);

#define ALL_TYPE_KEYWORDS \
	LT_CTK_KW_ATOMIC: \
	case LT_CTK_KW_ALIGNAS: \
	case LT_CTK_KW_BITINT: \
	case LT_CTK_KW_DECIMAL32: case LT_CTK_KW_DECIMAL64: case LT_CTK_KW_DECIMAL128: \
	case LT_CTK_KWSIGNED: case LT_CTK_KWUNSIGNED: \
	case LT_CTK_KWVOLATILE: \
	case LT_CTK_KWCONST: \
	case LT_CTK_KWCHAR: case LT_CTK_KWSHORT: case LT_CTK_KWINT: case LT_CTK_KWLONG: \
	case LT_CTK_KWFLOAT: case LT_CTK_KWDOUBLE: \
	case LT_CTK_KWBOOL: case LT_CTK_KW_BOOL: \
	case LT_CTK_KWVOID: \
	case LT_CTK_KWENUM: case LT_CTK_KWSTRUCT: case LT_CTK_KWUNION

b8 type_pending(lt_c_parse_ctx_t* cx) {
	switch (read_type(cx)) {
	case ALL_TYPE_KEYWORDS:
		return 1;

	case LT_CTK_IDENT: {
		lt_c_tk_t ident_tk = *cx->it;
		lstr_t name = str_from_tk(cx, ident_tk);

		lt_c_sym_t* sym = find_sym(cx->syms, name);
		if (!sym || sym->type != LT_CSYM_TYPEDEF)
			return 0;
	}	return 1;

	default:
		return 0;
	}
}

#define ALL_STORAGE_KEYWORDS \
	LT_CTK_KWSTATIC: \
	case LT_CTK_KWTYPEDEF: \
	case LT_CTK_KWEXTERN: \
	case LT_CTK_KWTHREAD_LOCAL: case LT_CTK_KW_THREAD_LOCAL: \
	case LT_CTK_KWREGISTER

b8 storage_pending(lt_c_parse_ctx_t* cx) {
	switch (read_type(cx)) {
	case ALL_STORAGE_KEYWORDS:
		return 1;

	default:
		return 0;
	}
}

static
lt_err_t parse_compound_literal(lt_c_parse_ctx_t* cx, lt_c_type_t* type, lt_c_expr_t** out) {
	lt_err_t err;

	if ((err = consume_type(cx, LT_CTK_LBRACE, NULL)))
		return err;

	lt_c_expr_t* root = NULL;
	lt_c_expr_t** init_end = &root;

	while (read_type(cx) != LT_CTK_RBRACE) {
		if (read_type(cx) == LT_CTK_DOT)
			fail("named fields for compound literals are not supported");
		else {
			if ((err = parse_new_expr(cx, 0, init_end)))
				return err;
			init_end = &(*init_end)->next;
		}

		if (read_type(cx) != LT_CTK_RBRACE && (err = consume_type(cx, LT_CTK_COMMA, NULL)))
			return err;
	}
	consume(cx, NULL);
	return LT_SUCCESS;
}

static
lt_err_t parse_members(lt_c_parse_ctx_t* cx, lt_c_type_t* type) {
	lt_err_t err;

	if ((err = consume_type(cx, LT_CTK_LBRACE, NULL)))
		return err;

	lt_darr(lt_c_type_t*) member_types = lt_darr_create(lt_c_type_t*, 8, &cx->alloc->interf);
	lt_darr(lstr_t) member_names = lt_darr_create(lstr_t, 8, &cx->alloc->interf);
	if (!member_types || !member_names)
		return LT_ERR_OUT_OF_MEMORY;

	while (read_type(cx) != LT_CTK_RBRACE) {
		if ((err = parse_decl(cx, LT_CDECL_MEMBER, &member_types, &member_names)))
			return err;
	}
	consume(cx, NULL);

	type->struct_.member_types = member_types;
	type->struct_.member_names = member_names;
	return LT_SUCCESS;
}


static
lt_err_t parse_enum_values(lt_c_parse_ctx_t* cx, lt_c_type_t* type) {
	lt_err_t err;

	if ((err = consume_type(cx, LT_CTK_LBRACE, NULL)))
		return err;

	lt_darr(lstr_t) value_names = lt_darr_create(lstr_t, 8, &cx->alloc->interf);
	lt_darr(lt_c_expr_t*) value_exprs = lt_darr_create(lt_c_expr_t*, 8, &cx->alloc->interf);
	if (!value_names || !value_exprs)
		return LT_ERR_OUT_OF_MEMORY;

	while (read_type(cx) != LT_CTK_RBRACE) {
		lt_c_tk_t ident_tk;
		if ((err = consume_type(cx, LT_CTK_IDENT, NULL)))
			return err;
		lstr_t name = str_from_tk(cx, ident_tk);

		lt_c_expr_t* expr = NULL;
		if (read_type(cx) == LT_CTK_EQ) {
			consume(cx, NULL);
			if ((err = parse_new_expr(cx, 0, &expr)))
				return err;
		}

		lt_darr_push(value_names, name);
		lt_darr_push(value_exprs, expr);

		if (read_type(cx) != LT_CTK_RBRACE && (err = consume_type(cx, LT_CTK_COMMA, NULL)))
			return err;
	}
	consume(cx, NULL);

	lt_c_type_t* new_type = lt_amalloc_lean(cx->alloc, sizeof(lt_c_type_t));
	if (!new_type)
		return LT_ERR_OUT_OF_MEMORY;
	*new_type = (lt_c_type_t) { .type = LT_CT_INT };

	type->enum_.type = new_type;
	type->enum_.value_names = value_names;
	type->enum_.value_exprs = value_exprs;
	return LT_SUCCESS;
}

static
lt_err_t parse_decl_ident(lt_c_parse_ctx_t* cx, u8 decl_type, lt_c_type_t* type_base, lt_c_type_t** out_root, lt_c_type_t*** out_insert, lstr_t* out_name) {
	lt_err_t err;

	lt_c_type_t* prefix_root = type_base;
	lt_c_type_t** prefix_end = &prefix_root;

	while (read_type(cx) == LT_CTK_ASTER) {
		consume(cx, NULL);
		lt_c_type_t* ptr = lt_amalloc_lean(cx->alloc, sizeof(lt_c_type_t));
		if (!ptr)
			return LT_ERR_OUT_OF_MEMORY;
		*ptr = (lt_c_type_t) { .type = LT_CT_PTR };
		*prefix_end = ptr;
		prefix_end = &ptr->ptr.child;
	}

	lt_c_type_t* parenth_root;
	lt_c_type_t** parenth_end = &parenth_root;

	if (read_type(cx) == LT_CTK_LPAREN) {
		consume(cx, NULL);

		lt_c_type_t** new_parenth_end;
		if ((err = parse_decl_ident(cx, decl_type, NULL, &parenth_root, &new_parenth_end, out_name)))
			return err;
		if ((err = consume_type(cx, LT_CTK_RPAREN, NULL)))
			return err;

		if (new_parenth_end)
			parenth_end = new_parenth_end;
	}
	else {
		if (read_type(cx) == LT_CTK_IDENT) {
			lt_c_tk_t ident_tk;
			if ((err = consume_type(cx, LT_CTK_IDENT, &ident_tk)))
				return err;
			*out_name = str_from_tk(cx, ident_tk);
		}
		else {
			*out_name = CLSTR("");
		}
	}

	lt_c_type_t* suffix_root;
	lt_c_type_t** suffix_end = &suffix_root;

	for (;;) {
		lt_c_tk_type_t tk = read_type(cx);

		if (tk == LT_CTK_LPAREN) {
			consume(cx, NULL);

			lt_darr(lt_c_type_t*) param_types = lt_darr_create(lt_c_type_t*, 4, &cx->alloc->interf);
			lt_darr(lstr_t) param_names = lt_darr_create(lstr_t, 4, &cx->alloc->interf);

			b8 first_param = 1;
			for (;;) {
				if (read_type(cx) == LT_CTK_RPAREN) {
					consume(cx, NULL);
					break;
				}

				if (!first_param && (err = consume_type(cx, LT_CTK_COMMA, NULL)))
					return err;
				first_param = 0;

				if (read_type(cx) == LT_CTK_TDOT) {
					consume(cx, NULL);
					if ((err = consume_type(cx, LT_CTK_RPAREN, NULL)))
						return err;
					break;
				}

				if ((err = parse_decl(cx, LT_CDECL_PARAM, &param_types, &param_names)))
					return err;
			}

			lt_c_type_t* func = lt_amalloc_lean(cx->alloc, sizeof(lt_c_type_t));
			if (!func)
				return LT_ERR_OUT_OF_MEMORY;
			*func = (lt_c_type_t) {
					.type = LT_CT_FUNC,
					.func.param_types = param_types,
					.func.param_names = param_names };
			*suffix_end = func;
			suffix_end = &func->func.return_type;
		}
		else if (tk == LT_CTK_LBRACKET) {
			consume(cx, NULL);

			lt_c_expr_t* size_expr = NULL;
			if (read_type(cx) != LT_CTK_RBRACKET && (err = parse_new_expr(cx, 1, &size_expr)))
				return err;

			if ((err = consume_type(cx, LT_CTK_RBRACKET, NULL)))
				return err;

			lt_c_type_t* array = lt_amalloc_lean(cx->alloc, sizeof(lt_c_type_t));
			if (!array)
				return LT_ERR_OUT_OF_MEMORY;
			*array = (lt_c_type_t) {
					.type = LT_CT_ARRAY,
					.array.count = size_expr };
			*suffix_end = array;
			suffix_end = &array->array.elem_type;
		}
		else {
			*prefix_end = type_base;
			*suffix_end = prefix_root;
			*parenth_end = suffix_root;

			*out_root = parenth_root;

			if (!out_insert)
				return LT_SUCCESS;

			if (prefix_end == &prefix_root)
				*out_insert = NULL;
			else
				*out_insert = prefix_end;
			return LT_SUCCESS;
		}
	}
}

static
lt_err_t parse_decl(lt_c_parse_ctx_t* cx, u8 decl_type, lt_darr(lt_c_type_t*)* out_types, lt_darr(lstr_t)* out_names) {
	lt_err_t err;

	u8 inline_ = 0;

#define ST_DEFAULT		0
#define ST_STATIC		1
#define ST_EXTERN		2
#define ST_REGISTER		3
#define ST_TYPEDEF		4

	u8 storage_class = ST_DEFAULT;

	static lstr_t storage_class_str[6] = {
		CLSTRI("internal-error"),
		CLSTRI("static"),
		CLSTRI("extern"),
		CLSTRI("register"),
		CLSTRI("typedef"),
	};

#define C_MUTABLE	0
#define C_CONSTANT	1

	u8 mutability = C_MUTABLE;

#define V_NONE		0
#define V_VOLATILE	1

	u8 volatility = V_NONE;

#define S_DEFAULT	0
#define S_SIGNED	1
#define S_UNSIGNED	2

	u8 sign = S_DEFAULT;

	static lstr_t sign_str[3] = {
		CLSTRI("internal-error"),
		CLSTRI("signed"),
		CLSTRI("unsigned"),
	};

#define L_DEFAULT	0
#define L_SHORT		1
#define L_LONG		2
#define L_LONG_LONG	3

	u8 length = L_DEFAULT;

	static lstr_t length_str[4] = {
		CLSTRI("internal-error"),
		CLSTRI("short"),
		CLSTRI("long"),
		CLSTRI("long long"),
	};

#define T_DEFAULT	0
#define T_BOOL		1
#define T_CHAR		2
#define T_INT		3
#define T_FLOAT		4
#define T_DOUBLE	5
#define T_TYPEDEF	6
#define T_UNION		7
#define T_STRUCT	8
#define T_ENUM		9
#define T_VOID		10

	u8 type = T_DEFAULT;
	lt_c_type_t* found_type = NULL;
	lstr_t typedef_name = NLSTR();

	static lstr_t type_str[11] = {
		CLSTRI("internal-error"),
		CLSTRI("bool"),
		CLSTRI("char"),
		CLSTRI("int"),
		CLSTRI("float"),
		CLSTRI("double"),
		CLSTRI("internal-error"),
		CLSTRI("union"),
		CLSTRI("struct"),
		CLSTRI("enum"),
	};

#define TYPE_STR(t) ((t) == T_TYPEDEF ? typedef_name : type_str[t])

	lstr_t decl_type_str[4] = {
		CLSTRI("declaration"),
		CLSTRI("function parameter"),
		CLSTRI("member declaration"),
		CLSTRI("type cast"),
	};

	for (;;) {
	next_specifier:
		lt_c_tk_type_t tk = read_type(cx);

		if (tk == LT_CTK_KWINLINE) {
			consume(cx, NULL);
			inline_ = 1;
			goto next_specifier;
		}

		if (tk == LT_CTK_KWVOLATILE) {
			if (volatility != V_NONE)
				fail("duplicate `volatile`");
			consume(cx, NULL);
			volatility = V_VOLATILE;
			goto next_specifier;
		}

		if (tk == LT_CTK_KWCONST) {
			if (mutability != C_MUTABLE)
				fail("duplicate `const`");
			consume(cx, NULL);
			mutability = C_CONSTANT;
			goto next_specifier;
		}

		if (tk == LT_CTK_KWSHORT) {
			if (length == L_SHORT)
				fail("duplicate `short`");
			if (length != L_DEFAULT)
				fail("both `%S` and `%S` present in %S", length_str[length], length_str[L_SHORT], decl_type_str[decl_type]);
			consume(cx, NULL);
			length = L_SHORT;
			goto next_specifier;
		}

		if (tk == LT_CTK_KWLONG) {
			if (length == L_LONG_LONG)
				fail("type too long");
			if (length != L_DEFAULT && length != L_LONG)
				fail("both `%S` and `%S` present in %S", length_str[length], length_str[L_LONG], decl_type_str[decl_type]);
			consume(cx, NULL);

			if (length == L_LONG)
				length = L_LONG_LONG;
			else
				length = L_LONG;
			goto next_specifier;
		}

		if (tk == LT_CTK_IDENT) {
			if (type != T_DEFAULT || sign != S_DEFAULT || length != L_DEFAULT)
				break;

			lt_c_tk_t ident_tk = *cx->it;
			lstr_t name = str_from_tk(cx, ident_tk);

			lt_c_sym_t* sym = find_sym(cx->syms, name);
			if (!sym)
				fail("use of undeclared identifier `%S`", name);
			if (sym->type != LT_CSYM_TYPEDEF)
				fail("unexpected `%S` in %S", name, decl_type_str[decl_type]);

			consume(cx, NULL);
			type = T_TYPEDEF;
			found_type = sym->typedef_.type;
			typedef_name = name;

			goto next_specifier;
		}

		if (tk == LT_CTK_KWSTRUCT) {
			if (type == T_STRUCT)
				fail("duplicate `struct`");
			if (type != T_DEFAULT)
				fail("both `%S` and `struct` present in %S", TYPE_STR(type), decl_type_str[decl_type]);
			consume(cx, NULL);

			type = T_STRUCT;

			lt_c_sym_t* sym = NULL;

			lstr_t tag = NLSTR();
			if (read_type(cx) == LT_CTK_IDENT) {
				tag = str_from_tk(cx, *cx->it);
				consume(cx, NULL);
				sym = find_struct(cx->syms, tag);
				if (!sym && (err = insert_struct_incomplete(cx, tag, &sym)))
					return err;
				found_type = sym->tag.type;
			}

			if (read_type(cx) != LT_CTK_LBRACE)
				goto next_specifier;

			if (sym) {
				if (struct_is_complete(sym->tag.type)) // !! dont fail if definitions are equal
					fail("multiple definitions of `struct %S`", tag);
				if ((err = parse_members(cx, sym->tag.type)))
					return err;
				found_type = sym->tag.type;
			}
			else {
				lt_c_type_t* new_type = lt_amalloc_lean(cx->alloc, sizeof(lt_c_type_t));
				if (!new_type)
					return LT_ERR_OUT_OF_MEMORY;
				*new_type = (lt_c_type_t) { .type = LT_CT_STRUCT };
				if ((err = parse_members(cx, new_type)))
					return err;
				found_type = new_type;
			}
			goto next_specifier;
		}

		if (tk == LT_CTK_KWUNION) {
			if (type == T_UNION)
				fail("duplicate `union`");
			if (type != T_DEFAULT)
				fail("both `%S` and `union` present in %S", TYPE_STR(type), decl_type_str[decl_type]);
			consume(cx, NULL);

			type = T_UNION;

			lt_c_sym_t* sym = NULL;

			lstr_t tag = NLSTR();
			if (read_type(cx) == LT_CTK_IDENT) {
				tag = str_from_tk(cx, *cx->it);
				consume(cx, NULL);
				sym = find_union(cx->syms, tag);
				if (!sym && (err = insert_union_incomplete(cx, tag, &sym)))
					return err;
				found_type = sym->tag.type;
			}

			if (read_type(cx) != LT_CTK_LBRACE)
				goto next_specifier;

			if (sym) {
				if (struct_is_complete(sym->tag.type))
					fail("multiple definitions of `union %S`", tag);
				if ((err = parse_members(cx, sym->tag.type)))
					return err;
				found_type = sym->tag.type;
			}
			else {
				lt_c_type_t* new_type = lt_amalloc_lean(cx->alloc, sizeof(lt_c_type_t));
				if (!new_type)
					return LT_ERR_OUT_OF_MEMORY;
				*new_type = (lt_c_type_t) { .type = LT_CT_UNION };
				if ((err = parse_members(cx, new_type)))
					return err;
				found_type = new_type;
			}
			goto next_specifier;
		}

		if (tk == LT_CTK_KWENUM) {
			if (type == T_ENUM)
				fail("duplicate `enum`");
			if (type != T_DEFAULT)
				fail("both `%S` and `enum` present in %S", TYPE_STR(type), decl_type_str[decl_type]);
			consume(cx, NULL);

			type = T_ENUM;

			lt_c_sym_t* sym = NULL;

			lstr_t tag = NLSTR();
			if (read_type(cx) == LT_CTK_IDENT) {
				tag = str_from_tk(cx, *cx->it);
				consume(cx, NULL);
				sym = find_struct(cx->syms, tag);
				if (!sym && (err = insert_enum_incomplete(cx, tag, &sym)))
					return err;
				found_type = sym->tag.type;
			}

			if (read_type(cx) != LT_CTK_LBRACE)
				goto next_specifier;

			if (sym) {
				if (enum_is_complete(sym->tag.type)) // !! dont fail if definitions are equal
					fail("multiple definitions of `enum %S`", tag);
				if ((err = parse_enum_values(cx, sym->tag.type)))
					return err;
				found_type = sym->tag.type;
			}
			else {
				lt_c_type_t* new_type = lt_amalloc_lean(cx->alloc, sizeof(lt_c_type_t));
				if (!new_type)
					return LT_ERR_OUT_OF_MEMORY;
				*new_type = (lt_c_type_t) { .type = LT_CT_ENUM };
				if ((err = parse_enum_values(cx, new_type)))
					return err;
				found_type = new_type;
			}
			goto next_specifier;
		}

		static const isz storage_class_tks[] = { LT_CTK_KWSTATIC, LT_CTK_KWEXTERN, LT_CTK_KWREGISTER, LT_CTK_KWTYPEDEF };
		static const isz storage_classes[] = { ST_STATIC, ST_EXTERN, ST_REGISTER, ST_TYPEDEF };
		for (usz i = 0; i < sizeof(storage_classes) / sizeof(*storage_classes); ++i) {
			if (tk != storage_class_tks[i])
				continue;
			consume(cx, NULL);

			if (decl_type != LT_CDECL_DECL)
				fail("cannot specify storage class for %S", decl_type_str[decl_type]);

			if (storage_class == storage_classes[i])
				fail("duplicate `%S`", storage_class_str[storage_class]);
			if (storage_class != ST_DEFAULT)
				fail("both `%S` and `%S` present in %S", storage_class_str[storage_class], storage_class_str[storage_classes[i]], decl_type_str[decl_type]);

			storage_class = storage_classes[i];
			goto next_specifier;
		}

		static const isz sign_tks[] = { LT_CTK_KWSIGNED, LT_CTK_KWUNSIGNED };
		static const isz signs[] = { S_SIGNED, S_UNSIGNED };
		for (usz i = 0; i < sizeof(signs) / sizeof(*signs); ++i) {
			if (tk != sign_tks[i])
				continue;
			consume(cx, NULL);

			if (sign == signs[i])
				fail("duplicate `%S`", sign_str[sign]);
			if (sign != T_DEFAULT)
				fail("both `%S` and `%S` present in %S", sign_str[sign], sign_str[signs[i]], decl_type_str[decl_type]);

			sign = signs[i];
			goto next_specifier;
		}

		static const isz type_tks[] = { LT_CTK_KWBOOL, LT_CTK_KWCHAR, LT_CTK_KWINT, LT_CTK_KWFLOAT, LT_CTK_KWDOUBLE, LT_CTK_KWVOID };
		static const isz types[] = { T_BOOL, T_CHAR, T_INT, T_FLOAT, T_DOUBLE, T_VOID };
		for (usz i = 0; i < sizeof(types) / sizeof(*types); ++i) {
			if (tk != type_tks[i])
				continue;
			consume(cx, NULL);

			if (type == types[i])
				fail("duplicate `%S`", type_str[type]);
			if (type != T_DEFAULT)
				fail("both `%S` and `%S` present in %S", TYPE_STR(type), TYPE_STR(types[i]), decl_type_str[decl_type]);

			type = types[i];
			goto next_specifier;
		}

		break;
	}

	lt_c_data_type_t dtype;
	lt_c_type_t* base_type;

	if (found_type) {
		if (sign != S_DEFAULT)
			fail("both `%S` and `%S` present in %S", sign_str[sign], TYPE_STR(type), decl_type_str[decl_type]);
		if (length != L_DEFAULT)
			fail("both `%S` and `%S` present in %S", length_str[length], TYPE_STR(type), decl_type_str[decl_type]);

		base_type = found_type;
		goto parse_ident;
	}

	if (length == L_DEFAULT && type == T_DEFAULT && sign == S_DEFAULT) {
		if (read_type(cx) == LT_CTK_EOF)
			fail("unexpected end-of-file, expected data type");
		fail("unexpected `%S`, expected data type", lt_c_tk_type_str(read_type(cx)));
	}

	base_type = lt_amalloc_lean(cx->alloc, sizeof(lt_c_type_t));
	if (!base_type)
		return LT_ERR_OUT_OF_MEMORY;

	switch (type) {
	case T_DEFAULT:
	case T_INT:
		if (length == L_SHORT)
			dtype = LT_CT_SHORT;
		else if (length == L_DEFAULT)
			dtype = LT_CT_INT;
		else if (length == L_LONG || length == L_LONG_LONG)
			dtype = LT_CT_LONG;
		else
			LT_ASSERT_NOT_REACHED();

		if (sign == S_UNSIGNED)
			dtype += LT_CT_UNSIGNED;

		*base_type = (lt_c_type_t) { .type = dtype };
		break;

	case T_CHAR:
		dtype = LT_CT_CHAR;
		if (sign == S_UNSIGNED)
			dtype += LT_CT_UNSIGNED;
		goto check_type_length_incompatible;

	case T_DOUBLE:
		dtype = LT_CT_DOUBLE;
		if (sign != S_DEFAULT)
			fail("both `%S` and `%S` present in %S", sign_str[sign], type_str[type], decl_type_str[decl_type]);
		if (length != L_DEFAULT && length != L_LONG)
			fail("both `%S` and `%S` present in %S", length_str[length], type_str[type], decl_type_str[decl_type]);
		break;

	case T_BOOL:
		dtype = LT_CT_BOOL;
		goto check_type_incompatible;
	case T_FLOAT:
		dtype = LT_CT_FLOAT;
		goto check_type_incompatible;
	case T_VOID:
		dtype = LT_CT_VOID;
		goto check_type_incompatible;

check_type_incompatible:
		if (sign != S_DEFAULT)
			fail("both `%S` and `%S` present in %S", sign_str[sign], type_str[type], decl_type_str[decl_type]);
check_type_length_incompatible:
		if (length != L_DEFAULT)
			fail("both `%S` and `%S` present in %S", length_str[length], type_str[type], decl_type_str[decl_type]);
		*base_type = (lt_c_type_t) { .type = dtype };
		break;

	default:
		LT_ASSERT_NOT_REACHED();
		fail("internal error"); // only exists to get rid of a warning
	}

parse_ident:

	if (read_type(cx) == LT_CTK_SEMICOLON) {
		if (decl_type == LT_CDECL_DECL &&
			((base_type->type == LT_CT_STRUCT && base_type->struct_.tag.len) ||
			(base_type->type == LT_CT_UNION && base_type->struct_.tag.len) ||
			(base_type->type == LT_CT_ENUM && base_type->enum_.tag.len)))
		{
			return LT_SUCCESS;
		}
		if (decl_type == LT_CDECL_MEMBER && base_type->type != LT_CT_STRUCT && base_type->type != LT_CT_UNION)
			return LT_SUCCESS;
	}

	extern int asdf();

	for (;;) {
		lt_c_type_t* type;
		lstr_t name;
		if ((err = parse_decl_ident(cx, decl_type, base_type, &type, NULL, &name)))
			return err;

		b8 is_function = (type->type == LT_CT_FUNC) && storage_class != ST_TYPEDEF;

		if (is_function) {
			if (decl_type != LT_CDECL_DECL)
				fail("function declaration in %S", decl_type_str[decl_type]);
			if (storage_class == ST_REGISTER)
				fail("function declared with storage class `%S`", storage_class_str[storage_class]);
		}
		else {
			if (inline_)
				fail("non-function declared declared `inline`");
		}

		if (decl_type == LT_CDECL_DECL) {
			lt_darr_push(*out_types, type);
			lt_darr_push(*out_names, name);

			lt_c_expr_t* initializer = NULL;

			if (is_function) {
				lt_c_sym_t* sym;
				if ((err = insert_func_incomplete(cx, name, type, &sym)))
					return err;

				if (read_type(cx) == LT_CTK_LBRACE) {
					consume(cx, NULL);

					if (storage_class == ST_EXTERN)
						fail("function definition with storage class `%S`", storage_class_str[storage_class]);

					PUSH_SCOPE()

					lt_c_stmt_t* root = NULL;
					lt_c_stmt_t** body_end = &root;
					while (read_type(cx) != LT_CTK_RBRACE) {
						if ((err = parse_new_stmt(cx, body_end)))
							return err;
						body_end = &(*body_end)->next;
					}
					consume(cx, NULL);

					POP_SCOPE()

					if (func_is_complete(sym))
						fail("multiple definitions of `%S`", name);
					sym->func.body = root;
				}
				else {
					if ((err = consume_type(cx, LT_CTK_SEMICOLON, NULL)))
						return err;
				}

				return LT_SUCCESS;
			}

			if (read_type(cx) == LT_CTK_EQ) {
				consume(cx, NULL);
				if ((err = parse_new_expr(cx, 0, &initializer)))
					return err;
			}

			if ((storage_class == ST_TYPEDEF || storage_class == ST_EXTERN) && initializer)
				fail("cannot initialize storage class `%S`", storage_class_str[storage_class]);

			lt_c_sym_t* new_sym = lt_amalloc_lean(cx->alloc, sizeof(lt_c_sym_t));
			if (!new_sym)
				return LT_ERR_OUT_OF_MEMORY;

			if (storage_class == ST_TYPEDEF) {
				lt_c_type_t* typedef_type = lt_amalloc_lean(cx->alloc, sizeof(lt_c_type_t));
				if (!typedef_type)
					return LT_ERR_OUT_OF_MEMORY;

				*typedef_type = (lt_c_type_t) {
						.type = LT_CT_TYPEDEF,
						.typedef_.child = type };

				*new_sym = (lt_c_sym_t) {
						.type = LT_CSYM_TYPEDEF,
						.name = name,
						.typedef_.type = typedef_type };

				if ((err = insert_sym(cx, &cx->syms->symbols, new_sym)))
					return err;
			}
			else {
				*new_sym = (lt_c_sym_t) {
						.type = LT_CSYM_VALUE,
						.name = name,
						.value.type = type,
						.value.initializer = initializer };

				if ((err = insert_sym(cx, &cx->syms->symbols, new_sym)))
					return err;
			}
		}
		else if (decl_type == LT_CDECL_MEMBER) {
			if (read_type(cx) == LT_CTK_COLON) {
				consume(cx, NULL);

				lt_c_expr_t* bitfield_width;
				if ((err = parse_new_expr(cx, 0, &bitfield_width)))
					return err;
			}
		}
		else if (decl_type == LT_CDECL_PARAM) {
			**out_types = type;
			**out_names = name;
			return LT_SUCCESS;
		}
		else if (decl_type == LT_CDECL_CAST) {
			**out_types = type;
			return LT_SUCCESS;
		}

		if ((decl_type == LT_CDECL_DECL || decl_type == LT_CDECL_MEMBER) && read_type(cx) == LT_CTK_SEMICOLON) {
			consume(cx, NULL);
			return LT_SUCCESS;
		}
		if ((err = consume_type(cx, LT_CTK_COMMA, NULL)))
			return err;
	}
}

static
lt_err_t parse_new_nondecl_stmt(lt_c_parse_ctx_t* cx, lt_c_stmt_t** out) {
	lt_err_t err;

	*out = lt_amalloc_lean(cx->alloc, sizeof(lt_c_stmt_t));
	if (!*out)
		return LT_ERR_OUT_OF_MEMORY;

	switch (read_type(cx)) {
	case LT_CTK_KWGOTO:
		consume(cx, NULL);

		lt_c_tk_t label;
		if ((err = consume_type(cx, LT_CTK_IDENT, &label)))
			return err;
		if ((err = consume_type(cx, LT_CTK_SEMICOLON, NULL)))
			return err;

		**out = (lt_c_stmt_t) {
				.type = LT_CS_GOTO,
				.goto_.label = str_from_tk(cx, label) };
		return LT_SUCCESS;

	case LT_CTK_SEMICOLON:
		consume(cx, NULL);
		**out = (lt_c_stmt_t) { .type = LT_CS_EMPTY, };
		return LT_SUCCESS;

	case LT_CTK_KWRETURN:
		consume(cx, NULL);
		**out = (lt_c_stmt_t) { .type = LT_CS_RETURN };
		if (read_type(cx) != LT_CTK_SEMICOLON && (err = parse_new_expr(cx, 1, &(*out)->return_.value)))
			return err;
		if ((err = consume_type(cx, LT_CTK_SEMICOLON, NULL)))
			return err;
		return LT_SUCCESS;

	case LT_CTK_KWIF:
		consume(cx, NULL);
		if ((err = consume_type(cx, LT_CTK_LPAREN, NULL)))
			return err;

		**out = (lt_c_stmt_t) { .type = LT_CS_IF };

		if ((err = parse_new_expr(cx, 1, &(*out)->if_.cond)))
			return err;
		if ((err = consume_type(cx, LT_CTK_RPAREN, NULL)))
			return err;
		if ((err = parse_new_stmt(cx, &(*out)->if_.then)))
			return err;
		if (read_type(cx) == LT_CTK_KWELSE) {
			consume(cx, 0);
			if ((err = parse_new_nondecl_stmt(cx, &(*out)->if_.else_)))
				return err;
		}
		return LT_SUCCESS;

	case LT_CTK_KWWHILE:
		consume(cx, NULL);
		if ((err = consume_type(cx, LT_CTK_LPAREN, NULL)))
			return err;

		**out = (lt_c_stmt_t) { .type = LT_CS_WHILE };

		if ((err = parse_new_expr(cx, 1, &(*out)->while_.cond)))
			return err;
		if ((err = consume_type(cx, LT_CTK_RPAREN, NULL)))
			return err;

		if ((err = parse_new_nondecl_stmt(cx, &(*out)->while_.do_)))
			return err;

		return LT_SUCCESS;

	case LT_CTK_KWFOR: {
		consume(cx, NULL);
		if ((err = consume_type(cx, LT_CTK_LPAREN, NULL)))
			return err;

		**out = (lt_c_stmt_t) { .type = LT_CS_FOR };

		PUSH_SCOPE()

		if (read_type(cx) == LT_CTK_SEMICOLON) {
			consume(cx, NULL);
		}
		else if (type_pending(cx)) {
			lt_darr(lt_c_type_t*) types = lt_darr_create(lt_c_type_t*, 4, &cx->alloc->interf);
			lt_darr(lstr_t) names = lt_darr_create(lstr_t, 4, &cx->alloc->interf);

			if ((err = parse_decl(cx, LT_CDECL_DECL, &types, &names)))
				return err;
		}
		else {
			lt_c_stmt_t* begin = lt_amalloc_lean(cx->alloc, sizeof(lt_c_stmt_t));
			if (!begin)
				return LT_ERR_OUT_OF_MEMORY;
			*begin = (lt_c_stmt_t) { .type = LT_CS_EXPR };

			if ((err = parse_new_expr(cx, 1, &begin->expr.expr)))
				return err;
			if ((err = consume_type(cx, LT_CTK_SEMICOLON, NULL)))
				return err;
			(*out)->for_.begin = begin;
		}

		if (read_type(cx) == LT_CTK_SEMICOLON) {
			consume(cx, NULL);
		}
		else {
			if ((err = parse_new_expr(cx, 1, &(*out)->for_.cond)))
				return err;
			if ((err = consume_type(cx, LT_CTK_SEMICOLON, NULL)))
				return err;
		}

		if (read_type(cx) != LT_CTK_RPAREN) {
			if ((err = parse_new_expr(cx, 1, &(*out)->for_.iterate)))
				return err;
		}

		if ((err = consume_type(cx, LT_CTK_RPAREN, NULL)))
			return err;

		POP_SCOPE()
	}	return LT_SUCCESS;

	case LT_CTK_LBRACE: {
		consume(cx, NULL);
		PUSH_SCOPE()

		lt_c_stmt_t* root = NULL;
		lt_c_stmt_t** block_end = &root;
		while (read_type(cx) != LT_CTK_RBRACE) {
			if ((err = parse_new_stmt(cx, block_end)))
				return err;
			block_end = &(*block_end)->next;
		}
		consume(cx, NULL);

		**out = (lt_c_stmt_t) {
				.type = LT_CS_BLOCK,
				.block.children = root };

		POP_SCOPE()
	}	return LT_SUCCESS;

	case LT_CTK_IDENT:
	case LT_CTK_NUM: case LT_CTK_STR: case LT_CTK_CHAR: case LT_CTK_LPAREN: case LT_CTK_MINUS: case LT_CTK_ASTER: case LT_CTK_AMP:
	{
		**out = (lt_c_stmt_t) { .type = LT_CS_EXPR };

		if ((err = parse_new_expr(cx, 1, &(*out)->expr.expr)))
			return err;
		if ((err = consume_type(cx, LT_CTK_SEMICOLON, NULL)))
			return err;
		return LT_SUCCESS;
	}

	default:
		if (read_type(cx) == LT_CTK_EOF)
			fail("unexpected end-of-file");
		fail("unexpected token `%S`, expected statement", str_from_tk(cx, *cx->it));
	}
}

static
lt_err_t parse_new_stmt(lt_c_parse_ctx_t* cx, lt_c_stmt_t** out) {
	lt_err_t err;

	switch (read_type(cx)) {
	case LT_CTK_IDENT:
		if (type_pending(cx) || storage_pending(cx)) {

		case ALL_TYPE_KEYWORDS:
		case ALL_STORAGE_KEYWORDS:

			lt_darr(lt_c_type_t*) types = lt_darr_create(lt_c_type_t*, 4, &cx->alloc->interf);
			lt_darr(lstr_t) names = lt_darr_create(lstr_t, 4, &cx->alloc->interf);

			if ((err = parse_decl(cx, LT_CDECL_DECL, &types, &names)))
				return err;

			*out = lt_amalloc_lean(cx->alloc, sizeof(lt_c_stmt_t));
			if (!*out)
				return LT_ERR_OUT_OF_MEMORY;

			**out = (lt_c_stmt_t) {
					.type = LT_CS_DECL,
					.decl.types = types,
					.decl.names = names };
			return LT_SUCCESS;
		}
	default:
		return parse_new_nondecl_stmt(cx, out);
	}
}

static
lt_err_t parse_new_expr_primary(lt_c_parse_ctx_t* cx, lt_c_expr_t** out) {
	lt_err_t err;

	if (read_type(cx) == LT_CTK_LPAREN) {
		consume(cx, NULL);

		if ((err = consume_type(cx, LT_CTK_RPAREN, NULL)))
			return err;
		return LT_SUCCESS;
	}

	*out = lt_amalloc_lean(cx->alloc, sizeof(lt_c_expr_t));
	if (!*out)
		return LT_ERR_OUT_OF_MEMORY;

	switch (read_type(cx)) {
	case LT_CTK_IDENT:
		consume(cx, NULL);
		**out = (lt_c_expr_t) { .type = LT_CE_IDENT };
		return LT_SUCCESS;

	case LT_CTK_NUM:
		consume(cx, NULL);
		**out = (lt_c_expr_t) { .type = LT_CE_NUM };
		return LT_SUCCESS;

	case LT_CTK_STR:
		consume(cx, NULL);
		**out = (lt_c_expr_t) { .type = LT_CE_STR };
		return LT_SUCCESS;

	case LT_CTK_CHAR:
		consume(cx, NULL);
		**out = (lt_c_expr_t) { .type = LT_CE_NUM };
		return LT_SUCCESS;

	default:
		if (read_type(cx) == LT_CTK_EOF)
			fail("unexpected end-of-file");
		fail("unexpected token `%S`, expected expression", str_from_tk(cx, *cx->it));
	}
}

typedef
struct operator {
	isz precedence;
	u8 expr_type;
	u8 tk_type;
	u8 right_to_left;
} operator_t;

lt_err_t parse_new_expr_unary(lt_c_parse_ctx_t* cx, lt_c_expr_t** out) {
	lt_err_t err;

	static operator_t pfx_op_tab[] = {
		{ 2, LT_CE_PFX_INC, LT_CTK_DPLUS, 1 },
		{ 2, LT_CE_PFX_DEC, LT_CTK_DMINUS, 1 },
		{ 2, LT_CE_UPLUS, LT_CTK_PLUS, 1 },
		{ 2, LT_CE_LOGINV, LT_CTK_EXC, 1 },
		{ 2, LT_CE_INV, LT_CTK_TILDE, 1 },
		{ 2, LT_CE_CAST, LT_CTK_LPAREN, 1 },
		{ 2, LT_CE_DEREF, LT_CTK_ASTER, 1 },
		{ 2, LT_CE_REF, LT_CTK_AMP, 1 },
		{ 2, LT_CE_SIZEOF, LT_CTK_KWSIZEOF, 1 },
		{ 2, LT_CE_ALIGNOF, LT_CTK_KW_ALIGNOF, 1 },
		{ 2, LT_CE_ALIGNOF, LT_CTK_KWALIGNOF, 1 },
	};
	static usz pfx_op_count = sizeof(pfx_op_tab) / sizeof(*pfx_op_tab);

	lt_c_expr_t* root;
	lt_c_expr_t** prefix_end = &root, **last = NULL;

	lt_c_expr_t* suffix;

	for (;;) {
		lt_c_tk_type_t tk = read_type(cx);

		operator_t* op = NULL;
		for (usz i = 0; i < pfx_op_count; ++i) {
			if (pfx_op_tab[i].tk_type == tk)
				op = &pfx_op_tab[i];
		}

		if (!op)
			break;

		consume(cx, NULL);

		lt_c_expr_t* new = lt_amalloc_lean(cx->alloc, sizeof(lt_c_expr_t));
		if (!new)
			return LT_ERR_OUT_OF_MEMORY;

		if (op->expr_type == LT_CE_CAST) {
			if (!type_pending(cx)) {
				if ((err = parse_new_expr(cx, 1, &suffix)))
					return err;
				if ((err = consume_type(cx, LT_CTK_RPAREN, NULL)))
					return err;
				goto parse_suffix;
			}
			lt_c_type_t* to, **pto = &to;
			if ((err = parse_decl(cx, LT_CDECL_CAST, &pto, NULL))) // !! does not handle storage class specifiers for compound literals
				return err;
			if ((err = consume_type(cx, LT_CTK_RPAREN, NULL)))
				return err;

			*new = (lt_c_expr_t) {
					.type = op->expr_type,
					.cast.to = to };

			b8 is_compound = 0;
			if (read_type(cx) == LT_CTK_LBRACE) {
				is_compound = 1;
				if ((err = parse_compound_literal(cx, to, &new->unary.child)))
					return err;
			}

			if (last && ((*last)->type == LT_CE_SIZEOF || (*last)->type == LT_CE_ALIGNOF)) {
				suffix = *last;
				*last = NULL;
				goto parse_suffix;
			}
			else if (is_compound) {
				*prefix_end = NULL;
				suffix = new;
				goto parse_suffix;
			}

			*prefix_end = new;
			last = prefix_end;
			prefix_end = &new->cast.child;
		}
		else {
			*new = (lt_c_expr_t) { .type = op->expr_type };
			*prefix_end = new;
			last = prefix_end;
			prefix_end = &new->unary.child;
		}
	}

	if ((err = parse_new_expr_primary(cx, &suffix)))
		return err;

parse_suffix:
	for (;;) {
		lt_c_expr_t* new;
		lt_c_tk_t ident_tk;

		switch (read_type(cx)) {
		case LT_CTK_DPLUS:
			consume(cx, NULL);
			if (!(new = lt_amalloc_lean(cx->alloc, sizeof(lt_c_expr_t))))
				return LT_ERR_OUT_OF_MEMORY;
			*new = (lt_c_expr_t) {
					.type = LT_CE_SFX_INC,
					.unary.child = suffix };
			suffix = new;
			break;

		case LT_CTK_DMINUS:
			consume(cx, NULL);
			if (!(new = lt_amalloc_lean(cx->alloc, sizeof(lt_c_expr_t))))
				return LT_ERR_OUT_OF_MEMORY;
			*new = (lt_c_expr_t) {
					.type = LT_CE_SFX_DEC,
					.unary.child = suffix };
			suffix = new;
			break;

		case LT_CTK_LPAREN: {
			consume(cx, NULL);

			lt_c_expr_t* root = NULL;
			lt_c_expr_t** args_end = &root;

			while (read_type(cx) != LT_CTK_RPAREN) {
				if (args_end != &root && (err = consume_type(cx, LT_CTK_COMMA, NULL)))
					return err;
				if ((err = parse_new_expr(cx, 0, args_end)))
					return err;
				args_end = &(*args_end)->next;
			}
			consume(cx, NULL);

			if (!(new = lt_amalloc_lean(cx->alloc, sizeof(lt_c_expr_t))))
				return LT_ERR_OUT_OF_MEMORY;
			*new = (lt_c_expr_t) {
					.type = LT_CE_CALL,
					.call.func = suffix,
					.call.args = root };
			suffix = new;
		}	break;

		case LT_CTK_LBRACKET:
			consume(cx, NULL);

			if (!(new = lt_amalloc_lean(cx->alloc, sizeof(lt_c_expr_t))))
				return LT_ERR_OUT_OF_MEMORY;
			*new = (lt_c_expr_t) {
					.type = LT_CE_SUBSCRIPT,
					.subscript.array = suffix };
			suffix = new;

			if ((err = parse_new_expr(cx, 1, &new->subscript.index)))
				return err;
			if ((err = consume_type(cx, LT_CTK_RBRACKET, NULL)))
				return err;
			break;

		case LT_CTK_DOT:
			consume(cx, NULL);
			if ((err = consume_type(cx, LT_CTK_IDENT, &ident_tk)))
				return err;

			if (!(new = lt_amalloc_lean(cx->alloc, sizeof(lt_c_expr_t))))
				return LT_ERR_OUT_OF_MEMORY;
			*new = (lt_c_expr_t) {
					.type = LT_CE_MEMBER,
					.member.parent = suffix,
					.member.name = str_from_tk(cx, ident_tk) };
			suffix = new;
			break;

		case LT_CTK_ARROW:
			consume(cx, NULL);
			if ((err = consume_type(cx, LT_CTK_IDENT, &ident_tk)))
				return err;

			if (!(new = lt_amalloc_lean(cx->alloc, sizeof(lt_c_expr_t))))
				return LT_ERR_OUT_OF_MEMORY;
			*new = (lt_c_expr_t) {
					.type = LT_CE_MEMBER,
					.member.parent = suffix,
					.member.name = str_from_tk(cx, ident_tk) };
			suffix = new;
			break;

		default:
			*prefix_end = suffix;
			*out = root;
			return LT_SUCCESS;
		}
	}
}

static operator_t binary_op_tab[] = {
	{ 4, LT_CE_ADD, LT_CTK_PLUS, 0 },
	{ 4, LT_CE_SUB, LT_CTK_MINUS, 0 },
	{ 3, LT_CE_DIV, LT_CTK_SLASH, 0 },
	{ 3, LT_CE_MUL, LT_CTK_ASTER, 0 },
	{ 3, LT_CE_MOD, LT_CTK_PERCENT, 0 },
	{ 10, LT_CE_OR, LT_CTK_PIPE, 0 },
	{ 8, LT_CE_AND, LT_CTK_AMP, 0 },
	{ 9, LT_CE_XOR, LT_CTK_CARET, 0 },
	{ 5, LT_CE_SHL, LT_CTK_DLESSER, 0 },
	{ 5, LT_CE_SHR, LT_CTK_DGREATER, 0 },

	{ 14, LT_CE_SET, LT_CTK_EQ, 1 },
	{ 14, LT_CE_SETADD, LT_CTK_PLUSEQ, 1 },
	{ 14, LT_CE_SETSUB, LT_CTK_MINUSEQ, 1 },
	{ 14, LT_CE_SETDIV, LT_CTK_SLASHEQ, 1 },
	{ 14, LT_CE_SETMUL, LT_CTK_ASTEREQ, 1 },
	{ 14, LT_CE_SETMOD, LT_CTK_PERCENTEQ, 1 },
	{ 14, LT_CE_SETOR, LT_CTK_PIPEEQ, 1 },
	{ 14, LT_CE_SETAND, LT_CTK_AMPEQ, 1 },
	{ 14, LT_CE_SETXOR, LT_CTK_CARETEQ, 1 },
	{ 14, LT_CE_SETSHL, LT_CTK_DLESSEREQ, 1 },
	{ 14, LT_CE_SETSHR, LT_CTK_DGREATEREQ, 1 },

	{ 6, LT_CE_LOGLESSER, LT_CTK_LESSER, 0 },
	{ 6, LT_CE_LOGGREATER, LT_CTK_GREATER, 0 },
	{ 6, LT_CE_LOGLEQ, LT_CTK_LESSEREQ, 0 },
	{ 6, LT_CE_LOGGEQ, LT_CTK_GREATEREQ, 0 },
	{ 7, LT_CE_LOGEQ, LT_CTK_DEQ, 0 },
	{ 7, LT_CE_LOGNEQ, LT_CTK_EXCEQ, 0 },
	{ 12, LT_CE_LOGOR, LT_CTK_DPIPE, 0 },
	{ 11, LT_CE_LOGAND, LT_CTK_DAMP, 0 },

	{ 13, LT_CE_TERNARY, LT_CTK_QUESTION, 0 },
	{ 15, LT_CE_COMMA, LT_CTK_COMMA, 0 },
};

static
operator_t* find_binary_op(lt_c_tk_type_t tk_type) {
	for (usz i = 0; i < sizeof(binary_op_tab) / sizeof(*binary_op_tab); ++i) {
		if (binary_op_tab[i].tk_type == tk_type)
			return &binary_op_tab[i];
	}
	return NULL;
}

lt_err_t parse_new_expr_binary(lt_c_parse_ctx_t* cx, b8 allow_comma, int max_precedence, lt_c_expr_t** out) {
	lt_err_t err;

	lt_c_expr_t* left;
	if ((err = parse_new_expr_unary(cx, &left)))
		return err;

	for (;;) {
		operator_t* op = find_binary_op(read_type(cx));
		if (!op || (!op->right_to_left && op->precedence >= max_precedence) || (op->right_to_left && op->precedence > max_precedence)) {
			*out = left;
			return LT_SUCCESS;
		}

		if (op->expr_type == LT_CE_COMMA && !allow_comma) {
			*out = left;
			return LT_SUCCESS;
		}

		consume(cx, NULL);

		lt_c_expr_t* new = lt_amalloc_lean(cx->alloc, sizeof(lt_c_expr_t));
		if (!new)
			return LT_ERR_OUT_OF_MEMORY;

		if (op->expr_type == LT_CE_TERNARY) {
			lt_c_expr_t* right;
			if ((err = parse_new_expr(cx, 1, &right)))
				return err;

			if ((err = consume_type(cx, LT_CTK_COLON, NULL)))
				return err;
			lt_c_expr_t* else_;
			if ((err = parse_new_expr_binary(cx, allow_comma, op->precedence, &else_)))
				return err;

			*new = (lt_c_expr_t) {
					.type = op->expr_type,
					.ternary.cond = left,
					.ternary.then = right,
					.ternary.else_ = else_ };
		}
		else {
			lt_c_expr_t* right;
			if ((err = parse_new_expr_binary(cx, allow_comma, op->precedence, &right)))
				return err;

			*new = (lt_c_expr_t) {
					.type = op->expr_type,
					.binary.left = left,
					.binary.right = right };
		}

		left = new;
	}

	return LT_SUCCESS;
}

static
lt_err_t parse_new_expr(lt_c_parse_ctx_t* cx, b8 allow_comma, lt_c_expr_t** out) {
	return parse_new_expr_binary(cx, allow_comma, 9999999, out);
}

lt_err_t lt_c_parse(lt_c_parse_ctx_t* cx, lt_c_tk_t* tokens, usz token_count, lt_arena_t* alloc) {
	lt_err_t err;

	cx->alloc = alloc;
	cx->it = tokens;
	cx->end = tokens + token_count;
	cx->root = NULL;
	cx->global_syms = (lt_c_symtab_t){ 0 };
	cx->syms = &cx->global_syms;

	lt_c_stmt_t** current = &cx->root;

	lt_c_tk_type_t tk_type = read_type(cx);
	while (tk_type != LT_CTK_EOF) {
		if ((err = parse_new_stmt(cx, current)))
			return err;

		current = &(*current)->next;

		tk_type = read_type(cx);
	}

	return LT_SUCCESS;
}

void lt_c_write_type(lt_c_type_t* type, lt_io_callback_t callb, void* usr, usz indent) {
	LT_ASSERT(type);

	switch ((lt_c_data_type_t)type->type) {
	case LT_CT_INVAL:
		LT_ASSERT_NOT_REACHED();
		break;

	case LT_CT_TYPEDEF:
		lt_c_write_type(type->typedef_.child, callb, usr, indent);
		break;

	case LT_CT_PTR:
		lt_c_write_type(type->array.elem_type, callb, usr, indent);
		lt_printf("^");
		break;

	case LT_CT_ARRAY:
		lt_c_write_type(type->array.elem_type, callb, usr, indent);
		lt_printf("[]");
		break;

	case LT_CT_FUNC:
		lt_c_write_type(type->func.return_type, callb, usr, indent);
		lt_printf("(");
		for (usz i = 0; i < lt_darr_count(type->func.param_types); ++i) {
			if (i)
				lt_printf(", ");
			lt_c_write_type(type->func.param_types[i], callb, usr, indent);
		}
		lt_printf(")");
		break;

	case LT_CT_ENUM: lt_printf("enum %S", type->enum_.tag.len ? type->enum_.tag : CLSTR("<anonymous>"));  break;
	case LT_CT_STRUCT: lt_printf("struct %S", type->struct_.tag.len ? type->struct_.tag : CLSTR("<anonymous>"));  break;
	case LT_CT_UNION: lt_printf("union %S", type->struct_.tag.len ? type->struct_.tag : CLSTR("<anonymous>")); break;

	case LT_CT_CHAR: lt_printf("char"); break;
	case LT_CT_UCHAR: lt_printf("unsigned char"); break;
	case LT_CT_SHORT: lt_printf("short"); break;
	case LT_CT_USHORT: lt_printf("unsigned short"); break;
	case LT_CT_INT: lt_printf("int"); break;
	case LT_CT_UINT: lt_printf("unsigned int"); break;
	case LT_CT_LONG: lt_printf("long"); break;
	case LT_CT_ULONG: lt_printf("unsigned long"); break;
	case LT_CT_FLOAT: lt_printf("float"); break;

	case LT_CT_DOUBLE: lt_printf("double"); break;
	case LT_CT_BOOL: lt_printf("bool"); break;
	case LT_CT_VOID: lt_printf("void"); break;
	}
}

void lt_c_write_expr(lt_c_expr_t* expr, lt_io_callback_t callb, void* usr, usz indent) {
	
}

void lt_c_write_stmt(lt_c_stmt_t* stmt, lt_io_callback_t callb, void* usr, usz indent) {
	
}
