#include <lt/mem.h>
#include <lt/align.h>

typedef
struct lt_arena {
	void* mem_start;
	void* mem_pos;
	usz free_bytes;
	usz page_count;
} lt_arena_t;

static LT_INLINE
lt_arena_t lt_arena_make(void* mem, usz size, usz page_count) {
	lt_arena_t arena;
	arena.mem_start = mem;
	arena.mem_pos = mem;
	arena.free_bytes = size;
	arena.page_count = page_count;
	return arena;
}

lt_arena_t* lt_arena_alloc(usz size) {
	if (!size)
		return NULL;

	usz page_size = lt_page_size();
	usz header_size = lt_word_align_fwd(sizeof(lt_arena_t));
	size += lt_pad(size + header_size, page_size);

	usz page_count = (size + header_size) / page_size;

	void* mem = lt_vmem_alloc(page_count);
	if (!mem)
		return NULL;

	*(lt_arena_t*)mem = lt_arena_make((char*)mem + header_size, size, page_count);
	return (lt_arena_t*)mem;
}

void lt_arena_free(lt_arena_t* arena) {
	lt_vmem_free(arena, arena->page_count);
}

void* lt_arena_reserve(lt_arena_t* arena, usz size) {
	usz aligned_size = lt_word_align_fwd(size);
// If this is ever false then we should crash anyway since the
// program has run out of memory.
// 	if (aligned_size > arena->free_bytes)
// 		return NULL;
	void* ptr = arena->mem_pos;
	arena->free_bytes -= aligned_size;
	arena->mem_pos = (char*)arena->mem_pos + aligned_size;
	return ptr;
}

lt_arestore_t lt_arena_save(lt_arena_t* arena) {
	return (lt_arestore_t){ arena->mem_pos, arena->free_bytes };
}

void lt_arena_restore(lt_arena_t* arena, lt_arestore_t* restore_point) {
	arena->mem_pos = restore_point->mem_pos;
	arena->free_bytes = restore_point->free_bytes;
}

