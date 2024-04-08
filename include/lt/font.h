#ifndef LT_FONT_H
#define LT_FONT_H 1

#include <lt/err.h>
#include <lt/fwd.h>

#define LT_FONT_PSF1_MAGIC 0x0436
#define LT_FONT_PSF2_MAGIC 0x864ab572

typedef
struct lt_font {
	u16 width, height;
	u32 glyph_count;

	u32* glyph_data;
} lt_font_t;

lt_err_t lt_font_load_psf(lt_font_t out_font[static 1], void* data, usz len, lt_alloc_t alloc[static 1]);
lt_err_t lt_font_write_psf(const lt_font_t out_font[static 1], lt_file_t* file, lt_alloc_t alloc[static 1]); // !! TODO: convert to write stream

lt_err_t lt_font_load(lt_font_t out_font[static 1], void* data, usz len, lt_alloc_t alloc[static 1]);

void lt_font_destroy(const lt_font_t font[static 1], lt_alloc_t alloc[static 1]);

void lt_font_render(const lt_font_t font[static 1], lstr_t text, u32* buf);

usz lt_font_text_width(const lt_font_t font[static 1], lstr_t text);

#endif
