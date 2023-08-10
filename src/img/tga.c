#include <lt/img.h>
#include <lt/lt.h>
#include <lt/math.h>
#include <lt/mem.h>
#include <lt/internal.h>

typedef
LT_PACKED_STRUCT(lt_tga_header) {
	u8 ident_length, clrmap_type, encoding;

	// Colormap spec
	u16 clrmap_origin, clrmap_length;
	u8 clrmap_entry_size;

	// Image spec
	u16 x, y, w, h;
	u8 bpp;
	u8 format;
} lt_tga_header_t;

static LT_INLINE
u32 clr_load32(u8* ptr) {
	u32 clr = *(u32*)ptr;
	return	((clr & 0x00FF0000) >> 16) |
			((clr & 0x0000FF00)) |
			((clr & 0x000000FF) << 16) |
			((clr & 0xFF000000));
}

static LT_INLINE
u32 clr_load24(u8* ptr) {
	return ((u32)0xFF << 24) | ((u32)ptr[0] << 16) | ((u32)ptr[1] << 8) | (u32)ptr[2];
}

lt_err_t lt_img_load_tga(lt_img_t* img, void* data, usz len, lt_alloc_t* alloc) {
	lt_err_t err;

	lt_tga_header_t* h = data;

	if (len < sizeof(lt_tga_header_t))
		return LT_ERR_INVALID_FORMAT;

	if (h->clrmap_type != 0)
		return LT_ERR_UNSUPPORTED;

	u8 attrib_bits = h->format & 0b1111;
	u8 origin = (h->format >> 5) & 0b1;
	u8 interleaving = (h->format >> 6) & 0b11;

	if (origin != 1 || interleaving != 0)
		return LT_ERR_UNSUPPORTED;

	switch (h->bpp) {
	case 32:
		if (attrib_bits != 8)
			return LT_ERR_INVALID_FORMAT;
		if (h->clrmap_type && h->clrmap_entry_size != 32)
			return LT_ERR_INVALID_FORMAT;
		break;
	case 24:
		if (attrib_bits != 0)
			return LT_ERR_INVALID_FORMAT;
		if (h->clrmap_type && h->clrmap_entry_size != 24)
			return LT_ERR_INVALID_FORMAT;
		break;
	default:
		return LT_ERR_UNSUPPORTED;
	}

	usz ident_offs = sizeof(lt_tga_header_t);
	usz clrmap_offs = ident_offs + h->ident_length;
	usz image_offs = clrmap_offs;
	if (h->clrmap_type)
		image_offs += h->clrmap_length * h->clrmap_entry_size;

// 	u8* clrmap_data = (u8*)data + clrmap_offs;
	u8* image_data = (u8*)data + image_offs;

	usz px_count = h->w * h->h;
	u32* rgba_data = lt_malloc(alloc, px_count * sizeof(u32));
	if (!rgba_data)
		return LT_ERR_OUT_OF_MEMORY;

	switch (h->encoding) {
	case 2: // Uncompressed, RGB
		if (h->bpp == 32) {
			for (usz i = 0; i < px_count; ++i)
				rgba_data[i] = clr_load32(image_data + i * 4);
		}
		else if (h->bpp == 24) {
			for (usz i = 0; i < px_count; ++i)
				rgba_data[i] = clr_load24(image_data + i * 3);
		}
		break;

	case 10: { // Runlength encoded, RGB
		u8* it = image_data;
		u32* out = rgba_data, *end = out + px_count;

		if (h->bpp == 32) {
			while (out < end) {
				u8 packet = *it++;
				usz count = (packet & 0x7F) + 1;

				if (out + count > end)
					fail_to(err = LT_ERR_INVALID_FORMAT, err0);

				if (packet & 0x80) {
					u32 clr = clr_load32(it);
					it += 4;
					for (usz i = 0; i < count; ++i)
						*out++ = clr;
				}
				else {
					for (usz i = 0; i < count; ++i) {
						*out++ = clr_load32(it);
						it += 4;
					}
				}
			}
		}
		else if (h->bpp == 24) {
			while (out < end) {
				u8 packet = *it++;
				usz count = (packet & 0x7F) + 1;

				if (out + count > end)
					fail_to(err = LT_ERR_INVALID_FORMAT, err0);

				if (packet & 0x80) {
					u32 clr = clr_load24(it);
					it += 3;
					for (usz i = 0; i < count; ++i)
						*out++ = clr;
				}
				else {
					for (usz i = 0; i < count; ++i) {
						*out++ = clr_load24(it);
						it += 3;
					}
				}
			}
		}
	}	break;

	case 1: // Uncompressed, color-mapped
	case 3: // Uncompressed, black and white
	case 9: // Runlength encoded, color-mapped
	case 11: // Compressed, black and white
	case 32: // Compressed, color-mapped
	case 33: // Compressed, color-mapped, 4-pass quadtree-type process
		return LT_ERR_UNSUPPORTED;

	case 0: // No data included
	default:
		return LT_ERR_INVALID_FORMAT;
	}

	img->width = h->w;
	img->height = h->h;
	img->data = rgba_data;
	img->format = LT_IMG_RGBA;
	return LT_SUCCESS;

err0:	lt_mfree(alloc, rgba_data);
		return err;
}

