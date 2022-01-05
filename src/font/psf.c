#include <lt/font.h>
#include <lt/mem.h>

typedef
struct lt_psf_header {
	u32 magic;
	u32 version;
	u32 header_size;
	u32 flags;
	u32 glyph_count;
	u32 glyph_bytes;
	u32 height;
	u32 width;
} lt_psf_header_t;

lt_font_t* lt_font_load_psf(lt_arena_t* arena, void* data, usz len) {
	lt_psf_header_t* h = data;

	if (len < sizeof(lt_psf_header_t) || h->magic != LT_FONT_PSF_MAGIC || h->version != 0)
		return NULL;

	usz glyphtab_size = h->glyph_count * h->glyph_bytes;

	if (len < h->header_size + glyphtab_size || h->width == 0 || h->height == 0)
		return NULL;

	u32 w = h->width;

	lt_font_t* font = lt_arena_reserve(arena, sizeof(lt_font_t));
	font->width = w;
	font->height = h->height;
	font->glyph_count = h->glyph_count;
	font->glyph_data = lt_arena_reserve(arena, glyphtab_size * sizeof(u32));

	u8* it = (u8*)data + h->header_size;
	for (u32* out = font->glyph_data, i; i < h->glyph_count; ++i) {
		for (usz j = 0; j < h->height; ++j) {
			u8 byte;
			for (u8 bit_i = 0; bit_i < w; ++bit_i) {
				if ((bit_i & 0b111) == 0)
					byte = *it++;

				if (byte & 1)
					*out++ = 0xFFFFFFFF;
				else
					*out++ = 0x00000000;
				byte >>= 1;
			}
		}
	}

	return font;
}
