#ifndef LT_HTML_H
#define LT_HTML_H 1

#include <lt/fwd.h>
#include <lt/lt.h>

extern b8 lt_html_reserved_char_tab[LT_U8_MAX];

static LT_INLINE
b8 lt_html_is_reserved_char(u32 c) {
	return c < LT_U8_MAX && !lt_html_reserved_char_tab[c];
}

lstr_t lt_htmlencode(lstr_t str, lt_alloc_t alloc[static 1]);

isz lt_write_htmlencoded(lt_write_fn_t callb, void* usr, lstr_t str);
isz lt_write_htmlencoded_char8(lt_write_fn_t callb, void* usr, u8 c);

#endif
