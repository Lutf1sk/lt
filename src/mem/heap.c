#include <lt/mem.h>

#if defined(LT_UNIX)
#	include <malloc.h>
#	define MSIZE(p) malloc_usable_size(p)
#elif defined(LT_WINDOWS)
#	include <malloc.h>
#	define MSIZE(p) _msize(p)
#endif

static
void* lt_libc_malloc(void* usr, usz size) {
	return malloc(size);
}

static
void lt_libc_free(void* usr, void* mem) {
	free(mem);
}

static
void* lt_libc_realloc(void* usr, void* mem, usz size) {
	return realloc(mem, size);
}

static
usz lt_libc_msize(void* usr, void* mem) {
	return MSIZE(mem);
}

static lt_alloc_t lt_libc_heap_ = LT_ALLOC_INTERFACE(lt_libc_malloc, lt_libc_free, lt_libc_realloc, lt_libc_msize);
lt_alloc_t* lt_libc_heap = &lt_libc_heap_;
