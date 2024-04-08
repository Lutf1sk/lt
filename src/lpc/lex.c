#include <lt/lpc.h>
#include <lt/darr.h>
#include <lt/ctype.h>
#include <lt/io.h>
#include <lt/debug.h>
#include <lt/mem.h>
#include <lt/math.h>

lt_err_t lt_lpc_unescape_str(lstr_t* out, lstr_t str) {
	return LT_ERR_NOT_IMPLEMENTED;
}

lstr_t lt_lpc_tk_type_strtab[LT_LPCTK_MAX] = {
	[LT_LPCTK_INVAL]		= CLSTRI("invalid"),
	[LT_LPCTK_EOF]			= CLSTRI("end-of-file"),
	[LT_LPCTK_EQ]			= CLSTRI("="),
	[LT_LPCTK_DEQ]			= CLSTRI("=="),
	[LT_LPCTK_PLUS]			= CLSTRI("+"),
	[LT_LPCTK_PLUSEQ]		= CLSTRI("+="),
	[LT_LPCTK_MINUS]		= CLSTRI("-"),
	[LT_LPCTK_MINUSEQ]		= CLSTRI("-="),
	[LT_LPCTK_ASTER]		= CLSTRI("*"),
	[LT_LPCTK_ASTEREQ]		= CLSTRI("*="),
	[LT_LPCTK_SLASH]		= CLSTRI("/"),
	[LT_LPCTK_SLASHEQ]		= CLSTRI("/="),
	[LT_LPCTK_PERCENT]		= CLSTRI("%"),
	[LT_LPCTK_PERCENTEQ]	= CLSTRI("%="),
	[LT_LPCTK_PIPE]			= CLSTRI("|"),
	[LT_LPCTK_PIPEEQ]		= CLSTRI("|="),
	[LT_LPCTK_DPIPE]		= CLSTRI("||"),
	[LT_LPCTK_AMP]			= CLSTRI("&"),
	[LT_LPCTK_AMPEQ]		= CLSTRI("&="),
	[LT_LPCTK_DAMP]			= CLSTRI("&&"),
	[LT_LPCTK_CARET]		= CLSTRI("^"),
	[LT_LPCTK_CARETEQ]		= CLSTRI("^="),
	[LT_LPCTK_LESSER]		= CLSTRI("<"),
	[LT_LPCTK_LESSEREQ]		= CLSTRI("<="),
	[LT_LPCTK_DLESSER]		= CLSTRI("<<"),
	[LT_LPCTK_DLESSEREQ]	= CLSTRI("<<="),
	[LT_LPCTK_GREATER]		= CLSTRI(">"),
	[LT_LPCTK_GREATEREQ]	= CLSTRI(">="),
	[LT_LPCTK_DGREATER]		= CLSTRI(">>"),
	[LT_LPCTK_DGREATEREQ]	= CLSTRI(">>="),
	[LT_LPCTK_EXC]			= CLSTRI("!"),
	[LT_LPCTK_EXCEQ]		= CLSTRI("!="),
	[LT_LPCTK_TILDE]		= CLSTRI("~"),

	[LT_LPCTK_IDENT]		= CLSTRI("identifier"),
	[LT_LPCTK_NUM]			= CLSTRI("number"),
	[LT_LPCTK_STR]			= CLSTRI("string"),
	[LT_LPCTK_CHAR]			= CLSTRI("character"),

	[LT_LPCTK_KWASSERT]		= CLSTRI("assert"),
	[LT_LPCTK_KWBEGIN]		= CLSTRI("begin"),
	[LT_LPCTK_KWBREAK]		= CLSTRI("break"),
	[LT_LPCTK_KWCASE]		= CLSTRI("case"),
	[LT_LPCTK_KWCONTINUE]	= CLSTRI("continue"),
	[LT_LPCTK_KWDEFER]		= CLSTRI("defer"),
	[LT_LPCTK_KWEND]		= CLSTRI("end"),
	[LT_LPCTK_KWFOR]		= CLSTRI("for"),
	[LT_LPCTK_KWPROC]		= CLSTRI("proc"),
	[LT_LPCTK_KWDO]			= CLSTRI("do"),
	[LT_LPCTK_KWIF]			= CLSTRI("if"),
	[LT_LPCTK_KWIMPORT]		= CLSTRI("import"),
	[LT_LPCTK_KWELIF]		= CLSTRI("elif"),
	[LT_LPCTK_KWELSE]		= CLSTRI("else"),
	[LT_LPCTK_KWRETURN]		= CLSTRI("return"),
	[LT_LPCTK_KWTHEN]		= CLSTRI("then"),
	[LT_LPCTK_KWSWITCH]		= CLSTRI("switch"),

	[LT_LPCTK_DOT]			= CLSTRI("."),
	[LT_LPCTK_DDOT]			= CLSTRI(".."),
	[LT_LPCTK_COLON]		= CLSTRI(":"),
	[LT_LPCTK_COMMA]		= CLSTRI(","),
	[LT_LPCTK_SEMICOLON]	= CLSTRI(";"),
	[LT_LPCTK_HASH]			= CLSTRI("#"),

	[LT_LPCTK_LPAREN]		= CLSTRI("("),
	[LT_LPCTK_RPAREN]		= CLSTRI(")"),
	[LT_LPCTK_LBRACE]		= CLSTRI("{"),
	[LT_LPCTK_RBRACE]		= CLSTRI("}"),
	[LT_LPCTK_LBRACKET]		= CLSTRI("["),
	[LT_LPCTK_RBRACKET]		= CLSTRI("]"),

	[LT_LPCTK_COMMENT]		= CLSTRI("comment"),
	[LT_LPCTK_WHITESPACE]	= CLSTRI("whitespace"),
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
#define DONE	10
#define JMP0(x) (x << 8)

static u16 tkttab1[256] = {
	['~'] = LT_LPCTK_TILDE		| JMP0(DONE),
	['.'] = LT_LPCTK_DOT		| JMP0(OPER4),
	[':'] = LT_LPCTK_COLON		| JMP0(DONE),
	[','] = LT_LPCTK_COMMA		| JMP0(DONE),
	[';'] = LT_LPCTK_SEMICOLON	| JMP0(DONE),
	['#'] = LT_LPCTK_HASH		| JMP0(DONE),
	['\\']	= 0		| JMP0(DONE),
	['?']	= 0		| JMP0(DONE),
	['$']	= 0		| JMP0(DONE),

	['('] = LT_LPCTK_LPAREN		| JMP0(DONE),
	[')'] = LT_LPCTK_RPAREN		| JMP0(DONE),
	['{'] = LT_LPCTK_LBRACE		| JMP0(DONE),
	['}'] = LT_LPCTK_RBRACE		| JMP0(DONE),
	['['] = LT_LPCTK_LBRACKET	| JMP0(DONE),
	[']'] = LT_LPCTK_RBRACKET	| JMP0(DONE),

	['+'] = LT_LPCTK_PLUS		| JMP0(OPER0),
	['-'] = LT_LPCTK_MINUS		| JMP0(OPER0),
	['*'] = LT_LPCTK_ASTER		| JMP0(OPER0),
	['/'] = LT_LPCTK_SLASH		| JMP0(OPER3),
	['%'] = LT_LPCTK_PERCENT	| JMP0(OPER0),
	['!'] = LT_LPCTK_EXC		| JMP0(OPER0),
	['^'] = LT_LPCTK_CARET		| JMP0(OPER0),
	['='] = LT_LPCTK_EQ			| JMP0(OPER1),
	['|'] = LT_LPCTK_PIPE		| JMP0(OPER1),
	['&'] = LT_LPCTK_AMP		| JMP0(OPER1),
	['<'] = LT_LPCTK_LESSER		| JMP0(OPER2),
	['>'] = LT_LPCTK_GREATER	| JMP0(OPER2),
};

static u8 intab1[256];
static b8 initialized = 0;

#include <immintrin.h>

static LT_INLINE
void initialize() {
	for (usz i = 'A'; i <= 'Z'; ++i) {
		tkttab1[i] = LT_LPCTK_IDENT | JMP0(IDENT);
		intab1[i] = 1;
	}
	for (usz i = 'a'; i <= 'z'; ++i) {
		tkttab1[i] = LT_LPCTK_IDENT | JMP0(IDENT);
		intab1[i] = 1;
	}
	intab1['_'] = 1;
	tkttab1['_'] = LT_LPCTK_IDENT | JMP0(IDENT);

	for (usz i = '0'; i <= '9'; ++i) {
		tkttab1[i] = LT_LPCTK_NUM | JMP0(NUM);
		intab1[i] = 1;
	}

	tkttab1['"'] = LT_LPCTK_STR | JMP0(STR);
	tkttab1['\''] = LT_LPCTK_CHAR | JMP0(CHAR);

	initialized = 1;
}

static LT_INLINE
lt_lpc_tk_type_t keyword(char* start, char* end) {
	usz len = end - start;
	if (len < 2)
		return LT_LPCTK_IDENT;

	switch (*start) {
	case 'a':
		if (len == 6 && memcmp(start + 1, "ssert", 5) == 0)
			return LT_LPCTK_KWASSERT;
		break;

	case 'b':
		if (len == 5 && memcmp(start + 1, "egin", 4) == 0)
			return LT_LPCTK_KWBEGIN;
		if (len == 5 && memcmp(start + 1, "reak", 4) == 0)
			return LT_LPCTK_KWBREAK;
		break;

	case 'c':
		if (len == 4 && memcmp(start + 1, "ase", 3) == 0)
			return LT_LPCTK_KWCASE;
		if (len == 8 && memcmp(start + 1, "ontinue", 7) == 0)
			return LT_LPCTK_KWCONTINUE;
		break;

	case 'd':
		if (len == 2 && start[1] == 'o')
			return LT_LPCTK_KWDO;
		if (len == 5 && memcmp(start + 1, "efer", 4) == 0)
			return LT_LPCTK_KWDEFER;
		break;

	case 'e':
		if (len == 3 && memcmp(start + 1, "nd", 2) == 0)
			return LT_LPCTK_KWEND;
		if (len != 4)
			break;
		if (memcmp(start + 1, "lif", 3) == 0)
			return LT_LPCTK_KWELIF;
		if (memcmp(start + 1, "lse", 3) == 0)
			return LT_LPCTK_KWELSE;
		break;

	case 'f':
		if (len == 3 && memcmp(start + 1, "or", 2) == 0)
			return LT_LPCTK_KWFOR;
		break;

	case 'i':
		if (len == 2 && start[1] == 'f')
			return LT_LPCTK_KWIF;
		if (len == 6 && memcmp(start + 1, "mport", 5) == 0)
			return LT_LPCTK_KWIMPORT;
		break;

	case 'p':
		if (len == 4 && memcmp(start + 1, "roc", 3) == 0)
			return LT_LPCTK_KWPROC;
		break;

	case 'r':
		if (len == 6 && memcmp(start + 1, "eturn", 5) == 0)
			return LT_LPCTK_KWRETURN;
		break;

	case 't':
		if (len == 4 && memcmp(start + 1, "hen", 3) == 0)
			return LT_LPCTK_KWTHEN;
		break;

	case 's':
		if (len == 6 && memcmp(start + 1, "witch", 5) == 0)
			return LT_LPCTK_KWSWITCH;
		break;
	}
	return LT_LPCTK_IDENT;
}

lt_err_t lt_lpc_lex(lt_lpc_lex_ctx_t* cx, void* data, usz len, lt_alloc_t* alloc) {
	if (!initialized)
		initialize();

	char* it = data;
	char* end = it + len;
	cx->err_str = NLSTR();
	cx->tokens = lt_malloc(alloc, len * sizeof(lt_lpc_tk_t));
	if (!cx->tokens)
		return LT_ERR_OUT_OF_MEMORY;

	__m256i ymm_newline = _mm256_set1_epi8('\n');
	__m256i ymm_comment_end = _mm256_set1_epi16((u16)'*' | ((u16)'/' << 8));

	lt_lpc_tk_t* out = cx->tokens;

	for (;;) {
		char c;
		while ((u8)(c = *it) <= 32)
			++it;

		if (it >= end) {
			cx->token_count = out - cx->tokens;
			cx->tokens = lt_mrealloc(alloc, cx->tokens, cx->token_count * sizeof(lt_lpc_tk_t));
			LT_ASSERT(cx->tokens);
			return LT_SUCCESS;
		}

		u16 tktabent = tkttab1[(u8)c];
		lt_lpc_tk_type_t type = tktabent & 0xFF;

		char* start = it++;
		static void* jmptab[] = { &&jinval, &&jident, &&jnum, &&jstr, &&jchar, &&joper0, &&joper1, &&joper2, &&joper3, &&joper4, &&jend };
		goto *jmptab[tktabent >> 8];

	jinval:
		--it;
		lt_aprintf(&cx->err_str, alloc, "unknown symbol 0x%hb '%c'", c, c);
		return LT_ERR_INVALID_SYNTAX;

	jident:
		while (lt_is_ident_body(*it))
			++it;
		type = keyword(start, it);
		goto jend;

	jnum:
		while (lt_is_numeric_body(*it))
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
			type += LT_LPCTK_EQOFFS;
			goto jend;
		}
		goto jend;

	joper1:
		if (*it == c) {
			++it;
			type += LT_LPCTK_DOFFS;
			goto jend;
		}
		if (*it == '=') {
			++it;
			type += LT_LPCTK_EQOFFS;
			goto jend;
		}
		goto jend;

	joper2:
		if (*it != c) {
			if (*it == '=') {
				++it;
				type += LT_LPCTK_EQOFFS;
			}
			goto jend;
		}

		if (it[1] == '=') {
			it += 2;
			type += LT_LPCTK_DOFFS + LT_LPCTK_EQOFFS;
			goto jend;
		}

		++it;
		type += LT_LPCTK_DOFFS;
		goto jend;

	joper3:
		if (*it == '*') {
			type = LT_LPCTK_COMMENT;
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
			usz count = lt_min(count0, count1 - 1) - 1;

			it += count;
			if (it >= end) {
				lt_aprintf(&cx->err_str, alloc, "unterminated multiline comment");
				return LT_ERR_INVALID_SYNTAX;
			}
			if (count == 30)
				goto mlcomment;

			it += 2;
			goto jend;
		}
		else if (*it == '/') {
			type = LT_LPCTK_COMMENT;
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
			goto jend;
		}
		if (*it == '=') {
			it += 1;
			type += LT_LPCTK_EQOFFS;
			goto jend;
		}
		goto jend;

	joper4:
		if (*it == c) {
			++it;
			type += LT_LPCTK_DOFFS;
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
