#include <lt/img.h>
#include <lt/mem.h>

lt_err_t lt_img_load(lt_img_t* img, void* data, usz len, lt_alloc_t* alloc) {
	if (lt_img_load_tga(img, data, len, alloc) == LT_SUCCESS)
		return LT_SUCCESS;
	if (lt_img_load_bmp(img, data, len, alloc) == LT_SUCCESS)
		return LT_SUCCESS;
	return LT_ERR_UNSUPPORTED;
}

void lt_img_free(lt_img_t* img, lt_alloc_t* alloc) {
	lt_mfree(alloc, img->data);
}

