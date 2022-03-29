#ifndef LT_MEM_H
#define LT_MEM_H

#include <lt/lt.h>

// libc
extern void* memset(void* data, int c, usz size);
extern void* memcpy(void* dst, const void* src, usz size);
extern void* memmove(void* dst, const void* src, usz size);

extern void* malloc(usz size);
extern void* realloc(void* addr, usz size);
extern void free(void* addr);

// page_size.c
extern usz (*lt_page_size)(void);

// vmem.c
void* lt_vmem_alloc(usz page_count);
void lt_vmem_free(void* addr, usz page_count);

// alloc.c
typedef void*(*lt_reserve_callback_t)(void* usr, usz size);
typedef void(*lt_relinq_callback_t)(void* usr, void* mem);
typedef void*(*lt_resize_callback_t)(void* usr, void* mem, usz size);

typedef
struct lt_alloc {
	void* usr;
	lt_reserve_callback_t reserve;
	lt_relinq_callback_t relinq;
	lt_resize_callback_t resize;
} lt_alloc_t;

#define LT_ALLOC_INTERFACE(usr, reserve, relinq, resize) \
		((lt_alloc_t){ (usr), (reserve), (relinq), (resize) })

static LT_INLINE
void* lt_reserve(lt_alloc_t* alloc, usz size) {
	return alloc->reserve(alloc->usr, size);
}

static LT_INLINE
void lt_relinq(lt_alloc_t* alloc, void* mem) {
	alloc->relinq(alloc->usr, mem);
}

static LT_INLINE
void* lt_resize(lt_alloc_t* alloc, void* mem, usz size) {
	return alloc->resize(alloc->usr, mem, size);
}

// heap.c
lt_alloc_t lt_heap_interface(void);

// arena.c
typedef struct lt_arena lt_arena_t;

lt_arena_t* lt_arena_alloc(usz size);
void lt_arena_free(lt_arena_t* arena);

void* lt_arena_reserve(lt_arena_t* arena, usz size);

lt_alloc_t lt_arena_interface(lt_arena_t* arena);

typedef struct lt_arestore {
	void* mem_pos;
	usz free_bytes;
} lt_arestore_t;

lt_arestore_t lt_arena_save(lt_arena_t* arena);
void lt_arena_restore(lt_arena_t* arena, lt_arestore_t* restore_point);

// pool.c
typedef struct lt_pool lt_pool_t;

lt_pool_t* lt_pool_alloc(usz chunk_size, usz count);
void lt_pool_free(lt_pool_t* pool);

void lt_pool_reset(lt_pool_t* pool);

void* lt_pool_reserve(lt_pool_t* pool);
void lt_pool_relinq(lt_pool_t* pool, void* chunk);


#endif
