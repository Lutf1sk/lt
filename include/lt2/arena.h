#pragma once

typedef struct arena {
	u8* top;
	u8* base;
	u8* end;
} arena_t;

INLINE
void* arena_alloc(arena_t* arena, usz size) {
    u8* top = arena->top;
    u8* new_top = top + ((size + 31) & -32);
    if UNLIKELY (new_top > arena->end)
        return NULL;
    arena->top = new_top;
    return top;
}

INLINE
void* arena_alloc_unsafe(arena_t* arena, usz size) {
    u8* top = (void*)(((usz)arena->top + 31) & -32);
    arena->top = top + size;
    return top;
}

INLINE
void arena_reset(arena_t* arena) {
	arena->top = arena->base;
}

