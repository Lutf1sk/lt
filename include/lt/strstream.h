#ifndef LT_STRSTREAM_H
#define LT_STRSTREAM_H 1

#include <lt/fwd.h>
#include <lt/lt.h>

typedef
struct lt_strstream {
	lstr_t str;
	usz asize;
	lt_alloc_t* alloc;
} lt_strstream_t;

b8 lt_strstream_create(lt_strstream_t* s, lt_alloc_t* alc);
void lt_strstream_destroy(lt_strstream_t* s, lt_alloc_t* alc);

b8 lt_strstream_write(lt_strstream_t* s, void* data, usz size);

LT_INLINE
b8 lt_strstream_writels(lt_strstream_t* s, lstr_t str) {
	return lt_strstream_write(s, str.str, str.len);
}

LT_INLINE
b8 lt_strstream_writec(lt_strstream_t* s, char c) {
	return lt_strstream_write(s, &c, 1);
}

#endif
