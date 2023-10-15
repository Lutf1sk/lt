#ifndef LT_RAND_H
#define LT_RAND_H 1

#include <lt/lt.h>

static LT_INLINE
u32 lt_wang_hash(u32 seed) {
	seed = (seed ^ 61) ^ (seed >> 16);
	seed *= 9;
	seed ^= seed >> 4;
	seed *= 0x27d4eb2d;
	seed ^= seed >> 15;
	return seed;
}

static LT_INLINE
u32 lt_pcg_hash(u32 seed) {
	u32 state = seed * 747796405u + 2891336453u;
	u32 word = ((state >> ((state >> 28u) + 4u)) ^ state) * 277803737u;
	return (word >> 22u) ^ word;
}

#endif
