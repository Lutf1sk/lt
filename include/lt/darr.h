#ifndef LT_DARR_H
#define LT_DARR_H 1

#include <lt/fwd.h>
#include <lt/lt.h>
#include <lt/align.h>
#include <lt/debug.h>

typedef
struct lt_darr {
	usz count;
	usz elem_size;
	usz alloced_size;
	lt_alloc_t* alloc;
} lt_darr_t;

#define LT_DARR_ALIGNED_SIZE lt_word_align_fwd(sizeof(lt_darr_t))

#define lt_darr_head(arr) ((lt_darr_t*)((u8*)arr - LT_DARR_ALIGNED_SIZE))
#define lt_darr_count(arr) (lt_darr_head(arr)->count)

void* lt_darr_create_ LT_DEBUG_ARGS(usz elem_size, usz initial_count, lt_alloc_t* alloc);
#define lt_darr_create(T, initial_count, alloc) ((T*)lt_darr_create_(sizeof(T), (initial_count), (alloc)))

void lt_darr_destroy LT_DEBUG_ARGS(void* arr);
void* lt_darr_make_space LT_DEBUG_ARGS(void* arr, usz count);

#define lt_darr_push(arr, ...) (((arr) = lt_darr_make_space((arr), 1)), ((arr)[lt_darr_count((arr)) - 1] = (__VA_ARGS__)))

static LT_INLINE
void lt_darr_pop(void* arr) {
	lt_darr_head(arr)->count--;
}

void lt_darr_erase LT_DEBUG_ARGS(void* arr, usz start_idx, usz count);

void* lt_darr_insert_ LT_DEBUG_ARGS(void* arr, usz idx, void* data, usz count);
#define lt_darr_insert(arr, idx, data, count) ((arr) = lt_darr_insert_((arr), (idx), (data), (count)))

static LT_INLINE
void lt_darr_clear(void* arr) {
	lt_darr_head(arr)->count = 0;
}

void* lt_darr_dup_ LT_DEBUG_ARGS(void* arr);
#define lt_darr_dup(T, arr) ((T*)lt_darr_dup_(arr))

#define lt_darr_lstr(arr) LSTR(arr, lt_darr_count(arr))

#define lt_darr_create_(...) LT_DEBUG_CALL(lt_darr_create_, __VA_ARGS__)
#define lt_darr_destroy(...) LT_DEBUG_CALL(lt_darr_destroy, __VA_ARGS__)
#define lt_darr_make_space(...) LT_DEBUG_CALL(lt_darr_make_space, __VA_ARGS__)
#define lt_darr_erase(...) LT_DEBUG_CALL(lt_darr_erase, __VA_ARGS__)
#define lt_darr_insert_(...) LT_DEBUG_CALL(lt_darr_insert_, __VA_ARGS__)
#define lt_darr_dup_(...) LT_DEBUG_CALL(lt_darr_dup_, __VA_ARGS__)

#endif
