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

void lt_hashtab_init(lt_hashtab_t* htab);
void lt_hashtab_free(lt_hashtab_t* htab, lt_alloc_t* alloc);

void lt_hashtab_insert(lt_hashtab_t* htab, u32 hash, void* val, lt_alloc_t* alloc);

typedef
struct lt_hashtab_iterator {
	u32 i, j;
	void* elem;
} lt_hashtab_iterator_t;

static LT_INLINE
void* lt_hashtab_next(lt_hashtab_t* tab, lt_hashtab_iterator_t* it) {
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

#define LT_DEFINE_HASHTAB_FIND_FUNC(T, Tkey, name, is_equal)	\
	T* name(lt_hashtab_t* htab, u32 hash, Tkey key) {			\
		LT_HASHTAB_FIND_BODY(T, is_equal)						\
	}


static LT_INLINE
u32 lt_hash(void* mem_, usz size) {
	u8* mem = mem_;
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
