#include <lt/img.h>
#include <lt/mem.h>

b8 lt_img_load(void* data, usz len, lt_img_t* img, lt_alloc_t* alloc) {
	if (lt_img_load_tga(data, len, img, alloc))
		return 1;
	if (lt_img_load_bmp(data, len, img, alloc))
		return 1;
	return 0;
}

void lt_img_free(lt_img_t* img, lt_alloc_t* alloc) {
	lt_mfree(alloc, img->data);
}

