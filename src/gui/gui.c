#include <lt/gui.h>
#include <lt/mem.h>
#include <lt/str.h>
#include <lt/debug.h>

static
lt_gui_style_t lt_gui_default_style_ = {
	.panel_bg_clr = 0x282828FF,
	.panel_border_clr = 0x404040FF,

	.text_clr = 0xE0E0E0FF,
	.border_clr = 0x000000FF,

	.ctrl_bg_clr = 0x505050FF,
	.ctrl_text_clr = 0xE0E0E0FF,
	.ctrl_hover_bg_clr = 0x707070FF,
	.ctrl_hover_text_clr = 0xF0F0F0FF,

	.button_hpad = 8,

	.padding = 3,
	.spacing = 2,
	.border = 1,
};

lt_gui_style_t* lt_gui_default_style = &lt_gui_default_style_;

#define DEPTH_RECT_OFFS 1
#define DEPTH_ICON_OFFS 2
#define DEPTH_TEXT_OFFS 3
#define DEPTH_WINDOW_STEP 5

static
b8 is_hovered(const lt_gui_ctx_t cx[static 1], lt_gui_rect_t* r) {
	return	cx->mouse_x >= r->x && cx->mouse_x < r->x + r->w &&
			cx->mouse_y >= r->y && cx->mouse_y < r->y + r->h;
}

static
b8 mb_pressed(const lt_gui_ctx_t cx[static 1], isz btn) {
	return (cx->mouse_state >> btn) == 1 && (cx->prev_mouse_state >> btn) == 0;
}

static LT_INLINE
void set_scissor(const lt_gui_ctx_t cx[static 1], const lt_gui_rect_t r[static 1]) {
	cx->scissor(cx->user_data, r);
}

static LT_INLINE
lt_gui_cont_t* cont_top(const lt_gui_ctx_t cx[static 1]) {
	return &cx->conts[cx->cont_top - 1];
}

static LT_INLINE
void cont_pop(lt_gui_ctx_t cx[static 1]) {
	--cx->cont_top;
	set_scissor(cx, &cont_top(cx)->r);
}

static LT_INLINE
void cont_push(lt_gui_ctx_t cx[static 1], lt_gui_cont_t c) {
	cx->conts[cx->cont_top++] = c;
	set_scissor(cx, &cont_top(cx)->r);
}

static LT_INLINE
usz text_width(const lt_gui_ctx_t cx[static 1], lstr_t text) {
	return cx->text_width(cx->user_data, text);
}

lt_err_t lt_gui_ctx_init(lt_gui_ctx_t cx[static 1], lt_alloc_t alloc[static 1]) {
	LT_ASSERT(cx->cont_max != 0);

	cx->conts = lt_malloc(alloc, sizeof(lt_gui_cont_t) * cx->cont_max);
	if (!cx->conts)
		return LT_ERR_OUT_OF_MEMORY;

	LT_ASSERT(cx->draw_rect);
	LT_ASSERT(cx->draw_text);
	LT_ASSERT(cx->draw_icon);
	LT_ASSERT(cx->scissor);
	return LT_SUCCESS;
}

void lt_gui_ctx_free(const lt_gui_ctx_t cx[static 1], lt_alloc_t alloc[static 1]) {
	lt_mfree(alloc, cx->conts);
}

void lt_gui_begin(lt_gui_ctx_t cx[static 1], isz x, isz y, isz w, isz h) {
	cx->cont_top = 1;

	lt_gui_cont_t* c = &cx->conts[0];
	c->r = LT_GUI_RECT(x, y, w, h);
	c->a = c->r;
	c->cols = 0;
	c->depth = 0;

	c->padding = 0;
	c->spacing = cx->style->spacing;

	cx->hovered = is_hovered(cx, &cont_top(cx)->r);
}

void lt_gui_end(lt_gui_ctx_t cx[static 1]) {
	LT_ASSERT(cx->cont_top == 1);
	cx->prev_mouse_state = cx->mouse_state;
}

lt_gui_cont_t* lt_gui_get_container(const lt_gui_ctx_t cx[static 1]) {
	return cont_top(cx);
}

void lt_gui_draw_border(const lt_gui_ctx_t cx[static 1], const lt_gui_rect_t r[static 1], u32 clr, u32 flags) {
	u32 tl_clr = clr;
	u32 br_clr = clr;

	if (flags & LT_GUI_BORDER_OUTSET)
		tl_clr += 0x20202000;
	if (flags & LT_GUI_BORDER_INSET)
		br_clr += 0x20202000;

	lt_gui_rect_t rects[4] = {
		{ r->x + r->w - cx->style->border, r->y, cx->style->border, r->h },
		{ r->x, r->y + r->h - cx->style->border, r->w, cx->style->border },
		{ r->x, r->y, cx->style->border, r->h },
		{ r->x, r->y, r->w, cx->style->border },
	};

	lt_gui_cont_t* c = cont_top(cx);
	cx->draw_rect(cx->user_data, &rects[0], br_clr, c->depth + DEPTH_RECT_OFFS);
	cx->draw_rect(cx->user_data, &rects[1], br_clr, c->depth + DEPTH_RECT_OFFS);
	cx->draw_rect(cx->user_data, &rects[2], tl_clr, c->depth + DEPTH_RECT_OFFS);
	cx->draw_rect(cx->user_data, &rects[3], tl_clr, c->depth + DEPTH_RECT_OFFS);
}

void lt_gui_draw_rect(const lt_gui_ctx_t cx[static 1], const lt_gui_rect_t r[static 1], u32 clr) {
	cx->draw_rect(cx->user_data, r, clr, cont_top(cx)->depth + DEPTH_RECT_OFFS);
}

void lt_gui_draw_icon(const lt_gui_ctx_t cx[static 1], const lt_gui_rect_t r[static 1], u32 clr, u32 icon) {
	cx->draw_icon(cx->user_data, r, clr, icon, cont_top(cx)->depth + DEPTH_ICON_OFFS);
}

void lt_gui_draw_text(const lt_gui_ctx_t cx[static 1], i32 x, i32 y, lstr_t str, u32 clr) {
	cx->draw_text(cx->user_data, x, y, str, clr, cont_top(cx)->depth + DEPTH_TEXT_OFFS);
}

static
void seta_padded(const lt_gui_ctx_t cx[static 1], lt_gui_cont_t c[static 1]) {
	c->a = LT_GUI_RECT(c->r.x + cx->style->padding, c->r.y + cx->style->padding, c->r.w - cx->style->padding*2, c->r.h - cx->style->padding*2);
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

	if (flags & LT_GUI_GROW_X)
		r->w = c->a.w;

	if (flags & LT_GUI_ALIGN_RIGHT)
		r->x += c->a.w - r->w;

	if (c->cols) {
		if (flags & LT_GUI_ALIGN_RIGHT)
			c->a.w -= r->w + c->spacing;
		else {
			c->a.x += r->w + c->spacing;
			c->a.w -= r->w + c->spacing;
		}

		if (c->ymax < r->h)
			c->ymax = r->h;

		if (!--c->cols) {
			c->a.w = c->r.w - c->padding*2;
			c->a.x = c->r.x + c->padding;

			c->a.y += c->ymax + c->spacing;
			c->a.h -= c->ymax + c->spacing;
			c->ymax = 0;
		}
	}
	else {
		c->a.y += r->h + c->spacing;
		c->a.h -= r->h + c->spacing;
	}
}

void lt_gui_panel_begin(lt_gui_ctx_t cx[static 1], isz w, isz h, u32 flags) {
	lt_gui_cont_t c;
	c.r.w = w;
	c.r.h = h;
	c.cols = 0;
	c.padding = cx->style->padding;
	c.spacing = cx->style->spacing;
	c.depth = cont_top(cx)->depth;
	make_space(cx, &c.r, flags);
	seta_padded(cx, &c);

	cont_push(cx, c);
	lt_gui_draw_rect(cx, &c.r, cx->style->panel_bg_clr);
	lt_gui_draw_border(cx, &c.r, cx->style->panel_border_clr, flags);
}

void lt_gui_panel_end(lt_gui_ctx_t cx[static 1]) {
	cont_pop(cx);
}

void lt_gui_row(lt_gui_ctx_t cx[static 1], usz cols) {
	lt_gui_cont_t* c = cont_top(cx);
	c->cols = cols;
	c->ymax = 0;
}

void lt_gui_label(lt_gui_ctx_t cx[static 1], lstr_t text, u32 flags) {
	lt_gui_rect_t r = { 0, 0, text_width(cx, text), cx->glyph_height };
	make_space(cx, &r, flags);

	lt_gui_draw_text(cx, r.x, r.y, text, cx->style->text_clr);
}

void lt_gui_text(lt_gui_ctx_t cx[static 1], lstr_t text, u32 flags) {
	lt_gui_cont_t* c = cont_top(cx);
	lt_gui_rect_t r = c->a;
	r.h = 0;

	char* end = text.str + text.len;

	usz max_chars = r.w / text_width(cx, CLSTR(" ")); // !!
	if (!max_chars)
		max_chars = 1;

	char* word_start = text.str;
	char* line_start = text.str;
	for (char* it = text.str; it < end; ++it) {
		if (*it == ' ')
			word_start = it + 1;

		usz line_len = (it - line_start);

		if (line_len >= max_chars) {
			usz word_len = it - word_start;

			char* new_line_start = it;

			if (line_start != word_start) {
				new_line_start -= word_len;
				line_len -= word_len;
			}
			else
				word_start = it;

			lt_gui_draw_text(cx, r.x, r.y + r.h, LSTR(line_start, line_len), cx->style->text_clr);
			r.h += cx->glyph_height;

			line_start = new_line_start;
		}
	}

	usz remain = end - line_start;
	lt_gui_draw_text(cx, r.x, r.y + r.h, LSTR(line_start, remain), cx->style->text_clr);
	r.h += cx->glyph_height;

	make_space(cx, &r, flags);
}

b8 lt_gui_textbox(lt_gui_ctx_t cx[static 1], isz ew, isz eh, lt_gui_textbox_state_t state[static 1], u32 flags) {
	lt_gui_rect_t r = {0, 0, ew, eh};
	make_space(cx, &r, flags);

	u32 bg = cx->style->panel_bg_clr - 0x1A1A1A00;
	b8 hovered = is_hovered(cx, &r);
	if (hovered || state->selected)
		bg += 0x10101000;

	if (mb_pressed(cx, 0))
		state->selected = hovered;

	lstr_t text = LSTR(state->buf, strnlen(state->buf, state->maxlen));

	usz max_chars = (r.w - cx->style->padding*2) / text_width(cx, CLSTR(" ")); // !!
	if (text.len > max_chars) {
		usz offs = text.len - max_chars;
		text = LSTR(text.str + offs, text.len - offs);
	}

	isz x = r.x + cx->style->padding;
	lt_gui_draw_text(cx, x, r.y + 1, text, cx->style->text_clr);

	lt_gui_draw_rect(cx, &r, bg);
	lt_gui_draw_border(cx, &r, cx->style->border_clr, flags);

	if (state->selected) {
		lt_gui_rect_t cr = { x + text_width(cx, text), r.y + 1, 1, cx->glyph_height - 1 };
		lt_gui_draw_rect(cx, &cr, cx->style->text_clr);
	}

	return state->selected;
}

u8 lt_gui_button(lt_gui_ctx_t cx[static 1], lstr_t text, u32 flags) {
	isz text_w = text_width(cx, text);

	lt_gui_rect_t r = {0, 0, text_w + cx->style->button_hpad*2, cx->glyph_height + 2};
	make_space(cx, &r, flags);

	u32 bg = cx->style->ctrl_bg_clr;
	u32 fg = cx->style->ctrl_text_clr;
	b8 hovered = is_hovered(cx, &r);
	if (hovered) {
		bg = cx->style->ctrl_hover_bg_clr;
		fg = cx->style->ctrl_hover_text_clr;
	}

	lt_gui_draw_rect(cx, &r, bg);
	lt_gui_draw_border(cx, &r, cx->style->border_clr, flags);

	lt_gui_draw_text(cx, r.x + cx->style->button_hpad, r.y + 1, text, fg);

	return hovered && mb_pressed(cx, 0);
}

u8 lt_gui_expandable(lt_gui_ctx_t cx[static 1], lstr_t text, b8 expanded[static 1], u32 flags) {
	lt_gui_rect_t r = { 0, 0, 0, cx->glyph_height + 2 };
	make_space(cx, &r, flags);

	u32 bg = cx->style->ctrl_bg_clr;
	u32 fg = cx->style->ctrl_text_clr;
	b8 hovered = is_hovered(cx, &r);
	if (hovered) {
		bg = cx->style->ctrl_hover_bg_clr;
		fg = cx->style->ctrl_hover_text_clr;
		if (mb_pressed(cx, 0))
			*expanded = !*expanded;
	}

	usz icon = LT_GUI_ICON_COLLAPSED;
	if (*expanded)
		icon = LT_GUI_ICON_EXPANDED;

	lt_gui_draw_rect(cx, &r, bg);
	lt_gui_draw_border(cx, &r, cx->style->border_clr, flags);

	lt_gui_draw_text(cx, r.x + r.w/2 - text_width(cx, text)/2, r.y + 1, text, fg);

	lt_gui_rect_t ir = { r.x + 1, r.y + 1, cx->glyph_height, cx->glyph_height };
	lt_gui_draw_icon(cx, &ir, cx->style->ctrl_text_clr, icon);

	return *expanded;
}

void lt_gui_vspace(lt_gui_ctx_t cx[static 1], usz space, u32 flags) {
	lt_gui_rect_t r = {0, 0, 1, space};
	make_space(cx, &r, flags);
}

void lt_gui_hspace(lt_gui_ctx_t cx[static 1], usz space, u32 flags) {
	lt_gui_rect_t r = {0, 0, space, 1};
	make_space(cx, &r, flags);
}

b8 lt_gui_dropdown_begin(lt_gui_ctx_t cx[static 1], lstr_t text, isz ew, isz eh, u32 state[static 1], u32 flags) {
	i32 text_w = text_width(cx, text);

	lt_gui_rect_t r = { 0, 0, text_w + cx->style->padding*3 + cx->glyph_height, cx->glyph_height + 2 };
	make_space(cx, &r, flags);

	u32 bg = cx->style->ctrl_bg_clr;
	u32 fg = cx->style->ctrl_text_clr;
	b8 hovered = is_hovered(cx, &r);

	lt_gui_cont_t c;
	c.cols = 0;
	c.r = LT_GUI_RECT(r.x, r.y + r.h, ew, eh);
	c.a = c.r;
	c.padding = 0;
	c.spacing = 0;
	c.depth = cont_top(cx)->depth + DEPTH_WINDOW_STEP;

	if (hovered) {
		bg = cx->style->ctrl_hover_bg_clr;
		fg = cx->style->ctrl_hover_text_clr;
		if (mb_pressed(cx, 0))
			*state = !*state;
	}

	if (!hovered && !is_hovered(cx, &c.r))
		*state = 0;

	lt_gui_draw_rect(cx, &r, bg);
	lt_gui_draw_border(cx, &r, cx->style->border_clr, flags);

	lt_gui_draw_text(cx, r.x + cx->style->padding, r.y + 1, text, fg);

	lt_gui_rect_t ir = { r.x + r.w - cx->glyph_height - cx->style->padding, r.y + 1, cx->glyph_height, cx->glyph_height };
	lt_gui_draw_icon(cx, &ir, cx->style->ctrl_text_clr, LT_GUI_ICON_EXPANDED);

	if (*state) {
		cont_push(cx, c);

		lt_gui_draw_rect(cx, &c.r, cx->style->panel_bg_clr);
		lt_gui_draw_border(cx, &c.r, cx->style->panel_border_clr, 0);
	}

	return *state;
}

void lt_gui_dropdown_end(lt_gui_ctx_t cx[static 1]) {
	cont_pop(cx);
	cx->mouse_x = -1;
	cx->mouse_y = -1;
}

b8 lt_gui_checkbox(lt_gui_ctx_t cx[static 1], lstr_t text, b8 state[static 1], u32 flags) {
	i32 text_w = text_width(cx, text);

	lt_gui_rect_t r = { 0, 0, text_w + cx->style->padding*2 + cx->glyph_height, cx->glyph_height };
	make_space(cx, &r, flags);

	u32 bg = cx->style->checkbox_bg_clr;
	u32 fg = cx->style->text_clr;
	u32 icon_clr = cx->style->checkbox_icon_clr;
	b8 hovered = is_hovered(cx, &r);
	if (hovered) {
		bg = cx->style->checkbox_hover_bg_clr;
		if (mb_pressed(cx, 0))
			*state = !*state;
	}

	lt_gui_draw_text(cx, r.x + cx->glyph_height + cx->style->padding*2, r.y, text, fg);

	lt_gui_rect_t cr = { r.x, r.y, cx->glyph_height, cx->glyph_height };
	lt_gui_draw_rect(cx, &cr, bg);
	lt_gui_draw_border(cx, &cr, cx->style->border_clr, flags);

	if (*state)
		lt_gui_draw_icon(cx, &cr, icon_clr, LT_GUI_ICON_CHECK);

	return *state;
}

