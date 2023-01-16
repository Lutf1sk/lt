#include <lt/mem.h>
#include <lt/align.h>

lt_arena_t* lt_amcreatem(lt_alloc_t* parent, void* mem, usz size, usz flags) {
	if (size < sizeof(lt_arena_t))
		return NULL;

	lt_arena_t* arena = mem;
	arena->base = mem;
	arena->size = size;
	arena->top = mem + sizeof(lt_arena_t);
	arena->flags = flags;
	arena->parent = parent;
	arena->interf = LT_ALLOC_INTERFACE(lt_amalloc, lt_amfree, lt_amrealloc, lt_amsize);
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

void lt_amdestroy(lt_arena_t* arena) {
	if (arena->parent)
		lt_mfree(arena->parent, arena);
	else
		lt_vmfree(arena, arena->size);
}

void* lt_amsave(lt_arena_t* arena) {
	return arena->top;
}

void lt_amrestore(lt_arena_t* arena, void* restore_point) {
	arena->top = restore_point;
}

void* lt_amalloc(lt_arena_t* arena, usz size) {
	u8* start = (u8*)lt_align_fwd((usz)arena->top, LT_ALLOC_DEFAULT_ALIGN);
	u8* data_start = start + LT_ALLOC_DEFAULT_ALIGN;
	void* new_top = data_start + size;
	if (new_top > arena->base + arena->size)
		return NULL;
	*(usz*)start = size;
	arena->top = new_top;
	return data_start;
}

void lt_amfree(lt_arena_t* arena, void* ptr) {
	usz* psize = (usz*)((usz)ptr - LT_ALLOC_DEFAULT_ALIGN);
	if ((u8*)ptr + *psize == arena->top)
		arena->top = psize;
}

void* lt_amrealloc(lt_arena_t* arena, void* ptr, usz new_size) {
	if (!ptr)
		return lt_amalloc(arena, new_size);

	usz* psize = (usz*)((usz)ptr - LT_ALLOC_DEFAULT_ALIGN);
	if ((u8*)ptr + *psize == arena->top) {
		void* new_top = (u8*)ptr + new_size;
		if (new_top > arena->base + arena->size)
			return NULL;
		*psize = new_size;
		arena->top = new_top;
		return ptr;
	}

	void* new_ptr = lt_amalloc(arena, new_size);
	if (!new_ptr)
		return NULL;
	memcpy(new_ptr, ptr, *psize);
	return new_ptr;
}

usz lt_amsize(lt_arena_t* arena, void* ptr) {
	return *(usz*)((usz)ptr - LT_ALLOC_DEFAULT_ALIGN);
}

b8 lt_amleaked(lt_arena_t* arena) {
	return arena->base != arena->top + sizeof(lt_arena_t);
}

