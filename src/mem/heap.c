#include <lt/mem.h>
#include <lt/debug.h>

#include <malloc.h>

static
void* lt_libc_malloc(void* usr, usz size) {
	if (!size)
		size = 1; // prevent malloc from returning NULL
	return malloc(size);
}

static
void lt_libc_free(void* usr, const void* mem) {
	free((void*)mem);
}

static
void* lt_libc_realloc(void* usr, void* mem, usz size) {
	if (!size)
		size = 1; // prevent realloc from returning NULL
	return realloc(mem, size);
}

static lt_alloc_t lt_libc_heap_ = LT_ALLOC_INTERFACE_INIT(lt_libc_malloc, lt_libc_malloc, lt_libc_free, lt_libc_realloc, lt_libc_realloc);
lt_alloc_t* lt_libc_heap = &lt_libc_heap_;
