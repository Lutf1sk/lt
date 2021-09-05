#include <lt/mem.h>
#include <lt/align.h>

typedef struct lt_pool_node {
	struct lt_pool_node* next;
} lt_pool_node_t;

typedef struct lt_pool {
	void* mem;
	usz page_count;
	usz chunk_count;
	usz chunk_size;
	lt_pool_node_t* head;
	struct lt_pool* next;
} lt_pool_t;

static LT_INLINE
lt_pool_t lt_pool_make(void* mem, usz chunk_size, usz chunk_count, usz page_count) {
	lt_pool_t pool;
	pool.mem = mem;
	pool.page_count = page_count;
	pool.chunk_count = chunk_count;
	pool.chunk_size = chunk_size;
	pool.head = NULL;
	pool.next = NULL;
	return pool;
}


lt_pool_t* lt_pool_alloc(usz chunk_size, usz count) {
	if (!count)
		return NULL;

	usz header_size = lt_word_align_fwd(sizeof(lt_pool_t));
	usz size = chunk_size * count;
	size += lt_pad(size + header_size, lt_page_size());

	usz page_count = (size + header_size) / lt_page_size();
	void* mem = lt_vmem_alloc(page_count);
	if (!mem)
		return NULL;

	if (chunk_size < sizeof(lt_pool_node_t))
		chunk_size = sizeof(lt_pool_node_t);
	chunk_size = lt_word_align_fwd(chunk_size);

	lt_pool_t* header = mem;
	*header = lt_pool_make((char*)mem + header_size, chunk_size, size / chunk_size, page_count);
	lt_pool_reset(header);

	return header;
}

void lt_pool_free(lt_pool_t* pool) {
	if (pool->next)
		lt_pool_free(pool->next);
	lt_vmem_free(pool, pool->page_count);
}

void lt_pool_reset(lt_pool_t* pool) {
	usz chunk_size = pool->chunk_size;
	char* it_end = (char*)pool->mem + pool->chunk_count * chunk_size;
	lt_pool_node_t* last_node = NULL;
	for (char* it = pool->mem; it + chunk_size < it_end; it += chunk_size) {
		lt_pool_node_t* node = (lt_pool_node_t*)it;
		node->next = last_node;
		last_node = node;
	}
	pool->head = last_node;

	if (pool->next)
		lt_pool_reset(pool->next);
}



void* lt_pool_reserve(lt_pool_t* pool);
void lt_pool_relinq(lt_pool_t* pool, void* chunk);


