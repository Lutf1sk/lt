#pragma once

#include <lt2/common.h>

INLINE
u32 wang_hash(u32 seed) {
	seed = (seed ^ 61) ^ (seed >> 16);
	seed *= 9;
	seed ^= seed >> 4;
	seed *= 0x27d4eb2d;
	seed ^= seed >> 15;
	return seed;
}

INLINE
u32 pcg_hash(u32 seed) {
	u32 state = seed * 747796405u + 2891336453u;
	u32 word = ((state >> ((state >> 28u) + 4u)) ^ state) * 277803737u;
	return (word >> 22u) ^ word;
}

#define LT_FNV_PRIME32 16777619
#define LT_FNV_PRIME64 1099511628211

#define LT_FNV_BASE32 2166136261
#define LT_FNV_BASE64 14695981039346656037

INLINE
u32 fnv1a_hashs(const char str[static 1]) {
	u32 hash = LT_FNV_BASE32;
	for (const char* it = str; *it; ++it) {
		hash = (hash ^ (u32)*it) * LT_FNV_PRIME32;
	}
	return hash;
}

INLINE
u32 fnv1a_hashls(lstr_t str) {
	u32 hash = LT_FNV_BASE32;
	for (char* it = str.str, *end = it + str.len; it < end; ++it) {
		hash = (hash ^ (u32)*it) * LT_FNV_PRIME32;
	}
	return hash;
}

INLINE
u16 fold_hash_32to16(u32 hash) {
	return (hash >> 16) ^ hash;
}


