#ifndef LT_MEM_H
#define LT_MEM_H

#include <lt/lt.h>
#include <lt/debug.h>

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
void* lt_vmalloc LT_DEBUG_ARGS(usz size);
void lt_vmfree LT_DEBUG_ARGS(void* addr, usz size);

#define lt_vmalloc(...) LT_DEBUG_CALL(lt_vmalloc, __VA_ARGS__)
#define lt_vmfree(...) LT_DEBUG_CALL(lt_vmfree, __VA_ARGS__)

// alloc.c

typedef void*(*lt_alloc_callback_t)LT_DEBUG_ARGS(void* usr, usz size);
typedef void(*lt_free_callback_t)LT_DEBUG_ARGS(void* usr, void* mem);
typedef void*(*lt_realloc_callback_t)LT_DEBUG_ARGS(void* usr, void* mem, usz size);
typedef void*(*lt_size_callback_t)LT_DEBUG_ARGS(void* usr, void* mem);

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
void* lt_malloc LT_DEBUG_ARGS(void* alloc_, usz size) {
	lt_alloc_t* alloc = alloc_;
	return LT_DEBUG_FWD(alloc->alloc, alloc, size);
}

static LT_INLINE
void lt_mfree LT_DEBUG_ARGS(void* alloc_, void* mem) {
	lt_alloc_t* alloc = alloc_;
	LT_DEBUG_FWD(alloc->free, alloc, mem);
}

static LT_INLINE
void* lt_mrealloc LT_DEBUG_ARGS(void* alloc_, void* mem, usz size) {
	lt_alloc_t* alloc = alloc_;
	return LT_DEBUG_FWD(alloc->realloc, alloc, mem, size);
}

static LT_INLINE
void* lt_msize LT_DEBUG_ARGS(void* alloc_, void* mem) {
	lt_alloc_t* alloc = alloc_;
	return LT_DEBUG_FWD(alloc->size, alloc, mem);
}

static LT_INLINE
void* lt_memdup LT_DEBUG_ARGS(void* alloc_, void* mem, usz size) {
	lt_alloc_t* alloc = alloc_;
	void* newmem = LT_DEBUG_FWD(lt_malloc, alloc, size);
	if (!newmem)
		return NULL;
	memcpy(newmem, mem, size);
	return newmem;
}

static LT_INLINE
lstr_t lt_strdup LT_DEBUG_ARGS(void* alloc, lstr_t str) {
	return LSTR(LT_DEBUG_FWD(lt_memdup, alloc, str.str, str.len), str.len);
}

#define lt_malloc(...) LT_DEBUG_CALL(lt_malloc, __VA_ARGS__)
#define lt_mfree(...) LT_DEBUG_CALL(lt_mfree, __VA_ARGS__)
#define lt_mrealloc(...) LT_DEBUG_CALL(lt_mrealloc, __VA_ARGS__)
#define lt_msize(...) LT_DEBUG_CALL(lt_msize, __VA_ARGS__)
#define lt_memdup(...) LT_DEBUG_CALL(lt_memdup, __VA_ARGS__)
#define lt_strdup(...) LT_DEBUG_CALL(lt_strdup, __VA_ARGS__)

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

lt_arena_t* lt_amcreatem LT_DEBUG_ARGS(lt_alloc_t* parent, void* mem, usz size, usz flags);
lt_arena_t* lt_amcreate LT_DEBUG_ARGS(lt_alloc_t* parent, usz size, usz flags);
void lt_amdestroy LT_DEBUG_ARGS(lt_arena_t* arena);

void* lt_amsave(lt_arena_t* arena);
void lt_amrestore(lt_arena_t* arena, void* restore_point);

void* lt_amalloc LT_DEBUG_ARGS(lt_arena_t* arena, usz size);
void lt_amfree LT_DEBUG_ARGS(lt_arena_t* arena, void* ptr);
void* lt_amrealloc LT_DEBUG_ARGS(lt_arena_t* arena, void* ptr, usz new_size);
usz lt_amsize LT_DEBUG_ARGS(lt_arena_t* arena, void* ptr);

b8 lt_amleaked(lt_arena_t* arena);

#define lt_amcreatem(...) LT_DEBUG_CALL(lt_amcreatem, __VA_ARGS__)
#define lt_amcreate(...) LT_DEBUG_CALL(lt_amcreate, __VA_ARGS__)
#define lt_amdestroy(...) LT_DEBUG_CALL(lt_amdestroy, __VA_ARGS__)

#define lt_amalloc(...) LT_DEBUG_CALL(lt_amalloc, __VA_ARGS__)
#define lt_amfree(...) LT_DEBUG_CALL(lt_amfree, __VA_ARGS__)
#define lt_amrealloc(...) LT_DEBUG_CALL(lt_amrealloc, __VA_ARGS__)
#define lt_amsize(...) LT_DEBUG_CALL(lt_amsize, __VA_ARGS__)


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

lt_pool_t* lt_pmcreatem LT_DEBUG_ARGS(lt_alloc_t* parent, void* mem, usz size, usz chunk_size, usz flags);
lt_pool_t* lt_pmcreate LT_DEBUG_ARGS(lt_alloc_t* parent, usz size, usz chunk_size, usz flags);
void lt_pmdestroy LT_DEBUG_ARGS(lt_pool_t* pool);

void lt_pmreset(lt_pool_t* pool);

void* lt_pmalloc LT_DEBUG_ARGS(lt_pool_t* pool);
void lt_pmfree LT_DEBUG_ARGS(lt_pool_t* pool, void* chunk);
usz lt_pmsize LT_DEBUG_ARGS(lt_pool_t* pool, void* chunk);

#define lt_pmcreatem(...) LT_DEBUG_CALL(lt_pmcreatem, __VA_ARGS__)
#define lt_pmcreate(...) LT_DEBUG_CALL(lt_pmcreate, __VA_ARGS__)
#define lt_pmdestroy(...) LT_DEBUG_CALL(lt_pmdestroy, __VA_ARGS__)

#define lt_pmalloc(...) LT_DEBUG_CALL(lt_pmalloc, __VA_ARGS__)
#define lt_pmfree(...) LT_DEBUG_CALL(lt_pmfree, __VA_ARGS__)
#define lt_pmsize(...) LT_DEBUG_CALL(lt_pmsize, __VA_ARGS__)

#endif
