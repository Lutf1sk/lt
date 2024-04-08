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
	u8 clrmap_bpp;

	// Image spec
	u16 x, y, w, h;
	u8 bpp;
	u8 format;
} lt_tga_header_t;

static LT_INLINE
u32 clr_load32(u8* ptr) {
	return	ptr[2] |
			((u32)ptr[1] << 8) |
			((u32)ptr[0] << 16) |
			((u32)ptr[3] << 24);
}

static LT_INLINE
u32 clr_load24(u8* ptr) {
	return ((u32)0xFF << 24) | ((u32)ptr[0] << 16) | ((u32)ptr[1] << 8) | (u32)ptr[2];
}

lt_err_t lt_img_load_tga(lt_img_t img[static 1], const void* data, usz len, lt_alloc_t alloc[static 1]) {
	lt_err_t err;

	const lt_tga_header_t* h = data;

	if (len < sizeof(lt_tga_header_t))
		return LT_ERR_INVALID_FORMAT;

// 	u8 attrib_bits = h->format & 0b1111;
	u8 origin = (h->format >> 4) & 0b11;
	u8 interleaving = (h->format >> 6) & 0b11;

	if (origin != 0b10 || interleaving != 0)
		return LT_ERR_UNSUPPORTED;

	usz ident_offs = sizeof(lt_tga_header_t);
	usz clrmap_offs = ident_offs + h->ident_length;
	usz image_offs = clrmap_offs;
	if (h->clrmap_type)
		image_offs += h->clrmap_length * h->clrmap_bpp / 8;

	u8* clrmap_data = (u8*)data + clrmap_offs;
	u8* image_data = (u8*)data + image_offs;

	usz px_count = h->w * h->h;

	if (h->encoding & 0b11110100)
		return LT_ERR_INVALID_FORMAT;

	u8 rle_compressed = h->encoding & 0b1000;
	u8 image_type = h->encoding & 0b0011;

	usz bytes_per_px = h->bpp / 8;

	// ----- RLE compression
	u8* uncompressed_image_data = NULL;
	if (rle_compressed) {
		usz image_size = px_count * bytes_per_px;

		uncompressed_image_data = lt_malloc(alloc, image_size);
		if (!uncompressed_image_data)
			return LT_ERR_OUT_OF_MEMORY;

		u8* it = image_data;
		u8* out = uncompressed_image_data, *end = out + image_size;

		if (bytes_per_px == 4) {
			while (out < end) {
				u8 packet = *it++;
				u8 length = (packet & 0x7F) + 1;
				usz size = length << 2;

				if (out + size > end)
					fail_to(err = LT_ERR_INVALID_FORMAT, err0);

				if (packet & 0x80) {
					u32 val = *(u32*)it;
					it += 4;

					lt_mset32(out, val, size);
					out += size;
				}
				else {
					memcpy(out, it, size);
					out += size;
					it += size;
				}
			}
		}
		else if (bytes_per_px == 3) {
			while (out < end) {
				u8 packet = *it++;
				u8 length = (packet & 0x7F) + 1;
				usz size = length * 3;

				if (out + size > end)
					fail_to(err = LT_ERR_INVALID_FORMAT, err0);

				if (packet & 0x80) {
					u8 val[3];
					memcpy(val, it, 3);
					it += 3;

					for (usz i = 0; i < length; ++i, out += 3)
						memcpy(out, val, 3);
				}
				else {
					memcpy(out, it, size);
					out += size;
					it += size;
				}
			}
		}
		else if (bytes_per_px == 1) {
			while (out < end) {
				u8 packet = *it++;
				u8 length = (packet & 0x7F) + 1;

				if (out + length > end)
					fail_to(err = LT_ERR_INVALID_FORMAT, err0);

				if (packet & 0x80) {
					u8 val = *it++;
					memset(out, val, length);
					out += length;
				}
				else {
					memcpy(out, it, length);
					out += length;
					it += length;
				}
			}
		}
		else
			fail_to(err = LT_ERR_UNSUPPORTED, err0);

		image_data = uncompressed_image_data;
	}

	u32* rgba_data = lt_malloc(alloc, px_count * sizeof(u32));
	if (!rgba_data)
		fail_to(err = LT_ERR_OUT_OF_MEMORY, err0);

	// ----- color mapping
	if (image_type == 0b01) {
		u8* base = clrmap_data + h->clrmap_origin;
		if (h->clrmap_bpp == 32) {
			for (usz i = 0; i < px_count; ++i)
				rgba_data[i] = clr_load32(base + image_data[i] * 4);
		}
		else if (h->clrmap_bpp == 24) {
			for (usz i = 0; i < px_count; ++i)
				rgba_data[i] = clr_load24(base + image_data[i] * 3);
		}
		else
			fail_to(err = LT_ERR_UNSUPPORTED, err1);
	}

	// ----- truecolor
	else if (image_type == 0b10) {
		if (bytes_per_px == 4) {
			for (usz i = 0; i < px_count; ++i)
				rgba_data[i] = clr_load32(image_data + i * 4);
		}
		else if (bytes_per_px == 3) {
			for (usz i = 0; i < px_count; ++i)
				rgba_data[i] = clr_load24(image_data + i * 3);
		}
		else
			fail_to(err = LT_ERR_UNSUPPORTED, err1);
	}

	// ----- monochrome
	else if (image_type == 0b11)
		fail_to(err = LT_ERR_UNSUPPORTED, err0);

	else
		fail_to(err = LT_ERR_UNSUPPORTED, err0);

	img->width = h->w;
	img->height = h->h;
	img->data = rgba_data;
	img->format = LT_IMG_RGBA;
	if (rle_compressed)
		lt_mfree(alloc, uncompressed_image_data);
	return LT_SUCCESS;

err1:	lt_mfree(alloc, rgba_data);
err0:	if (rle_compressed)
			lt_mfree(alloc, uncompressed_image_data);
		return err;
}

