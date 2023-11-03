#include <lt/c.h>
#include <lt/darr.h>
#include <lt/ctype.h>
#include <lt/io.h>
#include <lt/debug.h>
#include <lt/mem.h>
#include <lt/math.h>

lt_err_t lt_c_unescape_str(lstr_t* out, lstr_t str) {
	return LT_ERR_NOT_IMPLEMENTED;
}

lstr_t lt_c_tk_type_strtab[LT_CTK_MAX] = {
	[LT_CTK_INVAL]		= CLSTRI("invalid"),
	[LT_CTK_EOF]		= CLSTRI("end-of-file"),
	[LT_CTK_EQ]			= CLSTRI("="),
	[LT_CTK_DEQ]		= CLSTRI("=="),
	[LT_CTK_PLUS]		= CLSTRI("+"),
	[LT_CTK_DPLUS]		= CLSTRI("++"),
	[LT_CTK_PLUSEQ]		= CLSTRI("+="),
	[LT_CTK_MINUS]		= CLSTRI("-"),
	[LT_CTK_DMINUS]		= CLSTRI("--"),
	[LT_CTK_MINUSEQ]	= CLSTRI("-="),
	[LT_CTK_ASTER]		= CLSTRI("*"),
	[LT_CTK_ASTEREQ]	= CLSTRI("*="),
	[LT_CTK_SLASH]		= CLSTRI("/"),
	[LT_CTK_SLASHEQ]	= CLSTRI("/="),
	[LT_CTK_PERCENT]	= CLSTRI("%"),
	[LT_CTK_PERCENTEQ]	= CLSTRI("%="),
	[LT_CTK_PIPE]		= CLSTRI("|"),
	[LT_CTK_PIPEEQ]		= CLSTRI("|="),
	[LT_CTK_DPIPE]		= CLSTRI("||"),
	[LT_CTK_AMP]		= CLSTRI("&"),
	[LT_CTK_AMPEQ]		= CLSTRI("&="),
	[LT_CTK_DAMP]		= CLSTRI("&&"),
	[LT_CTK_CARET]		= CLSTRI("^"),
	[LT_CTK_CARETEQ]	= CLSTRI("^="),
	[LT_CTK_LESSER]		= CLSTRI("<"),
	[LT_CTK_LESSEREQ]	= CLSTRI("<="),
	[LT_CTK_DLESSER]	= CLSTRI("<<"),
	[LT_CTK_DLESSEREQ]	= CLSTRI("<<="),
	[LT_CTK_GREATER]	= CLSTRI(">"),
	[LT_CTK_GREATEREQ]	= CLSTRI(">="),
	[LT_CTK_DGREATER]	= CLSTRI(">>"),
	[LT_CTK_DGREATEREQ]	= CLSTRI(">>="),
	[LT_CTK_EXC]		= CLSTRI("!"),
	[LT_CTK_EXCEQ]		= CLSTRI("!="),
	[LT_CTK_QUESTION]	= CLSTRI("?"),
	[LT_CTK_TILDE]		= CLSTRI("~"),

	[LT_CTK_IDENT]		= CLSTRI("identifier"),
	[LT_CTK_NUM]		= CLSTRI("number"),
	[LT_CTK_STR]		= CLSTRI("string"),
	[LT_CTK_CHAR]		= CLSTRI("character"),

	[LT_CTK_KWALIGNAS]			= CLSTRI("alignas"),
	[LT_CTK_KWALIGNOF]			= CLSTRI("alignof"),
	[LT_CTK_KWAUTO]				= CLSTRI("auto"),
	[LT_CTK_KWBREAK]			= CLSTRI("break"),
	[LT_CTK_KWBOOL]				= CLSTRI("bool"),
	[LT_CTK_KWCASE]				= CLSTRI("case"),
	[LT_CTK_KWCHAR]				= CLSTRI("char"),
	[LT_CTK_KWCONST]			= CLSTRI("const"),
	[LT_CTK_KWCONSTEXPR]		= CLSTRI("constexpr"),
	[LT_CTK_KWCONTINUE]			= CLSTRI("continue"),
	[LT_CTK_KWDEFAULT]			= CLSTRI("default"),
	[LT_CTK_KWDO]				= CLSTRI("do"),
	[LT_CTK_KWDOUBLE]			= CLSTRI("double"),
	[LT_CTK_KWELSE]				= CLSTRI("else"),
	[LT_CTK_KWENUM]				= CLSTRI("enum"),
	[LT_CTK_KWEXTERN]			= CLSTRI("extern"),
	[LT_CTK_KWFALSE]			= CLSTRI("false"),
	[LT_CTK_KWFLOAT]			= CLSTRI("float"),
	[LT_CTK_KWFOR]				= CLSTRI("for"),
	[LT_CTK_KWGOTO]				= CLSTRI("goto"),
	[LT_CTK_KWIF]				= CLSTRI("if"),
	[LT_CTK_KWINLINE]			= CLSTRI("inline"),
	[LT_CTK_KWINT]				= CLSTRI("int"),
	[LT_CTK_KWLONG]				= CLSTRI("long"),
	[LT_CTK_KWNULLPTR]			= CLSTRI("nullptr"),
	[LT_CTK_KWREGISTER]			= CLSTRI("register"),
	[LT_CTK_KWRESTRICT]			= CLSTRI("restrict"),
	[LT_CTK_KWRETURN]			= CLSTRI("return"),
	[LT_CTK_KWSHORT]			= CLSTRI("short"),
	[LT_CTK_KWSIGNED]			= CLSTRI("signed"),
	[LT_CTK_KWSIZEOF]			= CLSTRI("sizeof"),
	[LT_CTK_KWSTATIC]			= CLSTRI("static"),
	[LT_CTK_KWSTATIC_ASSERT]	= CLSTRI("static_assert"),
	[LT_CTK_KWSTRUCT]			= CLSTRI("struct"),
	[LT_CTK_KWSWITCH]			= CLSTRI("switch"),
	[LT_CTK_KWTHREAD_LOCAL]		= CLSTRI("thread_local"),
	[LT_CTK_KWTRUE]				= CLSTRI("true"),
	[LT_CTK_KWTYPEDEF]			= CLSTRI("typedef"),
	[LT_CTK_KWTYPEOF]			= CLSTRI("typeof"),
	[LT_CTK_KWTYPEOF_UNQUAL]	= CLSTRI("typeof_unqual"),
	[LT_CTK_KWUNION]			= CLSTRI("union"),
	[LT_CTK_KWUNSIGNED]			= CLSTRI("unsigned"),
	[LT_CTK_KWVOID]				= CLSTRI("void"),
	[LT_CTK_KWVOLATILE]			= CLSTRI("volatile"),
	[LT_CTK_KWWHILE]			= CLSTRI("while"),
	[LT_CTK_KW_ALIGNAS]			= CLSTRI("_Alignas"),
	[LT_CTK_KW_ALIGNOF]			= CLSTRI("_Alignof"),
	[LT_CTK_KW_ATOMIC]			= CLSTRI("_Atomic"),
	[LT_CTK_KW_BITINT]			= CLSTRI("_BitInt"),
	[LT_CTK_KW_BOOL]			= CLSTRI("_Bool"),
	[LT_CTK_KW_COMPLEX]			= CLSTRI("_Complex"),
	[LT_CTK_KW_DECIMAL128]		= CLSTRI("_Decimal128"),
	[LT_CTK_KW_DECIMAL32]		= CLSTRI("_Decimal32"),
	[LT_CTK_KW_DECIMAL64]		= CLSTRI("_Decimal64"),
	[LT_CTK_KW_GENERIC]			= CLSTRI("_Generic"),
	[LT_CTK_KW_IMAGINARY]		= CLSTRI("_Imaginary"),
	[LT_CTK_KW_NORETURN]		= CLSTRI("_Noreturn"),
	[LT_CTK_KW_STATIC_ASSERT]	= CLSTRI("_Static_assert"),
	[LT_CTK_KW_THREAD_LOCAL]	= CLSTRI("_Thread_local"),

	[LT_CTK_DOT]		= CLSTRI("."),
	[LT_CTK_TDOT]		= CLSTRI("..."),
	[LT_CTK_ARROW]		= CLSTRI("->"),
	[LT_CTK_COLON]		= CLSTRI(":"),
	[LT_CTK_COMMA]		= CLSTRI(","),
	[LT_CTK_SEMICOLON]	= CLSTRI(";"),
	[LT_CTK_HASH]		= CLSTRI("#"),

	[LT_CTK_LPAREN]		= CLSTRI("("),
	[LT_CTK_RPAREN]		= CLSTRI(")"),
	[LT_CTK_LBRACE]		= CLSTRI("{"),
	[LT_CTK_RBRACE]		= CLSTRI("}"),
	[LT_CTK_LBRACKET]	= CLSTRI("["),
	[LT_CTK_RBRACKET]	= CLSTRI("]"),

	[LT_CTK_COMMENT]	= CLSTRI("comment"),
	[LT_CTK_WHITESPACE]	= CLSTRI("whitespace"),
};

#define INVAL	0
#define IDENT	1
#define NUM		2
#define STR		3
#define CHAR	4
#define OPER0	5
#define OPER1	6
#define OPER2	7
#define OPER3	8
#define OPER4	9
#define OPER5	10
#define DONE	11
#define JMP0(x) (x << 8)

static u16 tkttab1[256] = {
	['~'] = LT_CTK_TILDE	| JMP0(DONE),
	['.'] = LT_CTK_DOT		| JMP0(OPER4),
	[':'] = LT_CTK_COLON	| JMP0(DONE),
	[','] = LT_CTK_COMMA	| JMP0(DONE),
	[';'] = LT_CTK_SEMICOLON| JMP0(DONE),
	['#'] = LT_CTK_HASH		| JMP0(DONE),
	['\\']	= 0				| JMP0(DONE),
	['?']	= LT_CTK_COLON	| JMP0(DONE),

	['('] = LT_CTK_LPAREN	| JMP0(DONE),
	[')'] = LT_CTK_RPAREN	| JMP0(DONE),
	['{'] = LT_CTK_LBRACE	| JMP0(DONE),
	['}'] = LT_CTK_RBRACE	| JMP0(DONE),
	['['] = LT_CTK_LBRACKET	| JMP0(DONE),
	[']'] = LT_CTK_RBRACKET	| JMP0(DONE),

	['+'] = LT_CTK_PLUS		| JMP0(OPER1),
	['-'] = LT_CTK_MINUS	| JMP0(OPER5),
	['*'] = LT_CTK_ASTER	| JMP0(OPER0),
	['/'] = LT_CTK_SLASH	| JMP0(OPER3),
	['%'] = LT_CTK_PERCENT	| JMP0(OPER0),
	['!'] = LT_CTK_EXC		| JMP0(OPER0),
	['^'] = LT_CTK_CARET	| JMP0(OPER0),
	['='] = LT_CTK_EQ		| JMP0(OPER1),
	['|'] = LT_CTK_PIPE		| JMP0(OPER1),
	['&'] = LT_CTK_AMP		| JMP0(OPER1),
	['<'] = LT_CTK_LESSER	| JMP0(OPER2),
	['>'] = LT_CTK_GREATER	| JMP0(OPER2),
};

static u8 intab1[256];
static b8 initialized = 0;

#include <immintrin.h>

static LT_INLINE
void initialize() {
	for (usz i = 'A'; i <= 'Z'; ++i) {
		tkttab1[i] = LT_CTK_IDENT | JMP0(IDENT);
		intab1[i] = 1;
	}
	for (usz i = 'a'; i <= 'z'; ++i) {
		tkttab1[i] = LT_CTK_IDENT | JMP0(IDENT);
		intab1[i] = 1;
	}
	intab1['_'] = 1;
	tkttab1['_'] = LT_CTK_IDENT | JMP0(IDENT);
	intab1['$'] = 1;
	tkttab1['$'] = LT_CTK_IDENT | JMP0(IDENT);

	for (usz i = '0'; i <= '9'; ++i) {
		tkttab1[i] = LT_CTK_NUM | JMP0(NUM);
		intab1[i] = 1;
	}

	tkttab1['"'] = LT_CTK_STR | JMP0(STR);
	tkttab1['\''] = LT_CTK_CHAR | JMP0(CHAR);

	initialized = 1;
}

static LT_INLINE
lt_c_tk_type_t keyword(char* start, char* end) {
	usz len = end - start;
	if (len < 2)
		return LT_CTK_IDENT;

#define CHECK(str, tk) if (len == (sizeof(str) - 1) && memcmp(start, str, (sizeof(str) - 1)) == 0) return LT_CTK_KW##tk

	switch (*start) {
	case 'a':
		CHECK("alignas", ALIGNAS);
		CHECK("alignof", ALIGNOF);
		CHECK("auto", AUTO);
		break;

	case 'b':
		CHECK("break", BREAK);
		CHECK("bool", BOOL);
		break;

	case 'c':
		CHECK("case", CASE);
		CHECK("char", CHAR);
		CHECK("const", CONST);
		CHECK("constexpr", CONSTEXPR);
		CHECK("continue", CONTINUE);
		break;

	case 'd':
		CHECK("default", DEFAULT);
		CHECK("do", DO);
		CHECK("double", DOUBLE);
		break;

	case 'e':
		CHECK("else", ELSE);
		CHECK("enum", ENUM);
		CHECK("extern", EXTERN);
		break;

	case 'f':
		CHECK("false", FALSE);
		CHECK("float", FLOAT);
		CHECK("for", FOR);
		break;

	case 'g':
		CHECK("goto", GOTO);
		break;

	case 'i':
		CHECK("if", IF);
		CHECK("inline", INLINE);
		CHECK("int", INT);
		break;

	case 'l':
		CHECK("long", LONG);
		break;

	case 'n':
		CHECK("nullptr", NULLPTR);
		break;

	case 'r':
		CHECK("register", REGISTER);
		CHECK("restrict", RESTRICT);
		CHECK("return", RETURN);
		break;

	case 's':
		CHECK("short", SHORT);
		CHECK("signed", SIGNED);
		CHECK("sizeof", SIZEOF);
		CHECK("static", STATIC);
		CHECK("static_assert", STATIC_ASSERT);
		CHECK("struct", STRUCT);
		CHECK("switch", SWITCH);
		break;

	case 't':
		CHECK("thread_local", THREAD_LOCAL);
		CHECK("true", TRUE);
		CHECK("typedef", TYPEDEF);
		CHECK("typeof", TYPEOF);
		CHECK("typeof_unqual", TYPEOF_UNQUAL);
		break;

	case 'u':
		CHECK("union", UNION);
		CHECK("unsigned", UNSIGNED);
		break;

	case 'v':
		CHECK("void", VOID);
		CHECK("volatile", VOLATILE);
		break;

	case 'w':
		CHECK("while", WHILE);
		break;

	case '_':
		CHECK("_Alignas", _ALIGNAS);
		CHECK("_Alignof", _ALIGNOF);
		CHECK("_Atomic", _ATOMIC);
		CHECK("_BitInt", _BITINT);
		CHECK("_Bool", _BOOL);
		CHECK("_Complex", _COMPLEX);
		CHECK("_Decimal128", _DECIMAL128);
		CHECK("_Decimal32", _DECIMAL32);
		CHECK("_Decimal64", _DECIMAL64);
		CHECK("_Generic", _GENERIC);
		CHECK("_Imaginary", _IMAGINARY);
		CHECK("_Noreturn", _NORETURN);
		CHECK("_Static_assert", _STATIC_ASSERT);
		CHECK("_Thread_local", _THREAD_LOCAL);
		break;
	}
#undef CHECK

	return LT_CTK_IDENT;
}

lt_err_t lt_c_lex(lt_c_lex_ctx_t* cx, void* data, usz len, u32 flags, lt_alloc_t* alloc) {
	b8 filter_comment = 1;
	if (flags & LT_C_LEX_EMIT_COMMENTS)
		filter_comment = 0;

	if (!initialized)
		initialize();

	char* it = data;
	char* end = it + len;
	cx->err_str = NLSTR();
	cx->tokens = lt_malloc(alloc, len * sizeof(lt_c_tk_t));
	if (!cx->tokens)
		return LT_ERR_OUT_OF_MEMORY;

	__m256i ymm_newline = _mm256_set1_epi8('\n');
	__m256i ymm_comment_end = _mm256_set1_epi16((u16)'*' | ((u16)'/' << 8));

	lt_c_tk_t* out = cx->tokens;

	for (;;) {
		char c;
		while ((u8)(c = *it) <= 32)
			++it;

		if (it >= end) {
			cx->token_count = out - cx->tokens;
			cx->tokens = lt_mrealloc(alloc, cx->tokens, cx->token_count * sizeof(lt_c_tk_t));
			return LT_SUCCESS;
		}

		u16 tktabent = tkttab1[(u8)c];
		lt_c_tk_type_t type = tktabent & 0xFF;

		char* start = it++;
		static void* jmptab[] = { &&jinval, &&jident, &&jnum, &&jstr, &&jchar, &&joper0, &&joper1, &&joper2, &&joper3, &&joper4, &&joper5, &&jend };
		goto *jmptab[tktabent >> 8];

	jinval:
		--it;
		lt_aprintf(&cx->err_str, alloc, "unknown symbol 0x%hb '%c'", c, c);
		return LT_ERR_INVALID_SYNTAX;

	jident:
		while (intab1[(u8)*it])
			++it;
		type = keyword(start, it);
		goto jend;

	jnum:
		while (intab1[(u8)*it])
			++it;
		goto jend;

	jstr:
		while ((c = *it++) != '"') {
			if (it >= end) {
				lt_aprintf(&cx->err_str, alloc, "unterminated string literal");
				return LT_ERR_INVALID_SYNTAX;
			}
			if (c == '\\')
				++it;
		}
		goto jend;

	jchar:
		while ((c = *it++) != '\'') {
			if (it >= end) {
				lt_aprintf(&cx->err_str, alloc, "unterminated character literal");
				return LT_ERR_INVALID_SYNTAX;
			}
			if (c == '\\')
				++it;
		}
		goto jend;

	joper0:
		if (*it == '=') {
			++it;
			type += LT_CTK_EQOFFS;
			goto jend;
		}
		goto jend;

	joper1:
		if (*it == c) {
			++it;
			type += LT_CTK_DOFFS;
			goto jend;
		}
		if (*it == '=') {
			++it;
			type += LT_CTK_EQOFFS;
			goto jend;
		}
		goto jend;

	joper2:
		if (*it != c) {
			if (*it == '=') {
				++it;
				type += LT_CTK_EQOFFS;
			}
			goto jend;
		}

		if (it[1] == '=') {
			it += 2;
			type += LT_CTK_DOFFS + LT_CTK_EQOFFS;
			goto jend;
		}

		++it;
		type += LT_CTK_DOFFS;
		goto jend;

	joper3:
		if (*it == '*') {
			type = LT_CTK_COMMENT;
			++it;
		mlcomment:;
			__m256i str0 = _mm256_loadu_si256((__m256i*)it);
			__m256i str1 = _mm256_loadu_si256((__m256i*)(it - 1));

			__m256i mask0 = _mm256_cmpeq_epi16(str0, ymm_comment_end);
			__m256i mask1 = _mm256_cmpeq_epi16(str1, ymm_comment_end);

			u32 mask0b = _mm256_movemask_epi8(mask0);
			u32 mask1b = _mm256_movemask_epi8(mask1);

			usz count0 = mask0b ? __builtin_ffs(mask0b) : 32;
			usz count1 = mask1b ? __builtin_ffs(mask1b) : 32;
			usz count = lt_min_usz(count0, count1 - 1) - 1;

			it += count;
			if (it >= end) {
				lt_aprintf(&cx->err_str, alloc, "unterminated multiline comment");
				return LT_ERR_INVALID_SYNTAX;
			}
			if (count == 30)
				goto mlcomment;

			it += 2;
			if (filter_comment)
				continue;
			goto jend;
		}
		else if (*it == '/') {
			type = LT_CTK_COMMENT;
			++it;
		slcomment:;
			__m256i str0 = _mm256_loadu_si256((__m256i*)it);
			__m256i mask0 = _mm256_cmpeq_epi8(str0, ymm_newline);

			u32 maskb = _mm256_movemask_epi8(mask0);
			if (!maskb) {
				it += 32;
				if (it >= end) {
					it = end;
					goto jend;
				}
				goto slcomment;
			}

			it += __builtin_ctz(maskb);
			if (filter_comment)
				continue;
			goto jend;
		}
		if (*it == '=') {
			it += 1;
			type += LT_CTK_EQOFFS;
			goto jend;
		}
		goto jend;

	joper4:
		if (*it == c && it[1] == c) {
			it += 2;
			type += LT_CTK_TOFFS;
			goto jend;
		}
		goto jend;

	joper5:
		if (*it == c) {
			++it;
			type += LT_CTK_DOFFS;
			goto jend;
		}
		if (*it == '=') {
			++it;
			type += LT_CTK_EQOFFS;
			goto jend;
		}
		if (*it == '>') {
			++it;
			type = LT_CTK_ARROW;
			goto jend;
		}
		goto jend;

	jend:
		out->type = type;
		out->len = it - start;
		out->str_offs = start - (char*)data;
		++out;
	}
}
