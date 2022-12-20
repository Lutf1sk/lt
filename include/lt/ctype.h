#ifndef LT_CTYPE_H
#define LT_CTYPE_H 1

#include <lt/lt.h>

#define LT_NUMH (1 << 0)
#define LT_NUMB (1 << 1)
#define LT_NUMR (LT_NUMH | LT_NUMB)

#define LT_IDNH (1 << 2)
#define LT_IDNB (1 << 3)
#define LT_IDNT (LT_IDNH | LT_IDNB)

#define LT_IDNR (LT_IDNT | LT_NUMB)
#define LT_IBNR (LT_IDNB | LT_NUMR)

#define LT_WHSP (1 << 4)
#define LT_OPER (1 << 5)
#define LT_PARN (1 << 6)
#define LT_PUNC (1 << 7)

extern const u8 lt_ctype_table[256];
extern const u8 lt_upper_table[256];
extern const u8 lt_lower_table[256];

static LT_INLINE
u8 lt_is_ident_head(u8 c) {
	return lt_ctype_table[c] & LT_IDNH;
}

static LT_INLINE
u8 lt_is_ident_body(u8 c) {
	return lt_ctype_table[c] & LT_IDNB;
}

static LT_INLINE
u8 lt_is_numeric_head(u8 c) {
	return lt_ctype_table[c] & LT_NUMH;
}

static LT_INLINE
u8 lt_is_digit(u8 c) {
	return lt_ctype_table[c] & LT_NUMH;
}

static LT_INLINE
u8 lt_is_numeric_body(u8 c) {
	return lt_ctype_table[c] & LT_NUMB;
}

static LT_INLINE
u8 lt_is_space(u8 c) {
	return lt_ctype_table[c] & LT_WHSP;
}

static LT_INLINE
u8 lt_is_operator(u8 c) {
	return lt_ctype_table[c] & LT_OPER;
}

static LT_INLINE
u8 lt_is_paren(u8 c) {
	return lt_ctype_table[c] & LT_PARN;
}

static LT_INLINE
u8 lt_is_punc(u8 c) {
	return lt_ctype_table[c] & LT_PUNC;
}

static LT_INLINE
u8 lt_to_upper(u8 c) {
	return lt_upper_table[c];
}

static LT_INLINE
u8 lt_to_lower(u8 c) {
	return lt_lower_table[c];
}

static LT_INLINE
u8 lt_is_control_char(u8 c) {
	return c < 0x20 || c == 0x7F;
}

static LT_INLINE
b8 lt_is_unicode_control_char(u32 c) {
	return (c < 0x20) || (c >= 0x7F && c <= 0x9F);
}

#endif
