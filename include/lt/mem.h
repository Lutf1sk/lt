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

// custom

static LT_INLINE
usz lt_mcopy(void* dst, const void* src, usz size) {
	memcpy(dst, src, size);
	return size;
}

static LT_INLINE
usz lt_mzero(void* dst, usz size) {
	memset(dst, 0, size);
	return size;
}

static LT_INLINE
usz lt_mset8(void* dst, u8 v, usz size) {
	memset(dst, v, size);
	return size;
}

usz lt_mset16(void* dst, u16 v, usz size);
usz lt_mset32(void* dst, u32 v, usz size);

// page_size.c
extern usz (*lt_get_pagesize)(void);


// vmem.c
void* lt_vmalloc(usz size);
void lt_vmfree(void* addr, usz size);

// alloc.c
#define LT_ALLOC_INTERFACE_INIT(alloc, alloc_for_caller, free, realloc, realloc_for_caller) { \
		(lt_alloc_fn_t)(alloc), \
		(lt_alloc_for_caller_fn_t)(alloc_for_caller), \
		(lt_free_fn_t)(free), \
		(lt_realloc_fn_t)(realloc), \
		(lt_realloc_for_caller_fn_t)(realloc_for_caller), \
	}
#define LT_ALLOC_INTERFACE(alloc, alloc_for_caller, free, realloc, realloc_for_caller) ((lt_alloc_t) { \
		(lt_alloc_fn_t)(alloc), \
		(lt_alloc_for_caller_fn_t)(alloc_for_caller), \
		(lt_free_fn_t)(free), \
		(lt_realloc_fn_t)(realloc), \
		(lt_realloc_for_caller_fn_t)(realloc_for_caller), \
	})

static LT_NOINLINE
void* lt_malloc(lt_alloc_t alloc[static 1], usz size) {
	return alloc->alloc_for_caller(alloc, size, (u8*)LT_RETURN_ADDR - 1);
}

static LT_INLINE
void lt_mfree(lt_alloc_t alloc[static 1], const void* mem) {
	alloc->free(alloc, mem);
}

static LT_NOINLINE
void* lt_mrealloc(lt_alloc_t alloc[static 1], void* mem, usz size) {
	return alloc->realloc_for_caller(alloc, mem, size, (u8*)LT_RETURN_ADDR - 1);
}

static LT_NOINLINE
void* lt_memdup(lt_alloc_t alloc[static 1], const void* mem, usz size) {
	void* newmem = alloc->alloc_for_caller(alloc, size, (u8*)LT_RETURN_ADDR - 1);
	if (!newmem)
		return NULL;
	memcpy(newmem, mem, size);
	return newmem;
}

static LT_INLINE
lstr_t lt_strdup(lt_alloc_t alloc[static 1], lstr_t str) {
	return LSTR(lt_memdup(alloc, str.str, str.len), str.len);
}


// heap.c
extern lt_alloc_t* lt_libc_heap;

static LT_INLINE
void* lt_hmalloc(usz size) {
	return malloc(size);
}

static LT_INLINE
void lt_hmfree(const void* ptr) {
	return free((void*)ptr);
}

static LT_INLINE
void* lt_hmrealloc(void* ptr, usz new_size) {
	return realloc(ptr, new_size);
}


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
void lt_amdestroy(const lt_arena_t arena[static 1]);

void* lt_amsave(const lt_arena_t arena[static 1]);
void lt_amrestore(lt_arena_t arena[static 1], void* restore_point);

#include <lt/align.h>

static LT_INLINE
void* lt_amalloc_lean(lt_arena_t arena[static 1], usz size) {
	size = lt_align_fwd(size, LT_ALLOC_DEFAULT_ALIGN);
	u8* start = arena->top;
	u8* new_top = start + size;
	if (new_top > (u8*)arena->base + arena->size) {
		lt_werrf("arena allocation failed, not enough memory\n");
		return NULL;
	}
	arena->top = new_top;
	return start;
}


void* lt_amalloc(lt_arena_t arena[static 1], usz size);
void* lt_amalloc_for_caller(lt_arena_t arena[static 1], usz size, const void* caller);
void lt_amfree(lt_arena_t arena[static 1], const void* ptr);
void* lt_amrealloc(lt_arena_t arena[static 1], void* ptr, usz new_size);
void* lt_amrealloc_for_caller(lt_arena_t arena[static 1], void* ptr, usz new_size, const void* caller);

// void lt_amleak_check(lt_arena_t* arena);


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
void lt_pmdestroy(const lt_pool_t pool[static 1]);

void lt_pmreset(lt_pool_t pool[static 1]);

void* lt_pmalloc(lt_pool_t pool[static 1]);
void lt_pmfree(lt_pool_t pool[static 1], const void* chunk);

#endif
