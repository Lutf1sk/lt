#include <lt/darr.h>
#include <lt/mem.h>

#include <stdarg.h>

void* lt_darr_create_(usz elem_size, usz initial_count, lt_alloc_t* alloc) {
	LT_ASSERT(elem_size && initial_count);

	usz alloced_size = initial_count * elem_size;
	lt_darr_t* head = lt_malloc(alloc, LT_DARR_ALIGNED_SIZE + alloced_size);
	if (!head)
		return NULL;
	head->alloced_size = alloced_size;
	head->elem_size = elem_size;
	head->count = 0;
	head->alloc = alloc;
	return (u8*)head + LT_DARR_ALIGNED_SIZE;
}

void lt_darr_destroy(void* arr) {
	lt_darr_t* head = lt_darr_head(arr);
	LT_ASSERT(arr && head);
	lt_mfree(head->alloc, head);
}

void* lt_darr_make_space(void* arr, usz count) {
	lt_darr_t* head = lt_darr_head(arr);
	LT_ASSERT(arr && head);

	head->count += count;
	usz new_size = head->count * head->elem_size;
	if (new_size > head->alloced_size) {
		while (new_size > head->alloced_size)
			head->alloced_size <<= 1;
		head = lt_mrealloc(head->alloc, head, LT_DARR_ALIGNED_SIZE + new_size);
		arr = (u8*)head + LT_DARR_ALIGNED_SIZE;
		LT_ASSERT(arr);
	}
	return arr;
}

