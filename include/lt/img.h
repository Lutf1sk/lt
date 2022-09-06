#ifndef LT_IMG
#define LT_IMG 1

#include <lt/primitives.h>
#include <lt/fwd.h>

#define LT_IMG_TGA_MAGIC (char[1]){ 0x00 }
#define LT_IMG_BMP_MAGIC (char[2]){'B', 'M'}

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

b8 lt_img_load_tga(lt_arena_t* arena, void* data, usz len, lt_img_t* img);
b8 lt_img_load_bmp(lt_arena_t* arena, void* data, usz len, lt_img_t* img);
b8 lt_img_load(lt_arena_t* arena, void* data, usz len, lt_img_t* img);

#endif
