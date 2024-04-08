#ifndef LT_STRSTREAM_H
#define LT_STRSTREAM_H 1

#include <lt/fwd.h>
#include <lt/err.h>
#include <lt/text.h>

typedef
struct lt_strstream {
	lstr_t str;
	usz asize;
	lt_alloc_t* alloc;
} lt_strstream_t;

lt_err_t lt_strstream_create(lt_strstream_t out_ss[static 1], lt_alloc_t alloc[static 1]);
void lt_strstream_destroy(const lt_strstream_t ss[static 1]);

isz lt_strstream_write(lt_strstream_t ss[static 1], const void* data, usz size);

static LT_INLINE
isz lt_strstream_writels(lt_strstream_t ss[static 1], lstr_t str) {
	return lt_strstream_write(ss, str.str, str.len);
}

static LT_INLINE
isz lt_strstream_writec(lt_strstream_t ss[static 1], u32 c) {
	char utf8_buf[4];
	usz len = lt_utf8_encode(c, utf8_buf);
	return lt_strstream_write(ss, utf8_buf, len);
}

static LT_INLINE
void lt_strstream_clear(lt_strstream_t ss[static 1]) {
	ss->str.len = 0;
}

#endif
