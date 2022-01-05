#ifndef LT_FONT_H

#include <lt/primitives.h>
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

#endif
