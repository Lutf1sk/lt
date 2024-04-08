#include <lt/mem.h>
#include <lt/align.h>
#include <lt/debug.h>
#include <lt/math.h>

typedef
struct node {
	usz size;
	usz caller;
	u8 free;
} node_t;

static LT_INLINE
node_t* node_from_ptr(void* ptr) {
	return LT_SUBPTR(ptr, lt_align_fwd(sizeof(node_t), LT_ALLOC_DEFAULT_ALIGN));
}

static LT_INLINE
void* ptr_from_node(node_t* ptr) {
	return LT_ADDPTR(ptr, lt_align_fwd(sizeof(node_t), LT_ALLOC_DEFAULT_ALIGN));
}

lt_arena_t* lt_amcreatem(lt_alloc_t* parent, void* mem, usz size, usz flags) {
	if (size < sizeof(lt_arena_t))
		return NULL;

	lt_arena_t* arena = mem;
	arena->base = mem;
	arena->size = size;
	arena->top = (u8*)mem + lt_align_fwd(sizeof(lt_arena_t), LT_ALLOC_DEFAULT_ALIGN);
	arena->flags = flags;
	arena->parent = parent;
	arena->interf = LT_ALLOC_INTERFACE(lt_amalloc, lt_amalloc_for_caller, lt_amfree, lt_amrealloc, lt_amrealloc_for_caller);
	return arena;
}

lt_arena_t* lt_amcreate(lt_alloc_t* parent, usz size, usz flags) {
	if (size < sizeof(lt_arena_t))
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

	return lt_amcreatem(parent, base, size, flags);
}

void lt_amdestroy(const lt_arena_t arena[static 1]) {
	if (arena->parent)
		lt_mfree(arena->parent, arena);
	else
		lt_vmfree((void*)arena, arena->size);
}

void* lt_amsave(const lt_arena_t arena[static 1]) {
	return arena->top;
}

void lt_amrestore(lt_arena_t arena[static 1], void* restore_point) {
	arena->top = restore_point;
}

void* lt_amalloc_for_caller(lt_arena_t arena[static 1], usz size, const void* caller) {
	size = lt_align_fwd(size, LT_ALLOC_DEFAULT_ALIGN);

	node_t* start = arena->top;
	u8* data_start = ptr_from_node(start);
	u8* new_top = data_start + size;
	if (new_top > (u8*)arena->base + arena->size) {
		lt_werrf("arena allocation failed, not enough memory\n");
		return NULL;
	}
	start->size = size;
	start->caller = (usz)caller;
	start->free = 0;
	arena->top = new_top;
	return data_start;
}

LT_FLATTEN
void* lt_amalloc(lt_arena_t arena[static 1], usz size) {
	return lt_amalloc_for_caller(arena, size, LT_RETURN_ADDR - 1);
}

void lt_amfree(lt_arena_t arena[static 1], const void* ptr) {
	node_t* node = node_from_ptr((void*)ptr);
	if (node->free)
		lt_werrf("called mfree on free'd block\n");

	node->free = 1;
	if ((u8*)ptr + node->size == arena->top)
		arena->top = node;
}

void* lt_amrealloc_for_caller(lt_arena_t arena[static 1], void* ptr, usz new_size, const void* caller) {
	if (!ptr)
		return lt_amalloc_for_caller(arena, new_size, caller);

	new_size = lt_align_fwd(new_size, LT_ALLOC_DEFAULT_ALIGN);

	node_t* node = node_from_ptr(ptr);
	if (node->free)
		lt_werrf("called mrealloc on free'd block\n");

	if ((u8*)ptr + node->size == arena->top) {
		u8* new_top = (u8*)ptr + new_size;
		if (new_top > (u8*)arena->base + arena->size)
			return NULL;
		node->size = new_size;
		arena->top = new_top;
		return ptr;
	}

	void* new_ptr = lt_amalloc_for_caller(arena, new_size, caller);
	if (!new_ptr)
		return NULL;
	memcpy(new_ptr, ptr, node->size);
	lt_amfree(arena, ptr);
	return new_ptr;
}

LT_FLATTEN
void* lt_amrealloc(lt_arena_t arena[static 1], void* ptr, usz new_size) {
	return lt_amrealloc_for_caller(arena, ptr, new_size, LT_RETURN_ADDR);
}

// void lt_amleak_check(lt_arena_t* arena) {
// 	if ((u8*)arena->base == (u8*)arena->top + sizeof(lt_arena_t))
// 		return;

// 	node_t* it = LT_ADDPTR(arena->base, lt_align_fwd(sizeof(lt_arena_t), LT_ALLOC_DEFAULT_ALIGN));
// 	while (it < (node_t*)arena->top) {
// 		if (!it->free) {
// 			lt_werrf("block 0x%hz leaked from ", ptr_from_node(it));
// 			lt_print_instr_ptr(it->caller);
// 		}
// 		it = LT_ADDPTR(ptr_from_node(it), it->size);
// 	}
// }

