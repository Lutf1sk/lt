#ifndef LT_C_H
#define LT_C_H 1

#include <lt/fwd.h>
#include <lt/err.h>

// lex.c

#define LT_CTK_EQOFFS 1
#define LT_CTK_DOFFS 2
#define LT_CTK_TOFFS 4

typedef
enum lt_c_tk_type {
	LT_CTK_INVAL = 0,
	LT_CTK_EOF = 1,
	LT_CTK_EQ,
	LT_CTK_DEQ = LT_CTK_EQ + LT_CTK_DOFFS,
	LT_CTK_PLUS,
	LT_CTK_PLUSEQ = LT_CTK_PLUS + LT_CTK_EQOFFS,
	LT_CTK_MINUS,
	LT_CTK_MINUSEQ = LT_CTK_MINUS + LT_CTK_EQOFFS,
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

	LT_CTK_MAX,
} lt_c_tk_type_t;

typedef
LT_PACKED_STRUCT(lt_c_tk) {
	u8 type;
	u32 len : 24;
	u32 str_offs;
} lt_c_tk_t;

#define LT_CTK_INIT(_type, _file, _str) { .type = (_type), .file = (_file), .str = (_str) }
#define LT_CTK(_type, _file, _str) ((lt_lpc_tk_t)LT_CTK_INIT(_type, _file, _str))

typedef
struct lt_c_lex_ctx {
	char* it, *end;
	lt_c_tk_t* tokens;
	usz token_count;

	u32 line;

	u32 err_line;
	lstr_t err_str;
} lt_c_lex_ctx_t;

#define LT_C_LEX_EMIT_COMMENTS 0x01
#define LT_C_LEX_EMIT_NEWLINES 0x02

extern lstr_t lt_c_tk_type_strtab[LT_CTK_MAX];

static LT_INLINE
lstr_t lt_c_tk_type_str(lt_c_tk_type_t type) {
	return lt_c_tk_type_strtab[type];
}

lt_err_t lt_c_lex(lt_c_lex_ctx_t* cx, void* data, usz len, u32 flags, lt_alloc_t* alloc);

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

	lt_c_preproc_template_t* template;
	lt_darr(lt_c_preproc_scope_t) scopes;
	lt_darr(lstr_t) disallowed;

	lstr_t file_path;

	usz line;

	u32 err_line;
	lstr_t err_str;
} lt_c_preproc_ctx_t;

lt_err_t lt_c_preproc_create(lt_c_preproc_ctx_t* cx, lstr_t* include_dirs, usz include_dir_count, lt_arena_t* arena);

lt_err_t lt_c_define(lt_c_preproc_ctx_t* cx, lstr_t name, lstr_t value, lt_darr(lstr_t) params, u8 flags);
lt_err_t lt_c_redefine(lt_c_preproc_ctx_t* cx, lstr_t name, lstr_t value, lt_darr(lstr_t) params, u8 flags);
lt_err_t lt_c_undefine(lt_c_preproc_ctx_t* cx, lstr_t name);

lt_c_define_t* lt_c_lookup_define(lt_c_preproc_ctx_t* cx, lstr_t name);

lt_err_t lt_c_preproc(lt_c_preproc_ctx_t* cx, void* data, usz size, lstr_t file_path, lstr_t* out);

#endif