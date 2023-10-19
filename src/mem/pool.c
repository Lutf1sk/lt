#include <lt/mem.h>
#include <lt/align.h>
#include <lt/debug.h>

static
void* lt_pmalloc_if(lt_pool_t* pool, usz size) {
	if (size > pool->chunk_size) {
		lt_werrf("pool allocation failed, size is larger than chunk size\n");
		return NULL;
	}
	return lt_pmalloc(pool);
}

static
void* lt_pmrealloc_if(lt_pool_t* pool, void* chunk, usz size) {
	if (size > pool->chunk_size) {
		lt_werrf("pool allocation failed, size is larger than chunk size\n");
		return NULL;
	}
	if (!chunk)
		return lt_pmalloc(pool);
	return chunk;
}

lt_pool_t* lt_pmcreatem(lt_alloc_t* parent, void* mem, usz size, usz chunk_size, usz flags) {
	usz headersz = lt_align_fwd(sizeof(lt_pool_t), LT_ALLOC_DEFAULT_ALIGN);
	if (size < headersz)
		return NULL;

	lt_pool_t* pool = mem;
	pool->base = mem;
	pool->chunk_size = chunk_size;
	pool->size = size;
	pool->chunk_count = (size - headersz) / chunk_size;
	pool->head = NULL;
	pool->flags = flags;
	pool->parent = parent;
	pool->interf = LT_ALLOC_INTERFACE(lt_pmalloc_if, lt_pmalloc_if, lt_pmfree, lt_pmrealloc_if, lt_pmrealloc_if);

	lt_pmreset(pool);
	return pool;
}

lt_pool_t* lt_pmcreate(lt_alloc_t* parent, usz size, usz chunk_size, usz flags) {
	usz headersz = lt_align_fwd(sizeof(lt_pool_t), LT_ALLOC_DEFAULT_ALIGN);
	if (size < headersz)
		return NULL;

	void* base = NULL;
	if (!parent) {
		size = lt_align_fwd(size, lt_get_pagesize());
		base = lt_vmalloc(size);
	}
	else {
		size = lt_align_fwd(size, LT_ALLOC_DEFAULT_ALIGN);
		base = lt_malloc(parent, size);
	}
	if (!base)
		return NULL;
	return lt_pmcreatem(parent, base, size, chunk_size, flags);
}

void lt_pmdestroy(lt_pool_t* pool) {
	if (pool->parent)
		lt_mfree(pool->parent, pool);
	else
		lt_vmfree(pool, pool->size);
}

void lt_pmreset(lt_pool_t* pool) {
	void** last = &pool->head;
	void* it = pool->base;
	for (usz i = 0; i < pool->chunk_count; ++i) {
		*last = it;
		last = it;
		it = (u8*)it + pool->chunk_size;
	}
}

void* lt_pmalloc(lt_pool_t* pool) {
	void** head = pool->head;
	if (!head) {
		lt_werrf("pool allocation failed, not enough memory\n");
		return NULL;
	}
	pool->head = *head;
	return head;
}

void lt_pmfree(lt_pool_t* pool, void* chunk) {
	*(void**)chunk = pool->head;
	pool->head = chunk;
}

