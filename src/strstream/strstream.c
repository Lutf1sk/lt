#include <lt/strstream.h>
#include <lt/mem.h>

#define INITIAL_BUFSIZE 32

lt_err_t lt_strstream_create(lt_strstream_t ss[static 1], lt_alloc_t alloc[static 1]) {
	ss->str.str = lt_malloc(alloc, INITIAL_BUFSIZE);
	ss->str.len = 0;
	ss->asize = INITIAL_BUFSIZE;
	ss->alloc = alloc;
	if (!ss->str.str)
		return LT_ERR_OUT_OF_MEMORY;
	return LT_SUCCESS;
}

void lt_strstream_destroy(const lt_strstream_t ss[static 1]) {
	if (ss->str.str)
		lt_mfree(ss->alloc, ss->str.str);
}

isz lt_strstream_write(lt_strstream_t ss[static 1], const void* data, usz size) {
	if (!size)
		return size;

	if (ss->str.len + size > ss->asize) {
		while (ss->str.len + size > ss->asize)
			ss->asize <<= 1;

		void* res = lt_mrealloc(ss->alloc, ss->str.str, ss->asize);
		if (!res)
			return -LT_ERR_OUT_OF_MEMORY;
		ss->str.str = res;
	}

	memcpy(ss->str.str + ss->str.len, data, size);
	ss->str.len += size;
	return size;
}

