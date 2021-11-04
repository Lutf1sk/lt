#ifndef LT_UTF8_H
#define LT_UTF8_H 1

#include <lt/primitives.h>

usz lt_utf8_encode(char* out, u32 sym);
usz lt_utf8_encode_len(u32 sym);

usz lt_utf8_decode(u32* out, char* str);
usz lt_utf8_decode_len(char sym);

#endif
