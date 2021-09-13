#ifndef LT_BASE64_H
#define LT_BASE64_H 1

#include <lt/lt.h>

usz lt_base64_encode(lstr_t str, char* out);
usz lt_base64_decode(lstr_t str, char* out);

#endif
