#ifndef LT_DARR_H
#define LT_DARR_H 1

#include <lt/fwd.h>
#include <lt/lt.h>
#include <lt/align.h>

typedef
struct lt_darr {
	usz count;
	usz elem_size;
	usz alloced_size;
	lt_alloc_t* alloc;
} lt_darr_t;

#define LT_DARR_ALIGNED_SIZE lt_word_align_fwd(sizeof(lt_darr_t))

static LT_INLINE
lt_darr_t* lt_darr_head(void* arr) {
	return (lt_darr_t*)((usz)arr - LT_DARR_ALIGNED_SIZE);
}

static LT_INLINE
usz lt_darr_count(void* arr) {
	return lt_darr_head(arr)->count;
}

void* lt_darr_create_(usz elem_size, usz initial_count, lt_alloc_t alloc[static 1]);
#define lt_darr_create(T, initial_count, alloc) ((T*)lt_darr_create_(sizeof(T), (initial_count), (alloc)))

void lt_darr_destroy(void* arr);
void* lt_darr_make_space(void* arr, usz count);

#define lt_darr_push(arr, ...) (((arr) = lt_darr_make_space((arr), 1)), ((arr)[lt_darr_count((arr)) - 1] = (__VA_ARGS__)))

static LT_INLINE
void lt_darr_pop(void* arr) {
	lt_darr_head(arr)->count--;
}

void lt_darr_erase(void* arr, usz start_idx, usz count);

void* lt_darr_insert_(void* arr, usz idx, void* data, usz count);
#define lt_darr_insert(arr, idx, data, count) ((arr) = lt_darr_insert_((arr), (idx), (data), (count)))

static LT_INLINE
void lt_darr_clear(void* arr) {
	lt_darr_head(arr)->count = 0;
}

void* lt_darr_dup_(void* arr);
#define lt_darr_dup(arr) ((typeof(arr))lt_darr_dup_(arr))

#define lt_darr_lstr(arr) LSTR(arr, lt_darr_count(arr))

#endif
