#ifndef LT_HASHTAB_H
#define LT_HASHTAB_H 1

#include <lt/math.h>
#include <lt/fwd.h>

#define LT_HASHTAB_SIZE 256
#define LT_HASHTAB_MASK (LT_HASHTAB_SIZE-1)

typedef
struct lt_hashtab {
	usz counts[LT_HASHTAB_SIZE];
	void** values[LT_HASHTAB_SIZE];
} lt_hashtab_t;

void lt_hashtab_init(lt_hashtab_t out_htab[static 1]);
void lt_hashtab_free(const lt_hashtab_t htab[static 1], lt_alloc_t alloc[static 1]);

void lt_hashtab_insert(lt_hashtab_t htab[static 1], u32 hash, void* val, lt_alloc_t alloc[static 1]);

typedef
struct lt_hashtab_iterator {
	u32 i, j;
	void* elem;
} lt_hashtab_iterator_t;

static LT_INLINE
void* lt_hashtab_next(const lt_hashtab_t tab[static 1], lt_hashtab_iterator_t it[static 1]) {
	while (it->i < LT_HASHTAB_SIZE) {
		usz count = tab->counts[it->i];
		if (count == 1) {
			return tab->values[it->i++];
		}

		if (it->j < count) {
			return tab->values[it->i][it->j++];
		}

		++it->i;
		it->j = 0;
	}

	return NULL;
}

#define lt_foreach_hashtab_entry(T, ent, tab) \
	for (lt_hashtab_iterator_t it = {0}; (it.elem = lt_hashtab_next((tab), &it)); ) \
		for (T* ent = it.elem; ent; ent = 0)

#define LT_HASHTAB_FIND_BODY(T, is_equal)	\
	u32 idx = hash & LT_HASHTAB_MASK;		\
	usz count = (htab)->counts[idx];		\
	void** vals = (htab)->values[idx];		\
	if (count == 1) {						\
		if (is_equal(key, (T*)vals))		\
			return (T*)vals;				\
		return NULL;						\
	}										\
											\
	for (usz i = 0; i < count; ++i) {		\
		if (is_equal(key, (T*)vals[i]))		\
			return vals[i];					\
	}										\
											\
	return NULL;

#define LT_DEFINE_HASHTAB_FIND_FUNC(T, Tkey, name, is_equal)			\
	T* name(const lt_hashtab_t htab[static 1], u32 hash, Tkey key) {	\
		LT_HASHTAB_FIND_BODY(T, is_equal)								\
	}


static LT_INLINE
u32 lt_hash(const void* mem_, usz size) {
	const u8* mem = mem_;
	u32 hash = size;

	for (usz i = 0; i < size; ++i)
		hash ^= lt_rotl32(hash, 7) ^ mem[i];

	return hash;
}

static LT_INLINE
u32 lt_hashls(lstr_t str) {
	return lt_hash(str.str, str.len);
}

#endif
