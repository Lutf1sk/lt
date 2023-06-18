#include <lt/mem.h>

#if defined(LT_UNIX)
#	include <malloc.h>
#	define MSIZE(p) malloc_usable_size(p)
#elif defined(LT_WINDOWS)
#	include <malloc.h>
#	define MSIZE(p) _msize(p)
#endif

static
void* lt_libc_malloc LT_DEBUG_ARGS(void* usr, usz size) {
	if (!size) {
		LT_DEBUG_WERR("libc malloc called with size 0\n");
		size = 1;
	}
	return malloc(size);
}

static
void lt_libc_free LT_DEBUG_ARGS(void* usr, void* mem) {
	free(mem);
}

static
void* lt_libc_realloc LT_DEBUG_ARGS(void* usr, void* mem, usz size) {
	if (!size) {
		LT_DEBUG_WERR("libc realloc called with size 0\n");
		size = 1;
	}
	return realloc(mem, size);
}

static
usz lt_libc_msize LT_DEBUG_ARGS(void* usr, void* mem) {
	return MSIZE(mem);
}

static lt_alloc_t lt_libc_heap_ = LT_ALLOC_INTERFACE_INIT(lt_libc_malloc, lt_libc_free, lt_libc_realloc, lt_libc_msize);
lt_alloc_t* lt_libc_heap = &lt_libc_heap_;
