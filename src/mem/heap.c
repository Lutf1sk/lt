#include <lt/mem.h>

static
void* reserve(void* usr, usz size) {
	return malloc(size);
}

static
void relinq(void* usr, void* mem) {
	free(mem);
}

static
void* resize(void* usr, void* mem, usz size) {
	return realloc(mem, size);
}

lt_alloc_t lt_heap_interface(void) {
	return LT_ALLOC_INTERFACE(NULL, reserve, relinq, resize);
}

