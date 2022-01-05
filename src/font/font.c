#include <lt/font.h>

lt_font_t* lt_font_load(lt_arena_t* arena, void* data, usz len) {
	return lt_font_load_psf(arena, data, len);
}

