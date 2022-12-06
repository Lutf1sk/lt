#include <lt/strstream.h>
#include <lt/mem.h>

#define INITIAL_BUFSIZE 32

b8 lt_strstream_create(lt_strstream_t* s, lt_alloc_t* alc) {
	s->str.str = lt_malloc(alc, INITIAL_BUFSIZE);
	s->str.len = 0;
	s->asize = INITIAL_BUFSIZE;
	s->alloc = alc;
	return !!s->str.str;
}

void lt_strstream_destroy(lt_strstream_t* s) {
	if (s->str.str)
		lt_mfree(s->alloc, s->str.str);
}

isz lt_strstream_write(lt_strstream_t* s, void* data, usz size) {
	if (!size)
		return size;

	if (s->str.len + size > s->asize) {
		while (s->str.len + size > s->asize)
			s->asize <<= 1;

		s->str.str = lt_mrealloc(s->alloc, s->str.str, s->asize); // !!
		if (!s->str.str)
			return -1;
	}

	memcpy(s->str.str + s->str.len, data, size);
	s->str.len += size;
	return size;
}

