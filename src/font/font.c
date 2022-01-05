#include <lt/font.h>
#include <lt/mem.h>

lt_font_t* lt_font_load(lt_arena_t* arena, void* data, usz len) {
	return lt_font_load_psf(arena, data, len);
}

void lt_font_render(lt_font_t* font, lstr_t text, u32* buf) {
	usz w = font->width, h = font->height;
	usz dwords = w * h;

	usz total_w = text.len * w;

	for (usz i = 0; i < text.len; ++i) {
		char c = text.str[i];
		u32* glyph = &font->glyph_data[c * dwords];
		u32* out = &buf[i * w];

		for (usz j = 0; j < h; ++j) {
			memcpy(out, glyph, w * sizeof(u32));
			out += total_w;
			glyph += w;
		}
	}
}

