#ifndef LT_SORT_H
#define LT_SORT_H 1

#include <lt/mem.h>

#define LT_SORT_SWAP(T, a, b)	\
	do {						\
		T tmp = *(a);			\
		*(a) = *(b);			\
		*(b) = tmp;				\
	} while(0)

#define LT_BUBBLE_SORT_BODY(T, is_lesser)			\
	T* end = arr + count;							\
	for (;;) {										\
		b8 sorted = 1;								\
		T last = arr[0];							\
													\
		for (T* it = arr + 1; it < end; ++it) {		\
			T v = *it;								\
			if (is_lesser(v, last)) {				\
				sorted = 0;							\
													\
				*it = last;							\
				*(it - 1) = v;						\
			}										\
			else									\
				last = v;							\
		}											\
													\
		if (sorted)									\
			return;									\
	}

#define LT_DEFINE_BUBBLE_SORT_FUNC(T, name, is_lesser)	\
	void name(T* arr, usz count) {						\
		LT_BUBBLE_SORT_BODY(T, is_lesser)				\
	}

#define LT_MINMAX_SORT_BODY(T, is_lesser)				\
	T* end = arr + count;								\
	for (T* l = arr, *r = end - 1; l < r; ++l, --r) {	\
		T* pmin = l, *pmax = r;							\
		T min = *l, max = *r;							\
														\
		for (T* it = l; it <= r; ++it) {				\
			T v = *it;									\
														\
			if (is_lesser(v, min)) {					\
				pmin = it;								\
				min = v;								\
			}											\
			if (is_lesser(max, v)) {					\
				pmax = it;								\
				max = v;								\
			}											\
		}												\
														\
		LT_SORT_SWAP(T, l, pmin);						\
														\
		if (is_lesser(*pmin, max))						\
			LT_SORT_SWAP(T, r, pmax);					\
		else											\
			LT_SORT_SWAP(T, r, pmin);					\
	}

#define LT_DEFINE_MINMAX_SORT_FUNC(T, name, is_lesser)	\
	void name(T* arr, usz count) {						\
		LT_MINMAX_SORT_BODY(T, is_lesser)				\
	}


#define LT_INSERTION_SORT_BODY(T, is_lesser)						\
	T* end = arr + count;											\
																	\
	for (T* it = arr + 1; it < end; ++it) {							\
		for (;;) {													\
			if (is_lesser(*it, *(it - 1)))							\
				break;												\
			if (++it >= end)										\
				return;												\
		}															\
		T* pinsert = it - 1;										\
		for (; pinsert >= arr; --pinsert) {							\
			if (is_lesser(*pinsert, *it))							\
				break;												\
		}															\
		++pinsert;													\
																	\
		T v = *it;													\
		memmove(pinsert + 1, pinsert, ((usz)it - (usz)pinsert));	\
		*pinsert = v;												\
	}

#define LT_DEFINE_INSERTION_SORT_FUNC(T, name, is_lesser)	\
	void name(T* arr, usz count) {							\
		LT_INSERTION_SORT_BODY(T, is_lesser)				\
	}

#define LT_QUICKSORT_BODY(T, is_lesser)				\
	if (count <= 1)									\
		return;										\
													\
	T* stack[8 * sizeof(size_t)];					\
	T** sp = stack;									\
													\
	*sp++ = NULL;									\
	*sp++ = NULL;									\
													\
	T* first = arr;									\
	T* last = arr + count - 1;						\
													\
	while (stack < sp) {							\
		T* mid = first + ((last - first) >> 1);		\
													\
		if (is_lesser(*mid, *first))				\
			LT_SORT_SWAP(T, mid, first);			\
		if (is_lesser(*last, *mid)) {				\
			LT_SORT_SWAP(T, mid, last);				\
			if (is_lesser(*mid, *first))			\
				LT_SORT_SWAP(T, mid, first);		\
		}											\
													\
		T* left = first + 1, *right = last - 1;		\
		do {										\
			while (is_lesser(*left, *mid))			\
				++left;								\
													\
			while (is_lesser(*mid, *right))			\
				--right;							\
													\
			if (left < right) {						\
				LT_SORT_SWAP(T, left, right);		\
													\
				if (mid == left)					\
					mid = right;					\
				else if (mid == right)				\
					mid = left;						\
													\
				++left, --right;					\
			}										\
			else if (left == right) {				\
				++left, --right;					\
				break;								\
			}										\
													\
		} while (left < right);						\
													\
		usz threshold = 4;							\
		usz locount = right - first;				\
		usz hicount = last - left;					\
													\
		if (locount <= threshold) {					\
			if (hicount <= threshold) {				\
				last = *--sp;						\
				first = *--sp;						\
			}										\
			else									\
				first = left;						\
		}											\
		else if (hicount <= threshold)				\
			last = left;							\
		else if (locount > hicount) {				\
			*sp++ = first;							\
			*sp++ = right;							\
			first = left;							\
		}											\
		else {										\
			*sp++ = left;							\
			*sp++ = last;							\
			last = right;							\
		}											\
	}												\
	{												\
		LT_INSERTION_SORT_BODY(T, is_lesser)		\
	}

#define LT_DEFINE_QUICKSORT_FUNC(T, name, is_lesser)	\
	void name(T* arr, usz count) {						\
		LT_QUICKSORT_BODY(T, is_lesser)					\
	}

#define LT_MERGE_SORT_BODY(T, is_lesser)												\
	T* arr1 = lt_malloc(alloc, count * sizeof(T));										\
	T* arr2 = lt_malloc(alloc, count * sizeof(T));										\
	if (!arr1 || !arr2)																	\
		return NULL;																	\
																						\
	usz halfstep = count & 1;															\
	for (T* it = arr, *out = arr1, *end = arr + count - halfstep; it < end; it += 2) {	\
		T v0 = it[0], v1 = it[1];														\
		if (is_lesser(v0, v1)) {														\
			*out++ = v0;																\
			*out++ = v1;																\
		}																				\
		else {																			\
			*out++ = v1;																\
			*out++ = v0;																\
		}																				\
	}																					\
	if (halfstep)																		\
		arr1[count - 1] = arr[count - 1];												\
																						\
	T* rarr = arr1;																		\
	T* warr = arr2;																		\
																						\
	usz blocksz = 4;																	\
	usz arrsz = 2;																		\
																						\
	while (arrsz < count) {																\
		for (T* block = rarr, *end = rarr + count; block < end; block += blocksz) {		\
			T* it1 = block, *it2 = block + arrsz;										\
			T* end1 = it2, *end2 = it2 + arrsz;											\
																						\
			if (end1 > end) {															\
				usz len = end - block;													\
				memcpy(warr, block, len * sizeof(T));									\
				break;																	\
			}																			\
																						\
			if (end2 > end)																\
				end2 = end;																\
																						\
			while (it1 < end1 && it2 < end2) {											\
				T v1 = *it1, v2 = *it2;													\
																						\
				if (is_lesser(v1, v2)) {												\
					*warr++ = v1;														\
					++it1;																\
				}																		\
				else {																	\
					*warr++ = v2;														\
					++it2;																\
				}																		\
			}																			\
																						\
			usz len1 = end1 - it1;														\
			memcpy(warr, it1, len1 * sizeof(T));										\
			warr += len1;																\
																						\
			usz len2 = end2 - it2;														\
			memcpy(warr, it2, len2 * sizeof(T));										\
			warr += len2;																\
		}																				\
																						\
		if (rarr == arr1) {																\
			rarr = arr2;																\
			warr = arr1;																\
		}																				\
		else {																			\
			rarr = arr1;																\
			warr = arr2;																\
		}																				\
																						\
		arrsz <<= 1;																	\
		blocksz <<= 1;																	\
	}																					\
																						\
	lt_mfree(alloc, warr);																\
	return rarr;

#define LT_DEFINE_MERGE_SORT_FUNC(T, name, is_lesser)		\
	T* merge_sort(T* arr, usz count, lt_alloc_t* alloc) {	\
		LT_MERGE_SORT_BODY(T, is_lesser);					\
	}

#define LT_DEFINE_BINARY_SEARCH_FUNC(T, Tkey, name, is_lesser, is_equal)	\
	T* name(T* arr, Tkey key, usz count) {									\
		if (!count)															\
			return NULL;													\
																			\
		T* start = arr;														\
		T* end = arr + count;												\
																			\
		while (end - start > 0) {											\
			T* mid = start + ((end - start) >> 1);							\
			if (is_equal(*mid, key))										\
				return mid;													\
			if (is_lesser(*mid, key))										\
				start = mid + 1;											\
			else															\
				end = mid;													\
		}																	\
																			\
		return NULL;														\
	}

#define LT_DEFINE_BINARY_SEARCH_NEAREST_FUNC(T, Tkey, name, is_lesser, is_equal)	\
	T* name(T* arr, Tkey val, usz count) {											\
		T* start = arr;																\
		T* end = arr + count;														\
																					\
		while (end - start > 0) {													\
			T* mid = start + ((end - start) >> 1);									\
			if (is_equal(*mid, val))												\
				return mid;															\
			if (is_lesser(*mid, val))												\
				start = mid + 1;													\
			else																	\
				end = mid;															\
		}																			\
																					\
		return start;																\
	}

#endif