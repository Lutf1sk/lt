#include <lt/hashtab.h>
#include <lt/align.h>
#include <lt/mem.h>

#define LT_HASHTAB_ALLOC_COUNT 16

#include <lt/io.h>

void lt_hashtab_init(lt_hashtab_t* htab) {
	memset(htab, 0, sizeof(lt_hashtab_t));
}

void lt_hashtab_free(lt_hashtab_t* htab, lt_alloc_t* alloc) {
	for (usz i = 0; i < LT_HASHTAB_SIZE; ++i) {
		if (htab->counts[i] <= 1)
			continue;
		lt_mfree(alloc, htab->values[i]);
	}
}

void lt_hashtab_insert(lt_hashtab_t* htab, u32 hash, void* val, lt_alloc_t* alloc) {
	u32 idx = hash & LT_HASHTAB_MASK;
	usz count = htab->counts[idx];
	void** vals = htab->values[idx];

	if (!count) {
		htab->values[idx] = val;
		++htab->counts[idx];
		return;
	}
	else if (count == 1) {
		void* old_val = vals;
		vals = lt_malloc(alloc, LT_HASHTAB_ALLOC_COUNT * sizeof(void*));
		LT_ASSERT(vals);
		vals[0] = old_val;

		htab->values[idx] = vals;
	}
	else if (lt_is_pow2(count)) {
		vals = lt_mrealloc(alloc, vals, (count << 1) * sizeof(void*));
		LT_ASSERT(vals);

		htab->values[idx] = vals;
	}

	vals[htab->counts[idx]++] = val;
}

