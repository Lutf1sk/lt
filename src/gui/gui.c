#include <lt/gui.h>
#include <lt/mem.h>

#define BUTTON_HPAD 16

#define PANEL_BG 0x282828
#define PANEL_BORDER 0x404040

#define BUTTON_BG 0x505050
#define BUTTON_TEXT 0xE0E0E0

#define BORDER_SIZE 1
#define BORDER 0x000000
#define TEXT 0xE0E0E0

#define PAD 3
#define SPACE 2

static
b8 is_hovered(lt_gui_ctx_t* cx, lt_gui_rect_t* r) {
	return	cx->mouse_x >= r->x && cx->mouse_x < r->x + r->w &&
			cx->mouse_y >= r->y && cx->mouse_y < r->y + r->h;
}

static
b8 mb_pressed(lt_gui_ctx_t* cx, isz btn) {
	return (cx->mouse_state >> btn) == 1 && (cx->prev_mouse_state >> btn) == 0;
}

static
lt_gui_cont_t* cont_top(lt_gui_ctx_t* cx) {
	return &cx->conts[cx->cont_top - 1];
}

static
lt_gui_cont_t cont_pop(lt_gui_ctx_t* cx) {
	return cx->conts[--cx->cont_top];
}

static LT_INLINE
void set_scissor(lt_gui_ctx_t* cx, lt_gui_rect_t* r) {
	cx->scissor(cx->user_data, r);
}



void lt_gui_ctx_init(lt_arena_t* arena, lt_gui_ctx_t* cx) {
	cx->conts = lt_arena_reserve(arena, sizeof(lt_gui_cont_t) * cx->cont_max);

	LT_ASSERT(cx->draw_rect);
	LT_ASSERT(cx->draw_text);
}

void lt_gui_begin(lt_gui_ctx_t* cx, isz w, isz h) {
	cx->cont_top = 1;

	lt_gui_cont_t* c = &cx->conts[0];
	c->r = LT_GUI_RECT(0, 0, w, h);
	c->a = c->r;
	c->cols = 0;

	c->padding = 0;
}

void lt_gui_end(lt_gui_ctx_t* cx) {
	LT_ASSERT(cx->cont_top == 1);
	cx->prev_mouse_state = cx->mouse_state;
}

lt_gui_cont_t* lt_gui_get_container(lt_gui_ctx_t* cx) {
	return cont_top(cx);
}

usz lt_gui_draw_border(lt_gui_rect_t* r, u32 clr, u32 flags, lt_gui_rect_t* out_r, u32* out_clr) {
	u32 tl_clr = clr;
	u32 br_clr = clr;

	if (flags & LT_GUI_BORDER_OUTSET)
		tl_clr += 0x202020;
	if (flags & LT_GUI_BORDER_INSET)
		br_clr += 0x202020;

	out_clr[0] = br_clr;
	out_clr[1] = br_clr;
	out_clr[2] = tl_clr;
	out_clr[3] = tl_clr;

	out_r[0] = (lt_gui_rect_t){ r->x + r->w - BORDER_SIZE, r->y, BORDER_SIZE, r->h };
	out_r[1] = (lt_gui_rect_t){ r->x, r->y + r->h - BORDER_SIZE, r->w, BORDER_SIZE };
	out_r[2] = (lt_gui_rect_t){ r->x, r->y, BORDER_SIZE, r->h };
	out_r[3] = (lt_gui_rect_t){ r->x, r->y, r->w, BORDER_SIZE };

	return 4;
}

static
void seta_padded(lt_gui_cont_t* c) {
	c->a = LT_GUI_RECT(c->r.x + PAD, c->r.y + PAD, c->r.w - PAD*2, c->r.h - PAD*2);
}

static
void make_space(lt_gui_ctx_t* cx, lt_gui_rect_t* r, u32 flags) {
	lt_gui_cont_t* c = cont_top(cx);

	r->x = c->a.x;
	r->y = c->a.y;

	if (r->w <= 0)
		r->w += c->a.w;
	if (r->h <= 0)
		r->h += c->a.h;

	if (flags & LT_GUI_ALIGN_RIGHT)
		r->x += c->a.w - r->w;

	if (c->cols) {
		if (flags & LT_GUI_ALIGN_RIGHT)
			c->a.w -= r->w + SPACE;
		else {
			c->a.x += r->w + SPACE;
			c->a.w -= r->w + SPACE;
		}

		if (c->ymax < r->h)
			c->ymax = r->h;

		if (!--c->cols) {
			c->a.w = c->r.w - c->padding*2;
			c->a.x = c->r.x + c->padding;

			c->a.y += c->ymax + SPACE;
			c->a.h -= c->ymax + SPACE;
			c->ymax = 0;
		}
	}
	else {
		c->a.y += r->h + SPACE;
		c->a.h -= r->h + SPACE;
	}
}

void lt_gui_panel_begin(lt_gui_ctx_t* cx, isz w, isz h, u32 flags) {
	lt_gui_cont_t c;
	c.r.w = w;
	c.r.h = h;
	c.cols = 0;
	c.padding = PAD;
	make_space(cx, &c.r, flags);
	seta_padded(&c);

	set_scissor(cx, &c.r);

	cx->conts[cx->cont_top++] = c;

	lt_gui_rect_t rects[5] = { c.r };
	u32 clrs[5] = { PANEL_BG };
	lt_gui_draw_border(&c.r, PANEL_BORDER, flags, &rects[1], &clrs[1]);
	cx->draw_rect(cx->user_data, 5, rects, clrs);
}

void lt_gui_panel_end(lt_gui_ctx_t* cx) {
	cont_pop(cx);
	set_scissor(cx, &cont_top(cx)->r);
}

void lt_gui_row(lt_gui_ctx_t* cx, usz cols) {
	lt_gui_cont_t* c = cont_top(cx);
	c->cols = cols;
	c->ymax = 0;
}

void lt_gui_label(lt_gui_ctx_t* cx, lstr_t text, u32 flags) {
	lt_gui_rect_t r = { 0, 0, text.len * cx->glyph_width, cx->glyph_height };
	make_space(cx, &r, flags);

	cx->draw_text(cx->user_data, r.x, r.y, text, TEXT);
}

u8 lt_gui_button(lt_gui_ctx_t* cx, lstr_t text, u32 flags) {
	isz text_w = text.len * cx->glyph_width;

	lt_gui_rect_t r = {0, 0, text_w + BUTTON_HPAD*2, cx->glyph_height + 2};
	make_space(cx, &r, flags);

	u32 bg = BUTTON_BG;
	b8 hovered = is_hovered(cx, &r);
	if (hovered)
		bg += 0x202020;

	lt_gui_rect_t rects[5] = { r };
	u32 clrs[5] = { bg };
	lt_gui_draw_border(&r, BORDER, flags, &rects[1], &clrs[1]);
	cx->draw_rect(cx->user_data, 5, rects, clrs);

	cx->draw_text(cx->user_data, r.x + BUTTON_HPAD, r.y + 1, text, BUTTON_TEXT);

	return hovered && mb_pressed(cx, 0);
}

u8 lt_gui_expandable(lt_gui_ctx_t* cx, lstr_t text, b8* expanded, u32 flags) {
	lt_gui_rect_t r = { 0, 0, 0, cx->glyph_height + 2 };
	make_space(cx, &r, flags);

	u32 bg = BUTTON_BG;
	b8 hovered = is_hovered(cx, &r);
	if (hovered) {
		bg += 0x202020;
		if (mb_pressed(cx, 0))
			*expanded = !*expanded;
	}

	usz icon = LT_GUI_ICON_COLLAPSED;
	if (*expanded)
		icon = LT_GUI_ICON_EXPANDED;

	lt_gui_rect_t rects[5] = { r };
	u32 clrs[5] = { bg };
	lt_gui_draw_border(&r, BORDER, flags, &rects[1], &clrs[1]);
	cx->draw_rect(cx->user_data, 5, rects, clrs);

	cx->draw_text(cx->user_data, r.x + r.w/2 - (text.len * cx->glyph_width)/2, r.y + 1, text, BUTTON_TEXT);

	lt_gui_rect_t ir = { r.x + 1, r.y + 1, cx->glyph_height, cx->glyph_height };
	cx->draw_icon(cx->user_data, icon, &ir, BUTTON_TEXT);

	return *expanded;
}

void lt_gui_vspace(lt_gui_ctx_t* cx, usz space, u32 flags) {
	lt_gui_rect_t r = {0, 0, 1, space};
	make_space(cx, &r, flags);
}

void lt_gui_hspace(lt_gui_ctx_t* cx, usz space, u32 flags) {
	lt_gui_rect_t r = {0, 0, space, 1};
	make_space(cx, &r, flags);
}

b8 lt_gui_dropdown_begin(lt_gui_ctx_t* cx, lstr_t text, isz ew, isz eh, u32* state, u32 flags) {
	i32 text_w = text.len * cx->glyph_width;

	lt_gui_rect_t r = { 0, 0, text_w + PAD*3 + cx->glyph_height, cx->glyph_height + 2 };
	make_space(cx, &r, flags);

	u32 bg = BUTTON_BG;
	b8 hovered = is_hovered(cx, &r);

	lt_gui_cont_t c;
	c.cols = 0;
	c.r = LT_GUI_RECT(r.x, r.y + r.h, ew, eh);
	c.a = c.r;
	c.padding = 0;

	if (hovered) {
		bg += 0x202020;
		if (mb_pressed(cx, 0))
			*state = !*state;
	}

	if (!hovered && !is_hovered(cx, &c.r))
		*state = 0;

	lt_gui_rect_t rects[10] = { r };
	u32 clrs[10] = { bg };
	usz rect_count = 1;

	rect_count += lt_gui_draw_border(&r, BORDER, flags, &rects[rect_count], &clrs[rect_count]);

	if (*state) {
		cx->conts[cx->cont_top++] = c;

		rects[rect_count] = c.r;
		clrs[rect_count++] = PANEL_BG;

		rect_count += lt_gui_draw_border(&c.r, 0xFF, 0, &rects[rect_count], &clrs[rect_count]);
	}

	cx->draw_rect(cx->user_data, rect_count, rects, clrs);

	cx->draw_text(cx->user_data, r.x + PAD, r.y + 1, text, BUTTON_TEXT);

	lt_gui_rect_t ir = { r.x + r.w - cx->glyph_height - PAD, r.y + 1, cx->glyph_height, cx->glyph_height };
	cx->draw_icon(cx->user_data, LT_GUI_ICON_EXPANDED, &ir, BUTTON_TEXT);

	return *state;
}

void lt_gui_dropdown_end(lt_gui_ctx_t* cx) {
	cont_pop(cx);
	set_scissor(cx, &cont_top(cx)->r);
}

b8 lt_gui_checkbox(lt_gui_ctx_t* cx, lstr_t text, b8* state, u32 flags) {
	i32 text_w = text.len * cx->glyph_width;

	lt_gui_rect_t r = { 0, 0, text_w + PAD*2 + cx->glyph_height, cx->glyph_height };
	make_space(cx, &r, flags);

	u32 bg = PANEL_BG - 0x1A1A1A;
	b8 hovered = is_hovered(cx, &r);
	if (hovered) {
		bg += 0x101010;
		if (mb_pressed(cx, 0))
			*state = !*state;
	}

	cx->draw_text(cx->user_data, r.x + cx->glyph_height + PAD*2, r.y, text, TEXT);

	lt_gui_rect_t rects[5] = { { r.x, r.y, cx->glyph_height, cx->glyph_height } };
	u32 clrs[5] = { bg };
	lt_gui_draw_border(&rects[0], BORDER, flags, &rects[1], &clrs[1]);
	cx->draw_rect(cx->user_data, 5, rects, clrs);

	if (*state)
		cx->draw_icon(cx->user_data, LT_GUI_ICON_CHECK, &rects[0], TEXT);

	return *state;
}

