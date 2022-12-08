#include <lt/img.h>
#include <lt/lt.h>
#include <lt/bits.h>
#include <lt/mem.h>

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

lt_err_t lt_img_load_tga(lt_img_t* img, void* data, usz len, lt_alloc_t* alloc) {
	lt_tga_header_t* h = data;

	if (len < sizeof(lt_tga_header_t) || h->magic != LT_IMG_TGA_MAGIC[0] || h->clrmap != 0 || h->encoding != 2)
		return LT_ERR_INVALID_FORMAT;

	if (h->bpp != 32 || h->pixel_type != 40 || h->x != 0)
		return LT_ERR_INVALID_FORMAT;

	usz px_count = h->w * h->h;

	u32* bgra_data = (u32*)((usz)data + sizeof(lt_tga_header_t));
	u32* rgba_data = lt_malloc(alloc, px_count * sizeof(u32));
	if (!rgba_data)
		return LT_ERR_OUT_OF_MEMORY;

	for (usz i = 0; i < px_count; ++i) {
		u32 v = bgra_data[i];
		rgba_data[i] =
				((v & 0x00FF0000) >> 16) |
				((v & 0x0000FF00)) |
				((v & 0x000000FF) << 16) |
				((v & 0xFF000000));
	}

	img->width = h->w;
	img->height = h->h;
	img->data = rgba_data;
	img->format = LT_IMG_RGBA;

	return LT_SUCCESS;
}

