#pragma once

#include <lt2/common.h>

typedef struct pixbuf {
	i32 width;
	i32 height;
	u32* data;
} pixbuf_t;

void pb_draw_rect(pixbuf_t* buf, i32 x, i32 y, i32 w, i32 h, u32 color);
void pb_fill_rect(pixbuf_t* buf, i32 x, i32 y, i32 w, i32 h, u32 color);
void pb_draw_line(pixbuf_t* buf, i32 x, i32 y, i32 x2, i32 y2, u32 color);
void pb_draw_vline(pixbuf_t* buf, i32 x, i32 y, i32 y2, u32 color);
void pb_draw_hline(pixbuf_t* buf, i32 x, i32 y, i32 x2, u32 color);
void pb_fill_circle(pixbuf_t* buf, i32 x, i32 y, i32 r, i32 color);

