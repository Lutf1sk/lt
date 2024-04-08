#include <lt/lt.h>
#include <lt/img.h>

typedef
LT_PACKED_STRUCT(lt_bmp_fileheader) {
	char magic[2];
	u32 file_size;
	u16 reserved[2];
	u32 px_data_offs;
} lt_bmp_fileheader_t;

typedef
LT_PACKED_STRUCT(lt_bmp_coreheader1) {
	u32 header_size;
	u16 width, height;
	u16 clr_planes, bpp;
} lt_bmp_coreheader1_t;

typedef
LT_PACKED_STRUCT(lt_bmp_coreheader2) {
	u32 header_size;
	i32 width, height;
	u16 clr_planes, bpp;
} lt_bmp_coreheader2_t;

#define COMPRESS_NONE 0

typedef
LT_PACKED_STRUCT(lt_bmp_infoheader1) {
	u32 compression_type;
	u32 img_bytes;
	i32 h_res, v_res;
	u32 palette_clrs;
	u32 important_palette_clrs;
} lt_bmp_infoheader1_t;

#define HALFTONE_NONE 0

#define CLRENC_RGB 0

typedef
LT_PACKED_STRUCT(lt_bmp_infoheader2) {
	u16 res_unit;
	u16 pad;
	u16 fill_dir;
	u16 halftone_type;
	u32 halftone_params[2];
	u32 clr_enc;
	u32 identifier;
} lt_bmp_infoheader2_t;

lt_err_t lt_img_load_bmp(lt_img_t img[static 1], const void* data, usz len, lt_alloc_t alloc[static 1]) {
	usz end = (usz)data + len;

	const lt_bmp_fileheader_t* fh = data;
	if (len < sizeof(lt_bmp_fileheader_t) || fh->magic[0] != 'B' || fh->magic[1] != 'M')
		return LT_ERR_INVALID_FORMAT;

	u32* infoheader_start = (u32*)((usz)fh + sizeof(lt_bmp_fileheader_t));
	if ((usz)infoheader_start + sizeof(u32) > end || (usz)infoheader_start + *infoheader_start > end)
		return LT_ERR_INVALID_FORMAT;

	switch (*(u32*)infoheader_start) {
	// TODO: https://en.wikipedia.org/wiki/BMP_file_format

	default:
		return LT_ERR_INVALID_FORMAT;
	}

	return LT_SUCCESS;
}

