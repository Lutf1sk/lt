#ifndef LT_UTF8_H
#define LT_UTF8_H 1

#include <lt/lt.h>

#define LT_UTF16LE_BOM CLSTR("\xFF\xFE")
#define LT_UTF16BE_BOM CLSTR("\xFE\xFF")
#define LT_UTF8_BOM CLSTR("\xEF\xBB\xBF")

// utf8.c
usz lt_utf8_encode(u32 sym, char out[static 4]);

static inline
usz lt_utf8_encode_len(u32 v) {
	if (v < 0x80)
		return 1;
	else if (v < 0x800)
		return 2;
	else if (v < 0x10000)
		return 3;
	else
		return 4;
}

usz lt_utf8_decode(const char str[static 4], u32 out[static 1]);

static inline
usz lt_utf8_decode_len(char v) {
	if ((v & 0xF0) == 0xF0)
		return 4;
	else if ((v & 0xE0) == 0xE0)
		return 3;
	else if ((v & 0xC0) == 0xC0)
		return 2;
	else
		return 1;
}

usz lt_utf8_glyph_count(lstr_t str);

// wcwidth.c
isz lt_glyph_width(u32 c);
isz lt_utf8_str_width(lstr_t str);

#endif
