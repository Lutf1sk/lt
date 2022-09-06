#include <lt/img.h>

b8 lt_img_load(lt_arena_t* arena, void* data, usz len, lt_img_t* img) {
	if (lt_img_load_tga(arena, data, len, img))
		return 1;
	if (lt_img_load_bmp(arena, data, len, img))
		return 1;
	return 0;
}

