#ifndef LT_MEM_H
#define LT_MEM_H

#include <lt/lt.h>

#define LT_ALLOC_DEFAULT_ALIGN 16


// libc
extern void* memset(void* data, int c, usz size);
extern void* memcpy(void* dst, const void* src, usz size);
extern void* memmove(void* dst, const void* src, usz size);
extern int memcmp(const void* dst, const void* src, usz size);

extern void* malloc(usz size);
extern void* realloc(void* addr, usz size);
extern void free(void* addr);


// page_size.c
extern usz (*lt_get_pagesize)(void);


// vmem.c
void* lt_vmalloc(usz size);
void lt_vmfree(void* addr, usz size);


// alloc.c
typedef void*(*lt_alloc_callback_t)(void* usr, usz size);
typedef void(*lt_free_callback_t)(void* usr, void* mem);
typedef void*(*lt_realloc_callback_t)(void* usr, void* mem, usz size);
typedef void*(*lt_size_callback_t)(void* usr, void* mem);

typedef
struct lt_alloc {
	lt_alloc_callback_t alloc;
	lt_free_callback_t free;
	lt_realloc_callback_t realloc;
	lt_size_callback_t size;
} lt_alloc_t;

#define LT_ALLOC_INTERFACE(alloc, free, realloc, size) \
		((lt_alloc_t){ (lt_alloc_callback_t)(alloc), (lt_free_callback_t)(free), (lt_realloc_callback_t)(realloc), (lt_size_callback_t)(size) })

static LT_INLINE
void* lt_malloc(void* alloc_, usz size) {
	lt_alloc_t* alloc = alloc_;
	return alloc->alloc(alloc, size);
}

static LT_INLINE
void lt_mfree(void* alloc_, void* mem) {
	lt_alloc_t* alloc = alloc_;
	alloc->free(alloc, mem);
}

static LT_INLINE
void* lt_mrealloc(void* alloc_, void* mem, usz size) {
	lt_alloc_t* alloc = alloc_;
	return alloc->realloc(alloc, mem, size);
}

static LT_INLINE
void* lt_msize(void* alloc_, void* mem) {
	lt_alloc_t* alloc = alloc_;
	return alloc->size(alloc, mem);
}

static LT_INLINE
void* lt_memdup(void* alloc_, void* mem, usz size) {
	lt_alloc_t* alloc = alloc_;
	void* newmem = lt_malloc(alloc, size);
	if (!newmem)
		return NULL;
	memcpy(newmem, mem, size);
	return newmem;
}

static LT_INLINE
lstr_t lt_strdup(void* alloc, lstr_t str) {
	return LSTR(lt_memdup(alloc, str.str, str.len), str.len);
}

// heap.c
extern lt_alloc_t* lt_libc_heap;


// arena.c
typedef
struct lt_arena {
	lt_alloc_t interf;
	void* base;
	usz size;
	void* top;
	usz flags;
	lt_alloc_t* parent;
} lt_arena_t;

lt_arena_t* lt_amcreatem(lt_alloc_t* parent, void* mem, usz size, usz flags);
lt_arena_t* lt_amcreate(lt_alloc_t* parent, usz size, usz flags);
void lt_amdestroy(lt_arena_t* arena);

void* lt_amsave(lt_arena_t* arena);
void lt_amrestore(lt_arena_t* arena, void* restore_point);

void* lt_amalloc(lt_arena_t* arena, usz size);
void lt_amfree(lt_arena_t* arena, void* ptr);
void* lt_amrealloc(lt_arena_t* arena, void* ptr, usz new_size);
usz lt_amsize(lt_arena_t* arena, void* ptr);

b8 lt_amleaked(lt_arena_t* arena);

// pool.c
typedef struct lt_pool {
	lt_alloc_t interf;
	void* base;
	usz chunk_size;
	usz chunk_count;
	usz size;
	void* head;
	usz flags;
	lt_alloc_t* parent;
} lt_pool_t;

lt_pool_t* lt_pmcreatem(lt_alloc_t* parent, void* mem, usz size, usz chunk_size, usz flags);
lt_pool_t* lt_pmcreate(lt_alloc_t* parent, usz size, usz chunk_size, usz flags);
void lt_pmdestroy(lt_pool_t* pool);

void lt_pmreset(lt_pool_t* pool);

void* lt_pmalloc(lt_pool_t* pool);
void lt_pmfree(lt_pool_t* pool, void* chunk);
usz lt_pmsize(lt_pool_t* pool, void* chunk);

#endif
