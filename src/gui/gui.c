#include <lt/gui.h>
#include <lt/mem.h>

#define PAD 3
#define BUTTON_HPAD 16

#define BUTTON_BG 0xA0A0A0
#define BUTTON_TEXT 0x000000

#define BORDER 0x000000

void lt_gui_ctx_init(lt_arena_t* arena, lt_gui_ctx_t* cx) {
	cx->conts = lt_arena_reserve(arena, sizeof(lt_gui_cont_t) * cx->cont_max);

	LT_ASSERT(cx->draw_filled_rect);
	LT_ASSERT(cx->draw_rect);
	LT_ASSERT(cx->draw_text);
}

void lt_gui_begin(lt_gui_ctx_t* cx, isz w, isz h) {
	cx->cont_top = 1;

	cx->conts[0].x = -PAD;
	cx->conts[0].y = -PAD;
	cx->conts[0].w = w + PAD*2;
	cx->conts[0].h = h + PAD*2;
	cx->conts[0].yrem = h + PAD*2;
	cx->conts[0].xrem = w + PAD*2;
	cx->conts[0].cols = 0;
}

void lt_gui_end(lt_gui_ctx_t* cx) {
	LT_ASSERT(cx->cont_top);
	cx->prev_mouse_state = cx->mouse_state;
}

static
void get_avail_space(lt_gui_ctx_t* cx, isz* x, isz* y, isz* w, isz* h) {
	lt_gui_cont_t* c = &cx->conts[cx->cont_top - 1];

	if (!c->cols) {
		*x = c->x + PAD;
		*y = c->y + (c->h - c->yrem) + PAD;
		*w = c->w - PAD * 2;
		*h = c->yrem - PAD * 2;
	}
	else {
		*x = c->x + (c->w - c->xrem) + PAD;
		*y = c->y + (c->h - c->yrem) + PAD;
		*w = c->xrem - PAD * 2;
		*h = c->yrem - PAD * 2;
	}
}

static
void reserve_space(lt_gui_ctx_t* cx, isz w, isz h) {
	lt_gui_cont_t* c = &cx->conts[cx->cont_top - 1];

	if (!c->cols) {
		c->yrem -= h + PAD;
		c->xrem = c->w;
	}
	else {
		c->xrem -= w + PAD;
		if (c->ymax < h)
			c->ymax = h;

		if (!--c->cols)
			reserve_space(cx, 0, c->ymax);
	}
}

static
usz calc_size(isz avail, isz size) {
	if (size > 0)
		return size;

	size += avail;
	if (size < PAD*2)
		size = PAD*2;
	return size;
}

static
usz calc_area(isz avail, isz size, isz* pos) {
	if (size > 0)
		return size;

	*pos -= size;
	size += avail;
	if (size < PAD*2)
		size = PAD*2;
	return size;
}

static
b8 is_hovered(lt_gui_ctx_t* cx, isz x, isz y, isz w, isz h) {
	return	cx->mouse_x >= x && cx->mouse_x < x + w &&
			cx->mouse_y >= y && cx->mouse_y < y + h;
}

static
b8 mb_pressed(lt_gui_ctx_t* cx, isz btn) {
	return (cx->mouse_state >> btn) == 1 && (cx->prev_mouse_state >> btn) == 0;
}

void lt_gui_panel_begin(lt_gui_ctx_t* cx, isz w, isz h) {
	lt_gui_cont_t c;
	c.cols = 0;
	get_avail_space(cx, &c.x, &c.y, &c.w, &c.h);
	c.w = calc_size(c.w, w);
	c.h = calc_size(c.h, h);

	c.xrem = c.w;
	c.yrem = c.h;
	cx->conts[cx->cont_top++] = c;

	cx->draw_filled_rect(cx->user_data, c.x, c.y, c.w, c.h, 0x282828);
	cx->draw_rect(cx->user_data, c.x, c.y, c.w, c.h, 0x404040);

	cx->scissor(cx->user_data, c.x, c.y, c.w, c.h);
}

void lt_gui_panel_end(lt_gui_ctx_t* cx) {
	lt_gui_cont_t c = cx->conts[--cx->cont_top];
	reserve_space(cx, c.w, c.h);

	c = cx->conts[cx->cont_top - 1];
	cx->scissor(cx->user_data, c.x, c.y, c.w, c.h);
}

void lt_gui_row(lt_gui_ctx_t* cx, usz cols) {
	lt_gui_cont_t* c = &cx->conts[cx->cont_top - 1];
	c->cols = cols;
	c->xrem = c->w;
	c->ymax = 0;
}

void lt_gui_label(lt_gui_ctx_t* cx, lstr_t text) {
	isz x, y, w, h;
	get_avail_space(cx, &x, &y, &w, &h);
	cx->draw_text(cx->user_data, x, y, text, 0xFFFFFF);

	reserve_space(cx, text.len * cx->glyph_width, cx->glyph_height);
}

u8 lt_gui_button(lt_gui_ctx_t* cx, lstr_t text) {
	isz x, y, w, h;
	get_avail_space(cx, &x, &y, &w, &h);

	isz text_w = text.len * cx->glyph_width, text_h = cx->glyph_height;
	w = text_w + BUTTON_HPAD*2, h = text_h + 2;

	u32 bg = BUTTON_BG;
	b8 hovered = is_hovered(cx, x, y, w, h);
	if (hovered)
		bg += 0x202020;

	cx->draw_filled_rect(cx->user_data, x, y, w, h, bg);
	cx->draw_rect(cx->user_data, x, y, w, h, BORDER);

	isz text_x = x + BUTTON_HPAD, text_y = y + 1;
	cx->draw_text(cx->user_data, text_x, text_y, text, 0x000000);

	reserve_space(cx, w, h);

	return hovered && mb_pressed(cx, 0);
}

u8 lt_gui_expandable(lt_gui_ctx_t* cx, lstr_t text, isz tw) {
	isz x, y, w, h;
	get_avail_space(cx, &x, &y, &w, &h);

	isz text_w = text.len * cx->glyph_width, text_h = cx->glyph_height;
	w = calc_size(w, tw), h = text_h + 2;

	u32 bg = BUTTON_BG;
	b8 hovered = is_hovered(cx, x, y, w, h);
	if (hovered)
		bg += 0x202020;

	cx->draw_filled_rect(cx->user_data, x, y, w, h, bg);
	cx->draw_rect(cx->user_data, x, y, w, h, BORDER);

	cx->draw_text(cx->user_data, x + w/2 - text_w/2, y + 1, text, BUTTON_TEXT);

	reserve_space(cx, w, h);

	return hovered && mb_pressed(cx, 0);
}

void lt_gui_vspace(lt_gui_ctx_t* cx, usz space) {
	lt_gui_cont_t* c = &cx->conts[cx->cont_top - 1];

	isz x, y, w, h;
	get_avail_space(cx, &x, &y, &w, &h);

	c->yrem -= calc_size(h, space);
}

void lt_gui_hspace(lt_gui_ctx_t* cx, usz space) {
	lt_gui_cont_t* c = &cx->conts[cx->cont_top - 1];

	isz x, y, w, h;
	get_avail_space(cx, &x, &y, &w, &h);

	c->xrem -= calc_size(w, space);
}

