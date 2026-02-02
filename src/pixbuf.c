#include <lt2/pixbuf.h>

FLATTEN
void pb_draw_rect(pixbuf_t* buf, i32 x, i32 y, i32 w, i32 h, u32 color) {
	if (w <= 0 || h <= 0)
		return;

	i32 x2 = x + w;
	i32 y2 = y + h;
	if (x2 < 0 || x >= buf->width || y2 < 0 || y >= buf->height)
		return;

	pb_draw_vline(buf, x, y, y2, color);
	pb_draw_vline(buf, x + w - 1, y, y2, color);
	pb_draw_hline(buf, x, y, x2, color);
	pb_draw_hline(buf, x, y + h - 1, x2, color);
}

void pb_fill_rect(pixbuf_t* buf, i32 x, i32 y, i32 w, i32 h, u32 color) {
	if (w <= 0 || h <= 0)
		return;

	i32 x2 = x + w;
	i32 y2 = y + h;
	if (x2 < 0 || x >= buf->width || y2 < 0 || y >= buf->height)
		return;

	if (x < 0)
		x = 0;
	if (y < 0)
		y = 0;

	if (x2 > buf->width)
		x2 = buf->width;
	if (y2 > buf->height)
		y2 = buf->height;

	for (i32 row = y; row < y2; ++row) {
		for (i32 col = x; col < x2; ++col) {
			buf->data[row * buf->width + col] = color;
		}
	}
}

static
void line_lo(pixbuf_t* buf, i32 x1, i32 y1, i32 x2, i32 y2, u32 color) {
	if (x2 < x1) {
		i32 tmp = x2;
		x2 = x1;
		x1 = tmp;

		tmp = y2;
		y2 = y1;
		y1 = tmp;
	}

	i32 dx = x2 - x1;
	i32 dy = y2 - y1;
	i32 yi = 1;
	if (dy < 0) {
		yi = -1;
		dy = -dy;
	}

	isz d = dy * 2 - dx;
	for (i32 x = x1, y = y1; x <= x2; ++x) {
		if (y < 0 || x >= buf->width || y < 0 || y >= buf->height) // !! this is dumb
			continue;

		buf->data[y * buf->width + x] = color;
		if (d > 0) {
			y += yi;
			d -= (dx - dy) * 2;
		}
		else {
			d += dy * 2;
		}
	}
}

static
void line_hi(pixbuf_t* buf, i32 x1, i32 y1, i32 x2, i32 y2, u32 color) {
	if (y2 < y1) {
		i32 tmp = y2;
		y2 = y1;
		y1 = tmp;

		tmp = x2;
		x2 = x1;
		x1 = tmp;
	}

	i32 dx = x2 - x1;
	i32 dy = y2 - y1;
	i32 xi = 1;
	if (dx < 0) {
		xi = -1;
		dx = -dx;
	}

	isz d = dx * 2 - dy;
	for (i32 x = x1, y = y1; y <= y2; ++y) {
		if (y < 0 || y >= buf->height || x < 0 || x >= buf->width) // !! this is dumb
			continue;

		buf->data[y * buf->width + x] = color;
		if (d > 0) {
			x += xi;
			d -= (dy - dx) * 2;
		}
		else {
			d += dx * 2;
		}
	}
}

FLATTEN
void pb_draw_line(pixbuf_t* buf, i32 x, i32 y, i32 x2, i32 y2, u32 color) {
	i32 adx = x2 - x;
	if (adx < 0)
		adx = -adx;
	i32 ady = y2 - y;
	if (ady < 0)
		ady = -ady;

	if (adx > ady)
		line_lo(buf, x, y, x2, y2, color);
	else
		line_hi(buf, x, y, x2, y2, color);
}

void pb_draw_hline(pixbuf_t* buf, i32 x, i32 y, i32 x2, u32 color) {
	if (y < 0 || y >= buf->height)
		return;

	if (x2 < x) {
		i32 tmp = x2;
		x2 = x;
		x = tmp;
	}

	if (x < 0)
		x = 0;
	if (x2 > buf->width)
		x2 = buf->width;

	for (i32 i = x; i < x2; ++i)
		buf->data[y * buf->width + i] = color;
}

void pb_draw_vline(pixbuf_t* buf, i32 x, i32 y, i32 y2, u32 color) {
	if (x < 0 || x >= buf->width)
		return;

	if (y2 < y) {
		i32 tmp = y2;
		y2 = y;
		y = tmp;
	}

	if (y < 0)
		y = 0;
	if (y2 > buf->height)
		y2 = buf->height;

	for (i32 i = y; i < y2; ++i)
		buf->data[i * buf->width + x] = color;
}

#ifndef ON_WASI
#	include <math.h>

void pb_fill_circle(pixbuf_t* buf, i32 x, i32 y, i32 r, i32 color) {
	i32 min_y = y - r;
	i32 max_y = y + r;
	if (min_y < 0)
		min_y = 0;
	if (max_y > buf->height)
		max_y = buf->height;

	i32 r2 = r * r;

	for (i32 iy = min_y; iy < max_y; ++iy) {
		i32 i = iy - y;
		i32 hw = sqrt(r2 - i * i);

		i32 min_x = x - hw;
		i32 max_x = x + hw;
		if (min_x < 0)
			min_x = 0;
		if (max_x > buf->width)
			max_x = buf->width;

		for (i32 ix = min_x; ix < max_x; ++ix)
			buf->data[iy * buf->width + ix] = color;
	}
}
#endif

