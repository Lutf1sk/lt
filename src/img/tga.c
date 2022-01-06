#include <lt/img.h>
#include <lt/lt.h>

typedef
LT_PACKED_STRUCT(lt_tga_header) {
	u8 magic;
	u8 clrmap;
	u8 encoding;
	u16 cmaporig, cmaplen;
	u8 cmapent;
	u16 x, y, h, w;
	u8 bpp;
	u8 pixel_type;
} lt_tga_header_t;

b8 lt_img_load_tga(lt_arena_t* arena, void* data, usz len, lt_img_t* img) {
	lt_tga_header_t* h = data;

	if (len < sizeof(lt_tga_header_t) || h->magic != LT_IMG_TGA_MAGIC || h->clrmap != 0 || h->encoding != 2)
		return 0;

	if (h->bpp != 32 || h->pixel_type != 40 || h->x != 0)
		return 0;

	img->width = h->w;
	img->height = h->h;
	img->data = (u8*)data + sizeof(lt_tga_header_t);
	img->format = LT_IMG_ARGB;

	return 1;
}

