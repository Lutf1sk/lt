#include <lt/font.h>
#include <lt/mem.h>
#include <lt/utf8.h>

lt_font_t* lt_font_load(lt_arena_t* arena, void* data, usz len) {
	return lt_font_load_psf(arena, data, len);
}



void lt_font_render(lt_font_t* font, lstr_t text, u32* buf) {
	usz w = font->width, h = font->height;
	usz dwords = w * h;

	usz total_w = 0;
	char* txt_it = text.str, *end = text.str + text.len;
	while (txt_it < end) {
		txt_it += lt_utf8_decode_len(*txt_it);
		LT_ASSERT(txt_it <= end);
		total_w += w;
	}

	txt_it = text.str;
	for (usz i = 0; txt_it < end; ++i) {
		u32 c = 0;
		txt_it += lt_utf8_decode(&c, txt_it);

		if (c >= font->glyph_count)
			continue;

		u32* glyph = &font->glyph_data[c * dwords];
		u32* out = &buf[i * w];

		for (usz j = 0; j < h; ++j) {
			memcpy(out, glyph, w * sizeof(u32));
			out += total_w;
			glyph += w;
		}
	}
}

