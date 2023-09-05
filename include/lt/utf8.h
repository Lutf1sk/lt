#ifndef LT_UTF8_H
#define LT_UTF8_H 1

#include <lt/lt.h>

// utf8.c
usz lt_utf8_encode(char* out, u32 sym);
usz lt_utf8_encode_len(u32 sym);

usz lt_utf8_decode(u32* out, char* str);
usz lt_utf8_decode_len(char sym);

usz lt_utf8_glyph_count(lstr_t str);

// wcwidth.c
isz lt_glyph_width(u32 c);
isz lt_utf8_str_width(lstr_t str);

#endif
