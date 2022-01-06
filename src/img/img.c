#include <lt/img.h>

b8 lt_img_load(lt_arena_t* arena, void* data, usz len, lt_img_t* img) {
	return lt_img_load_tga(arena, data, len, img);
}

