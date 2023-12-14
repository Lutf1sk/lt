#include <lt/font.h>
#include <lt/mem.h>
#include <lt/text.h>

lt_err_t lt_font_load(lt_font_t* font, void* data, usz len, lt_alloc_t* alloc) {
	return lt_font_load_psf(font, data, len, alloc);
}

void lt_font_destroy(lt_font_t* font, lt_alloc_t* alloc) {
	lt_mfree(alloc, font->glyph_data);
}

void lt_font_render(lt_font_t* font, lstr_t text, u32* buf) {
	usz w = font->width, h = font->height;
	usz dwords = w * h;

	usz total_w = lt_font_text_width(font, text);

	char* txt_it = text.str, *end = text.str + text.len;
	for (usz i = 0; txt_it < end; ++i) {
		u32 c = 0;
		txt_it += lt_utf8_decode(txt_it, &c);

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

usz lt_font_text_width(lt_font_t* font, lstr_t text) {
	return lt_utf8_glyph_count(text) * font->width;
}

