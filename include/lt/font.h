#ifndef LT_FONT_H
#define LT_FONT_H 1

#include <lt/lt.h>
#include <lt/fwd.h>

#define LT_FONT_PSF_MAGIC 0x864ab572

typedef
struct lt_font {
	u16 width, height;
	u32 glyph_count;

	u32* glyph_data;
} lt_font_t;

lt_font_t* lt_font_load_psf(lt_arena_t* arena, void* data, usz len);
lt_font_t* lt_font_load(lt_arena_t* arena, void* data, usz len);

void lt_font_render(lt_font_t* font, lstr_t text, u32* buf);

#endif
