#ifndef LT_C_H
#define LT_C_H 1

#include <lt/fwd.h>
#include <lt/err.h>

// lex.c

#define LT_CTK_EQOFFS 1
#define LT_CTK_DOFFS 2
#define LT_CTK_TOFFS 3

typedef
enum lt_c_tk_type {
	LT_CTK_INVAL = 0,
	LT_CTK_EOF = 1,
	LT_CTK_EQ,
	LT_CTK_DEQ = LT_CTK_EQ + LT_CTK_DOFFS,
	LT_CTK_PLUS,
	LT_CTK_PLUSEQ = LT_CTK_PLUS + LT_CTK_EQOFFS,
	LT_CTK_DPLUS = LT_CTK_PLUS + LT_CTK_DOFFS,
	LT_CTK_MINUS,
	LT_CTK_MINUSEQ = LT_CTK_MINUS + LT_CTK_EQOFFS,
	LT_CTK_DMINUS = LT_CTK_MINUS + LT_CTK_DOFFS,
	LT_CTK_ASTER,
	LT_CTK_ASTEREQ = LT_CTK_ASTER + LT_CTK_EQOFFS,
	LT_CTK_SLASH,
	LT_CTK_SLASHEQ = LT_CTK_SLASH + LT_CTK_EQOFFS,
	LT_CTK_PERCENT,
	LT_CTK_PERCENTEQ = LT_CTK_PERCENT + LT_CTK_EQOFFS,
	LT_CTK_PIPE,
	LT_CTK_PIPEEQ = LT_CTK_PIPE + LT_CTK_EQOFFS,
	LT_CTK_DPIPE = LT_CTK_PIPE + LT_CTK_DOFFS,
	LT_CTK_AMP,
	LT_CTK_AMPEQ = LT_CTK_AMP + LT_CTK_EQOFFS,
	LT_CTK_DAMP = LT_CTK_AMP + LT_CTK_DOFFS,
	LT_CTK_CARET,
	LT_CTK_CARETEQ = LT_CTK_CARET + LT_CTK_EQOFFS,
	LT_CTK_LESSER,
	LT_CTK_LESSEREQ = LT_CTK_LESSER + LT_CTK_EQOFFS,
	LT_CTK_DLESSER = LT_CTK_LESSER + LT_CTK_DOFFS,
	LT_CTK_DLESSEREQ = LT_CTK_LESSER + LT_CTK_DOFFS + LT_CTK_EQOFFS,
	LT_CTK_GREATER,
	LT_CTK_GREATEREQ = LT_CTK_GREATER + LT_CTK_EQOFFS,
	LT_CTK_DGREATER = LT_CTK_GREATER + LT_CTK_DOFFS,
	LT_CTK_DGREATEREQ = LT_CTK_GREATER + LT_CTK_DOFFS + LT_CTK_EQOFFS,
	LT_CTK_EXC,
	LT_CTK_EXCEQ = LT_CTK_EXC + LT_CTK_EQOFFS,
	LT_CTK_QUESTION,
	LT_CTK_TILDE,

	LT_CTK_IDENT,
	LT_CTK_NUM,
	LT_CTK_STR,
	LT_CTK_CHAR,

	LT_CTK_KWALIGNAS,
	LT_CTK_KWALIGNOF,
	LT_CTK_KWAUTO,
	LT_CTK_KWBREAK,
	LT_CTK_KWBOOL,
	LT_CTK_KWCASE,
	LT_CTK_KWCHAR,
	LT_CTK_KWCONST,
	LT_CTK_KWCONSTEXPR,
	LT_CTK_KWCONTINUE,
	LT_CTK_KWDEFAULT,
	LT_CTK_KWDO,
	LT_CTK_KWDOUBLE,
	LT_CTK_KWELSE,
	LT_CTK_KWENUM,
	LT_CTK_KWEXTERN,
	LT_CTK_KWFALSE,
	LT_CTK_KWFLOAT,
	LT_CTK_KWFOR,
	LT_CTK_KWGOTO,
	LT_CTK_KWIF,
	LT_CTK_KWINLINE,
	LT_CTK_KWINT,
	LT_CTK_KWLONG,
	LT_CTK_KWNULLPTR,
	LT_CTK_KWREGISTER,
	LT_CTK_KWRESTRICT,
	LT_CTK_KWRETURN,
	LT_CTK_KWSHORT,
	LT_CTK_KWSIGNED,
	LT_CTK_KWSIZEOF,
	LT_CTK_KWSTATIC,
	LT_CTK_KWSTATIC_ASSERT,
	LT_CTK_KWSTRUCT,
	LT_CTK_KWSWITCH,
	LT_CTK_KWTHREAD_LOCAL,
	LT_CTK_KWTRUE,
	LT_CTK_KWTYPEDEF,
	LT_CTK_KWTYPEOF,
	LT_CTK_KWTYPEOF_UNQUAL,
	LT_CTK_KWUNION,
	LT_CTK_KWUNSIGNED,
	LT_CTK_KWVOID,
	LT_CTK_KWVOLATILE,
	LT_CTK_KWWHILE,
	LT_CTK_KW_ALIGNAS,
	LT_CTK_KW_ALIGNOF,
	LT_CTK_KW_ATOMIC,
	LT_CTK_KW_BITINT,
	LT_CTK_KW_BOOL,
	LT_CTK_KW_COMPLEX,
	LT_CTK_KW_DECIMAL128,
	LT_CTK_KW_DECIMAL32,
	LT_CTK_KW_DECIMAL64,
	LT_CTK_KW_GENERIC,
	LT_CTK_KW_IMAGINARY,
	LT_CTK_KW_NORETURN,
	LT_CTK_KW_STATIC_ASSERT,
	LT_CTK_KW_THREAD_LOCAL,

	LT_CTK_DOT,
	LT_CTK_TDOT = LT_CTK_DOT + LT_CTK_TOFFS,
	LT_CTK_ARROW,
	LT_CTK_COLON,
	LT_CTK_COMMA,
	LT_CTK_SEMICOLON,
	LT_CTK_HASH,

	LT_CTK_LPAREN,
	LT_CTK_RPAREN,
	LT_CTK_LBRACE,
	LT_CTK_RBRACE,
	LT_CTK_LBRACKET,
	LT_CTK_RBRACKET,

	LT_CTK_COMMENT,
	LT_CTK_WHITESPACE,
	LT_CTK_NEWLINE,

	LT_CTK_MAX,
} lt_c_tk_type_t;

typedef
LT_PACKED_STRUCT(lt_c_tk) {
	u8 type;
	u32 origin : 24;
	u32 len;
	u32 str_offs;
} lt_c_tk_t;

typedef
struct lt_c_lex_ctx {
	lt_c_tk_t* tokens;
	usz token_count;

	usz* line_offsets;
	usz line_count;

	u32 err_line;
	u32 err_col;
	lstr_t err_str;
} lt_c_lex_ctx_t;

#define LT_C_LEX_EMIT_COMMENTS 0x01
#define LT_C_LEX_EMIT_NEWLINES 0x02
#define LT_C_LEX_EMIT_KEYWORDS 0x04

extern lstr_t lt_c_tk_type_strtab[LT_CTK_MAX];

static LT_INLINE
lstr_t lt_c_tk_type_str(lt_c_tk_type_t type) {
	return lt_c_tk_type_strtab[type];
}

lt_err_t lt_c_char_literal_value(lstr_t literal, i64* out_val, lstr_t* err_str, lt_alloc_t* alloc);

lt_c_tk_type_t lt_c_tk_keyword_type(char* start, char* end);

lstr_t lt_c_tk_str(char* src_start, lt_c_tk_t tk);
usz lt_c_offs_pos(usz* line_offsets, usz line_count, usz offs, usz* out_col);

lt_err_t lt_c_lex(lt_c_lex_ctx_t* cx, void* data, usz len, u32 origin, u32 flags, lt_alloc_t* alloc);

// preprocess.c

#define LT_CDEF_VARIADIC_SEPARATE	0x01
#define LT_CDEF_VARIADIC_LAST		0x02
#define LT_CDEF_VARIADIC			0x03

typedef
struct lt_c_define {
	u8 flags;
	lstr_t name;
	lstr_t value;
	lt_darr(lstr_t) params;
} lt_c_define_t;

typedef
struct lt_c_preproc_template {
	lstr_t in_file;
	u32 line;

	lstr_t from_ident;
	lt_darr(lstr_t) params;
	lt_darr(lstr_t) args;

	char* it;
	char* end;
	struct lt_c_preproc_template* prev;
} lt_c_preproc_template_t;

typedef
struct lt_c_preproc_scope {
	b8 condition;
	b8 dead;
} lt_c_preproc_scope_t;

typedef
struct lt_c_preproc_ctx {
	lt_arena_t* arena;

	lt_darr(lt_c_define_t) defines;
	lstr_t* include_dirs;
	usz include_dir_count;

	char* it;
	char* end;
	lt_c_preproc_template_t* template;

	lt_darr(lt_c_preproc_scope_t) scopes;
	lt_darr(lstr_t) disallowed;

	b8 is_condition;
	lt_strstream_t* ss;

	lstr_t file_path;
	u32 line;

	lstr_t err_file;
	u32 err_line;
	lstr_t err_str;
} lt_c_preproc_ctx_t;

lt_err_t lt_c_preproc_create(lt_c_preproc_ctx_t* cx, lstr_t* include_dirs, usz include_dir_count, lt_arena_t* arena);

lt_err_t lt_c_define(lt_c_preproc_ctx_t* cx, lstr_t name, lstr_t value, lt_darr(lstr_t) params, u8 flags);
lt_err_t lt_c_redefine(lt_c_preproc_ctx_t* cx, lstr_t name, lstr_t value, lt_darr(lstr_t) params, u8 flags);
lt_err_t lt_c_undefine(lt_c_preproc_ctx_t* cx, lstr_t name);

lt_c_define_t* lt_c_lookup_define(lt_c_preproc_ctx_t* cx, lstr_t name);

lt_err_t lt_c_preproc(lt_c_preproc_ctx_t* cx, void* data, usz size, lstr_t file_path, lstr_t* out);

// parse.c

#define LT_CT_SIGNED	0
#define LT_CT_UNSIGNED	1

typedef
enum lt_c_data_type {
	LT_CT_INVAL = 0,
	LT_CT_PTR,
	LT_CT_ARRAY,
	LT_CT_FUNC,
	LT_CT_STRUCT,
	LT_CT_ENUM,
	LT_CT_UNION,
	LT_CT_TYPEDEF,

	LT_CT_CHAR,
	LT_CT_UCHAR = LT_CT_CHAR + LT_CT_UNSIGNED,
	LT_CT_SHORT,
	LT_CT_USHORT = LT_CT_SHORT + LT_CT_UNSIGNED,
	LT_CT_INT,
	LT_CT_UINT = LT_CT_INT + LT_CT_UNSIGNED,
	LT_CT_LONG,
	LT_CT_ULONG = LT_CT_LONG + LT_CT_UNSIGNED,
	LT_CT_FLOAT,
	LT_CT_DOUBLE,
	LT_CT_BOOL,

	LT_CT_VOID,
} lt_c_data_type_t;

typedef
struct lt_c_type {
	u8 type;
	union {
		struct {
			lt_c_type_t* child;
		} ptr;

		struct {
			lt_c_type_t* elem_type;
			lt_c_expr_t* count;
		} array;

		struct {
			lt_c_type_t* child;
		} typedef_;

		struct {
			lt_darr(lt_c_type_t*) param_types;
			lt_darr(lstr_t) param_names;
			lt_c_type_t* return_type;
		} func;

		struct {
			lt_darr(lt_c_type_t*) member_types;
			lt_darr(lstr_t) member_names;
			lstr_t tag;
		} struct_;

		struct {
			lt_c_type_t* type;
			lt_darr(lstr_t) value_names;
			lt_darr(lt_c_expr_t*) value_exprs;
			lstr_t tag;
		} enum_;
	};
} lt_c_type_t;

typedef
enum lt_c_expr_type {
	LT_CE_INVAL = 0,
	LT_CE_ADD,
	LT_CE_SUB,
	LT_CE_MUL,
	LT_CE_DIV,
	LT_CE_MOD,
	LT_CE_NEG,
	LT_CE_UPLUS,

	LT_CE_OR,
	LT_CE_AND,
	LT_CE_XOR,
	LT_CE_SHL,
	LT_CE_SHR,
	LT_CE_INV,

	LT_CE_PFX_INC,
	LT_CE_SFX_INC,
	LT_CE_PFX_DEC,
	LT_CE_SFX_DEC,

	LT_CE_SET,
	LT_CE_SETADD,
	LT_CE_SETSUB,
	LT_CE_SETMUL,
	LT_CE_SETDIV,
	LT_CE_SETMOD,
	LT_CE_SETOR,
	LT_CE_SETAND,
	LT_CE_SETXOR,
	LT_CE_SETSHL,
	LT_CE_SETSHR,

	LT_CE_LOGEQ,
	LT_CE_LOGNEQ,
	LT_CE_LOGLEQ,
	LT_CE_LOGGEQ,
	LT_CE_LOGLESSER,
	LT_CE_LOGGREATER,
	LT_CE_LOGOR,
	LT_CE_LOGAND,
	LT_CE_LOGINV,

	LT_CE_IDENT,
	LT_CE_NUM,
	LT_CE_STR,
	LT_CE_PROC,

	LT_CE_TERNARY,
	LT_CE_COMMA,
	LT_CE_CAST,
	LT_CE_SIZEOF,
	LT_CE_ALIGNOF,

	LT_CE_COMPOUND,
	LT_CE_MEMBER,

	LT_CE_CALL,
	LT_CE_DEREF,
	LT_CE_REF,
	LT_CE_SUBSCRIPT,

	LT_CE_MAX,
} lt_c_expr_type_t;

typedef
struct lt_c_expr {
	u8 type;

	union {
		struct {
			lt_c_expr_t* child;
		} unary;

		struct {
			lt_c_expr_t* left;
			lt_c_expr_t* right;
		} binary;

		struct {
			lt_c_expr_t* func;
			lt_c_expr_t* args;
		} call;

		struct {
			lt_c_expr_t* array;
			lt_c_expr_t* index;
		} subscript;

		struct {
			lt_c_expr_t* cond;
			lt_c_expr_t* then;
			lt_c_expr_t* else_;
		} ternary;

		struct {
			lt_c_type_t* to;
			lt_c_expr_t* child;
		} cast;

		struct {
			lt_c_expr_t* parent;
			lstr_t name;
		} member;

		struct {
			lt_c_expr_t* initializers;
		} compound;

		u64 uint_val;
		i64 int_val;
		i32 char_val;
		f64 float_val;
		lstr_t str_val;
	};

	lt_c_expr_t* next;
} lt_c_expr_t;

#define LT_CDECL_DECL	0
#define LT_CDECL_PARAM	1
#define LT_CDECL_MEMBER	2
#define LT_CDECL_CAST	3

typedef
enum lt_c_stmt_type {
	LT_CS_INVAL = 0,
	LT_CS_DECL,
	LT_CS_IF,
	LT_CS_FOR,
	LT_CS_WHILE,
	LT_CS_DO_WHILE,
	LT_CS_RETURN,
	LT_CS_SWITCH,
	LT_CS_GOTO,
	LT_CS_CASE,
	LT_CS_BREAK,
	LT_CS_CONTINUE,
	LT_CS_EXPR,
	LT_CS_BLOCK,
	LT_CS_LABEL,
	LT_CS_EMPTY,

	LT_CS_MAX,
} lt_c_stmt_type_t;

#define LT_CS(type_, ...) (lt_c_stmt_t) { .type = type, __VA_ARGS__ }

typedef
struct lt_c_stmt {
	u8 type;
	union {
		struct {
			lt_darr(lt_c_type_t*) types;
			lt_darr(lstr_t) names;
		} decl;

		struct {
			lt_c_expr_t* cond;
			lt_c_stmt_t* then;
			lt_c_stmt_t* else_;
		} if_;

		struct {
			lt_c_stmt_t* begin;
			lt_c_expr_t* cond;
			lt_c_expr_t* iterate;

			lt_c_stmt_t* do_;
		} for_;

		struct {
			lt_c_expr_t* cond;
			lt_c_stmt_t* do_;
		} while_;

		struct {
			lt_c_expr_t* value;
		} return_;

		struct {
			lt_c_expr_t* value;
			lt_c_stmt_t* body;
		} switch_;

		struct {
			lstr_t label;
		} goto_;

		struct {
			lstr_t name;
		} label;

		struct {
			lt_c_expr_t* expr;
		} expr;

		struct {
			lt_c_stmt_t* children;
		} block;
	};

	lt_c_stmt_t* next;
} lt_c_stmt_t;

typedef
enum lt_c_sym_type {
	LT_CSYM_TYPEDEF,
	LT_CSYM_VALUE,
	LT_CSYM_FUNC,
	LT_CSYM_TAG,
} lt_c_sym_type_t;

typedef
struct lt_c_sym {
	u8 type;
	lstr_t name;
	union {
		struct {
			lt_c_expr_t* initializer;
			lt_c_type_t* type;
		} value;

		struct {
			lt_c_stmt_t* body;
			lt_c_type_t* type;
		} func;

		struct {
			lt_c_type_t* type;
		} typedef_;

		struct {
			lt_c_type_t* type;
		} tag;
	};
} lt_c_sym_t;

typedef
struct lt_c_symtab {
	lt_darr(lt_c_sym_t*) symbols;
	lt_darr(lt_c_sym_t*) structs;
	lt_darr(lt_c_sym_t*) unions;
	lt_darr(lt_c_sym_t*) enums;
	lt_c_symtab_t* parent;
} lt_c_symtab_t;

typedef
struct lt_c_parse_ctx {
	lt_c_tk_t* it, *end;
	char* str_base;
	lt_c_stmt_t* root;

	usz scope_depth;
	lt_c_symtab_t global_syms;
	lt_c_symtab_t* syms;

	u32 err_offs, err_len;
	lstr_t err_str;

	lt_arena_t* alloc;
} lt_c_parse_ctx_t;

extern lstr_t lt_c_expr_type_strtab[LT_CE_MAX];

static LT_INLINE
lstr_t lt_c_expr_type_str(lt_c_expr_type_t type) {
	return lt_c_expr_type_strtab[type];
}

extern lstr_t lt_c_stmt_type_strtab[LT_CS_MAX];

static LT_INLINE
lstr_t lt_c_stmt_type_str(lt_c_stmt_type_t type) {
	return lt_c_stmt_type_strtab[type];
}

void lt_c_free_expr(lt_c_expr_t* expr, lt_arena_t* alloc);
void lt_c_free_stmt(lt_c_stmt_t* stmt, lt_arena_t* alloc);

lt_c_sym_t** lt_c_lookup_sym(lt_c_sym_t** arr, usz count, lstr_t name);

lt_err_t lt_c_parse(lt_c_parse_ctx_t* cx, lt_c_tk_t* tokens, usz token_count, lt_arena_t* alloc);
lt_err_t lt_c_resolve_tree(lt_c_parse_ctx_t* cx);

void lt_c_write_type(lt_c_type_t* type, lt_io_callback_t callb, void* usr, usz indent);
void lt_c_write_expr(lt_c_expr_t* expr, lt_io_callback_t callb, void* usr, usz indent);
void lt_c_write_stmt(lt_c_stmt_t* stmt, lt_io_callback_t callb, void* usr, usz indent);

#endif