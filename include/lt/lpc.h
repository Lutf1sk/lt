#ifndef LT_LPC_H
#define LT_LPC_H 1

#include <lt/fwd.h>
#include <lt/err.h>

// lex.c

#define LT_LPCTK_EQOFFS 1
#define LT_LPCTK_DOFFS 2

typedef
enum lt_lpc_tk_type {
	LT_LPCTK_INVAL = 0,
	LT_LPCTK_EOF = 1,
	LT_LPCTK_EQ,
	LT_LPCTK_DEQ = LT_LPCTK_EQ + LT_LPCTK_DOFFS,
	LT_LPCTK_PLUS,
	LT_LPCTK_PLUSEQ = LT_LPCTK_PLUS + LT_LPCTK_EQOFFS,
	LT_LPCTK_MINUS,
	LT_LPCTK_MINUSEQ = LT_LPCTK_MINUS + LT_LPCTK_EQOFFS,
	LT_LPCTK_ASTER,
	LT_LPCTK_ASTEREQ = LT_LPCTK_ASTER + LT_LPCTK_EQOFFS,
	LT_LPCTK_SLASH,
	LT_LPCTK_SLASHEQ = LT_LPCTK_SLASH + LT_LPCTK_EQOFFS,
	LT_LPCTK_PERCENT,
	LT_LPCTK_PERCENTEQ = LT_LPCTK_PERCENT + LT_LPCTK_EQOFFS,
	LT_LPCTK_PIPE,
	LT_LPCTK_PIPEEQ = LT_LPCTK_PIPE + LT_LPCTK_EQOFFS,
	LT_LPCTK_DPIPE = LT_LPCTK_PIPE + LT_LPCTK_DOFFS,
	LT_LPCTK_AMP,
	LT_LPCTK_AMPEQ = LT_LPCTK_AMP + LT_LPCTK_EQOFFS,
	LT_LPCTK_DAMP = LT_LPCTK_AMP + LT_LPCTK_DOFFS,
	LT_LPCTK_CARET,
	LT_LPCTK_CARETEQ = LT_LPCTK_CARET + LT_LPCTK_EQOFFS,
	LT_LPCTK_LESSER,
	LT_LPCTK_LESSEREQ = LT_LPCTK_LESSER + LT_LPCTK_EQOFFS,
	LT_LPCTK_DLESSER = LT_LPCTK_LESSER + LT_LPCTK_DOFFS,
	LT_LPCTK_DLESSEREQ = LT_LPCTK_LESSER + LT_LPCTK_DOFFS + LT_LPCTK_EQOFFS,
	LT_LPCTK_GREATER,
	LT_LPCTK_GREATEREQ = LT_LPCTK_GREATER + LT_LPCTK_EQOFFS,
	LT_LPCTK_DGREATER = LT_LPCTK_GREATER + LT_LPCTK_DOFFS,
	LT_LPCTK_DGREATEREQ = LT_LPCTK_GREATER + LT_LPCTK_DOFFS + LT_LPCTK_EQOFFS,
	LT_LPCTK_EXC,
	LT_LPCTK_EXCEQ = LT_LPCTK_EXC + LT_LPCTK_EQOFFS,
	LT_LPCTK_TILDE,

	LT_LPCTK_IDENT,
	LT_LPCTK_NUM,
	LT_LPCTK_STR,
	LT_LPCTK_CHAR,

	LT_LPCTK_KWASSERT,
	LT_LPCTK_KWBEGIN,
	LT_LPCTK_KWBREAK,
	LT_LPCTK_KWCASE,
	LT_LPCTK_KWCONTINUE,
	LT_LPCTK_KWDEFER,
	LT_LPCTK_KWEND,
	LT_LPCTK_KWFOR,
	LT_LPCTK_KWPROC,
	LT_LPCTK_KWDO,
	LT_LPCTK_KWIF,
	LT_LPCTK_KWIMPORT,
	LT_LPCTK_KWELIF,
	LT_LPCTK_KWELSE,
	LT_LPCTK_KWRETURN,
	LT_LPCTK_KWTHEN,
	LT_LPCTK_KWSWITCH,

	LT_LPCTK_DOT,
	LT_LPCTK_DDOT = LT_LPCTK_DOT + LT_LPCTK_DOFFS,
	LT_LPCTK_COLON,
	LT_LPCTK_COMMA,
	LT_LPCTK_SEMICOLON,
	LT_LPCTK_HASH,

	LT_LPCTK_LPAREN,
	LT_LPCTK_RPAREN,
	LT_LPCTK_LBRACE,
	LT_LPCTK_RBRACE,
	LT_LPCTK_LBRACKET,
	LT_LPCTK_RBRACKET,

	LT_LPCTK_COMMENT,
	LT_LPCTK_WHITESPACE,

	LT_LPCTK_MAX,
} lt_lpc_tk_type_t;

typedef
LT_PACKED_STRUCT(lt_lpc_tk) {
	u8 type;
	u32 len : 24;
	u32 str_offs;
} lt_lpc_tk_t;

#define LT_LPCTK_INIT(_type, _file, _str) { .type = (_type), .file = (_file), .str = (_str) }
#define LT_LPCTK(_type, _file, _str) ((lt_lpc_tk_t)LT_LPCTK_INIT(_type, _file, _str))

typedef
struct lt_lpc_lex_ctx {
	char* it, *end;
	lstr_t err_str;
	lt_lpc_tk_t* tokens;
	usz token_count;
} lt_lpc_lex_ctx_t;

lt_err_t lt_lpc_unescape_str(lstr_t* out, lstr_t str);

extern lstr_t lt_lpc_tk_type_strtab[LT_LPCTK_MAX];

static LT_INLINE
lstr_t lt_lpc_tk_type_str(lt_lpc_tk_type_t type) {
	return lt_lpc_tk_type_strtab[type];
}

lt_err_t lt_lpc_lex(lt_lpc_lex_ctx_t* cx, void* data, usz len, lt_alloc_t* alloc);

// parse.c

typedef
enum lt_lpc_expr_type {
	LT_LPCE_INVAL = 0,
	LT_LPCE_ADD,
	LT_LPCE_SUB,
	LT_LPCE_MUL,
	LT_LPCE_DIV,
	LT_LPCE_MOD,
	LT_LPCE_NEG,

	LT_LPCE_OR,
	LT_LPCE_AND,
	LT_LPCE_XOR,
	LT_LPCE_SHL,
	LT_LPCE_SHR,
	LT_LPCE_INV,

	LT_LPCE_SET,
	LT_LPCE_SETADD,
	LT_LPCE_SETSUB,
	LT_LPCE_SETMUL,
	LT_LPCE_SETDIV,
	LT_LPCE_SETMOD,
	LT_LPCE_SETOR,
	LT_LPCE_SETAND,
	LT_LPCE_SETXOR,
	LT_LPCE_SETSHL,
	LT_LPCE_SETSHR,

	LT_LPCE_LOGEQ,
	LT_LPCE_LOGNEQ,
	LT_LPCE_LOGLEQ,
	LT_LPCE_LOGGEQ,
	LT_LPCE_LOGLESSER,
	LT_LPCE_LOGGREATER,
	LT_LPCE_LOGOR,
	LT_LPCE_LOGAND,
	LT_LPCE_LOGINV,

	LT_LPCE_RANGE,

	LT_LPCE_IDENT,
	LT_LPCE_NUM,
	LT_LPCE_STR,
	LT_LPCE_PROC,

	LT_LPCE_IF,

	LT_LPCE_IMPORT,

	LT_LPCE_CALL,
	LT_LPCE_DEREF,
	LT_LPCE_REF,
	LT_LPCE_SUBSCRIPT,

	LT_LPCE_PTR,
	LT_LPCE_ARRAY,

	LT_LPCE_MAX,
} lt_lpc_expr_type_t;

typedef
struct lt_lpc_expr {
	u8 type;
	union {
		struct {
			lt_lpc_expr_t* left;
			lt_lpc_expr_t* right;
		} binary;

		struct {
			lt_lpc_expr_t* child;
		} unary;

		struct {
			lt_lpc_expr_t* array;
			lt_lpc_expr_t* index;
		} subscript;

		struct {
			lt_lpc_expr_t* type;
			lt_lpc_expr_t* count;
		} array;

		struct {
			lt_lpc_expr_t* func;
			lt_lpc_expr_t* args;
		} call;

		struct {
			lt_lpc_expr_t* ret_type;
			lt_lpc_expr_t* arg_types;
			lt_lpc_stmt_t* body;
		} proc;

		struct {
			lt_lpc_expr_t* cond;
			lt_lpc_expr_t* then;
			lt_lpc_expr_t* else_;
		} if_;

		u64 uint_val;
		i64 int_val;
		lstr_t str_val;
		i32 char_val;
		b8 bool_val;
		f64 float_val;
	};

	lt_lpc_expr_t* next;
} lt_lpc_expr_t;

typedef
enum lt_lpc_stmt_type {
	LT_LPCS_INVAL = 0,
	LT_LPCS_ASSERT,
	LT_LPCS_IF,
	LT_LPCS_FOR,
	LT_LPCS_VAR,
	LT_LPCS_RETURN,
	LT_LPCS_IMPORT,
	LT_LPCS_SWITCH,
	LT_LPCS_CASE,
	LT_LPCS_BREAK,
	LT_LPCS_CONTINUE,
	LT_LPCS_DEFER,
	LT_LPCS_EXPR,

	LT_LPCS_MAX,
} lt_lpc_stmt_type_t;

#define LT_LPC_RESOLVE_STARTED	1
#define LT_LPC_RESOLVE_FINISHED	2
#define LT_LPC_GENERATED		3

typedef
struct lt_lpc_stmt {
	u8 type;
	u8 state;

	union {
		struct {
			lt_lpc_expr_t* cond;
		} assert;

		struct {
			lt_lpc_expr_t* cond;
			lt_lpc_stmt_t* then;
			lt_lpc_stmt_t* else_;
		} if_;

		struct {
			lt_lpc_expr_t* range;
			lt_lpc_stmt_t* do_;
			lstr_t name;
		} for_;

		struct {
			lt_lpc_expr_t* type;
			lt_lpc_expr_t* init;
			lstr_t name;
		} var;

		struct {
			lt_lpc_expr_t* value;
		} return_;

		struct {
			lt_lpc_expr_t* path;
		} import;

		struct {
			lt_lpc_expr_t* value;
			lt_lpc_stmt_t* cases;
			lt_lpc_stmt_t* else_;
		} switch_;

		struct {
			lt_lpc_expr_t* values;
			lt_lpc_stmt_t* then;
		} case_;

		struct {
			lt_lpc_expr_t* expr;
		} defer;

		struct {
			lt_lpc_expr_t* expr;
		} expr;
	};

	lt_lpc_stmt_t* next;
} lt_lpc_stmt_t;

typedef
struct lt_lpc_sym {
	lstr_t name;
	lt_lpc_stmt_t* definition;
} lt_lpc_sym_t;

typedef
struct lt_lpc_parse_ctx {
	lt_lpc_tk_t* it, *end;
	char* str_base;
	lt_lpc_stmt_t* root;

	usz scope_depth;
	lt_darr(lt_lpc_sym_t*) globals;
	lt_darr(lt_lpc_sym_t*) symbols;

	u32 err_offs, err_len;
	lstr_t err_str;

	lt_arena_t* alloc;
} lt_lpc_parse_ctx_t;

extern lstr_t lt_lpc_expr_type_strtab[LT_LPCE_MAX];

static LT_INLINE
lstr_t lt_lpc_expr_type_str(lt_lpc_expr_type_t type) {
	return lt_lpc_expr_type_strtab[type];
}

extern lstr_t lt_lpc_stmt_type_strtab[LT_LPCS_MAX];

static LT_INLINE
lstr_t lt_lpc_stmt_type_str(lt_lpc_stmt_type_t type) {
	return lt_lpc_stmt_type_strtab[type];
}

lt_lpc_sym_t** lt_lpc_lookup_sym(usz count, lt_lpc_sym_t* arr[static count], lstr_t name);

lt_err_t lt_lpc_parse(lt_lpc_parse_ctx_t* cx, lt_lpc_tk_t* tokens, usz token_count, lt_arena_t* alloc);
lt_err_t lt_lpc_resolve_tree(lt_lpc_parse_ctx_t* cx);

void lt_lpc_write_expr(lt_lpc_expr_t* expr, lt_write_fn_t callb, void* usr, usz indent);
void lt_lpc_write_stmt(lt_lpc_stmt_t* stmt, lt_write_fn_t callb, void* usr, usz indent);

#endif
