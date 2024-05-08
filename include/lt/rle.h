#ifndef LT_RLE_H
#define LT_RLE_H 1

#include <lt/lt.h>

#define LT_RLE_EXT 0x80

#define LT_RLE_SIZE_MASK 0x7F

// size of 'out' should be len + 1
usz lt_rlencode(void* buf, usz size, void* out);

usz lt_rldecode(void* buf, usz size, void* out);

#endif
