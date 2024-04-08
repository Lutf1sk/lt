#ifndef LT_IMG
#define LT_IMG 1

#include <lt/err.h>
#include <lt/fwd.h>

typedef
enum lt_img_format {
	LT_IMG_RGBA,
} lt_img_format_t;

typedef
struct lt_img {
	u32 width, height;
	u32 flags, format;
	void* data;
} lt_img_t;

lt_err_t lt_img_load_tga(lt_img_t out_img[static 1], const void* data, usz len, lt_alloc_t alloc[static 1]);
lt_err_t lt_img_load_bmp(lt_img_t out_img[static 1], const void* data, usz len, lt_alloc_t alloc[static 1]);
lt_err_t lt_img_load(lt_img_t out_img[static 1], const void* data, usz len, lt_alloc_t alloc[static 1]);

void lt_img_free(const lt_img_t img[static 1], lt_alloc_t alloc[static 1]);

#endif
