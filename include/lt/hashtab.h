#include <lt/bits.h>
#include <lt/fwd.h>

#define LT_HASHTAB_SIZE 256
#define LT_HASHTAB_MASK (LT_HASHTAB_SIZE-1)

typedef
struct lt_hashtab {
	usz counts[LT_HASHTAB_SIZE];
	void** values[LT_HASHTAB_SIZE];
} lt_hashtab_t;

void lt_hashtab_init(lt_hashtab_t* htab);
void lt_hashtab_free(lt_hashtab_t* htab, lt_alloc_t* alloc);

void lt_hashtab_insert(lt_hashtab_t* htab, u32 hash, void* val, lt_alloc_t* alloc);

#define LT_HASHTAB_FIND_IMPL(htab, hash, it, cond) { \
	u32 _idx = hash & LT_HASHTAB_MASK; \
	usz _count = (htab)->counts[_idx]; \
	void** _vals = (htab)->values[_idx]; \
	if (_count == 1) { \
		(it) = (void*)_vals; \
		if (cond) \
			return (void*)_vals; \
		return NULL; \
	} \
	\
	for (usz _i = 0; _i < _count; ++_i) { \
		void* _val = _vals[_i]; \
		(it) = _val; \
		if (cond) \
			return (void*)_val; \
	} \
	\
	return NULL; \
}

static LT_INLINE
u32 lt_hash(void* _mem, usz size) {
	u8* mem = _mem;
	u32 hash = size;

	for (usz i = 0; i < size; ++i)
		hash ^= lt_rotl32(hash, 7) ^ mem[i];

	return hash;
}

