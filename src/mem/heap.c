#include <lt/mem.h>
#include <lt/debug.h>

#include <malloc.h>

static
void* lt_libc_malloc(void* usr, usz size) {
	if (!size) {
		lt_werrf("libc malloc called with size 0\n");
		size = 1;
	}
	return malloc(size);
}

static
void lt_libc_free(void* usr, void* mem) {
	free(mem);
}

static
void* lt_libc_realloc(void* usr, void* mem, usz size) {
	if (!size) {
		lt_werrf("libc realloc called with size 0\n");
		size = 1;
	}
	return realloc(mem, size);
}

static lt_alloc_t lt_libc_heap_ = LT_ALLOC_INTERFACE_INIT(lt_libc_malloc, lt_libc_malloc, lt_libc_free, lt_libc_realloc, lt_libc_realloc);
lt_alloc_t* lt_libc_heap = &lt_libc_heap_;
