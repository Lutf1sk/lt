#include <lt/gui.h>
#include <lt/mem.h>
#include <lt/str.h>

static
lt_gui_style_t lt_gui_default_style_ = {
	.panel_bg_clr = 0xFF282828,
	.panel_border_clr = 0xFF404040,

	.text_clr = 0xFFE0E0E0,
	.border_clr = 0xFF000000,

	.ctrl_bg_clr = 0xFF505050,
	.ctrl_text_clr = 0xFFE0E0E0,

	.button_hpad = 8,

	.padding = 3,
	.spacing = 2,
	.border = 1,
};

lt_gui_style_t* lt_gui_default_style = &lt_gui_default_style_;

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

b8 lt_gui_ctx_init(lt_gui_ctx_t* cx, lt_alloc_t* alloc) {
	cx->conts = lt_malloc(alloc, sizeof(lt_gui_cont_t) * cx->cont_max);
	if (!cx->conts)
		return 0;

	memset(&cx->cmdbufs, 0, sizeof(cx->cmdbufs));

	LT_ASSERT(cx->draw_rect);
	LT_ASSERT(cx->draw_text);
	return 1;
}

void lt_gui_ctx_free(lt_gui_ctx_t* cx, lt_alloc_t* alloc) {
	lt_mfree(alloc, cx->conts);
}

void lt_gui_begin(lt_gui_ctx_t* cx, isz x, isz y, isz w, isz h) {
	cx->cont_top = 1;

	lt_gui_cont_t* c = &cx->conts[0];
	c->r = LT_GUI_RECT(x, y, w, h);
	c->a = c->r;
	c->cols = 0;

	c->padding = 0;
	c->spacing = cx->style->spacing;

	cx->cbuf = &cx->cmdbufs[0];
}

void lt_gui_end(lt_gui_ctx_t* cx) {
	for (usz i = 0; i < 2; ++i) {
		lt_gui_command_buffer_t* cbuf = &cx->cmdbufs[i];

		cx->draw_rect(cx->user_data, cbuf->box_count, cbuf->box_rects, cbuf->box_clrs);

		for (usz i = 0; i < cbuf->icon_count; ++i)
			cx->draw_icon(cx->user_data, cbuf->icon_ids[i], &cbuf->icon_rects[i], cbuf->icon_clrs[i]);

		cx->draw_text(cx->user_data, cbuf->text_count, cbuf->text_points, cbuf->text_strs, cbuf->text_clrs);

		cbuf->box_count = 0;
		cbuf->icon_count = 0;
		cbuf->text_count = 0;
		cbuf->text_data_idx = 0;
	}

	LT_ASSERT(cx->cont_top == 1);
	cx->prev_mouse_state = cx->mouse_state;
}

lt_gui_cont_t* lt_gui_get_container(lt_gui_ctx_t* cx) {
	return cont_top(cx);
}

void lt_gui_draw_border(lt_gui_ctx_t* cx, lt_gui_rect_t* r, u32 clr, u32 flags) {
	u32 tl_clr = clr;
	u32 br_clr = clr;

	if (flags & LT_GUI_BORDER_OUTSET)
		tl_clr += 0x202020;
	if (flags & LT_GUI_BORDER_INSET)
		br_clr += 0x202020;

	usz count = cx->cbuf->box_count;
	u32* clrs = cx->cbuf->box_clrs;
	lt_gui_rect_t* rects = cx->cbuf->box_rects;

	clrs[count + 0] = br_clr;
	clrs[count + 1] = br_clr;
	clrs[count + 2] = tl_clr;
	clrs[count + 3] = tl_clr;

	rects[count + 0] = (lt_gui_rect_t){ r->x + r->w - cx->style->border, r->y, cx->style->border, r->h };
	rects[count + 1] = (lt_gui_rect_t){ r->x, r->y + r->h - cx->style->border, r->w, cx->style->border };
	rects[count + 2] = (lt_gui_rect_t){ r->x, r->y, cx->style->border, r->h };
	rects[count + 3] = (lt_gui_rect_t){ r->x, r->y, r->w, cx->style->border };

	cx->cbuf->box_count += 4;
}

void lt_gui_draw_rect(lt_gui_ctx_t* cx, lt_gui_rect_t* r, u32 clr) {
	usz count = cx->cbuf->box_count;
	cx->cbuf->box_clrs[count] = clr;
	cx->cbuf->box_rects[count] = *r;
	++cx->cbuf->box_count;
}

void lt_gui_draw_icon(lt_gui_ctx_t* cx, u32 icon, lt_gui_rect_t* r, u32 clr) {
	usz count = cx->cbuf->icon_count;
	cx->cbuf->icon_clrs[count] = clr;
	cx->cbuf->icon_rects[count] = *r;
	cx->cbuf->icon_ids[count] = icon;
	++cx->cbuf->icon_count;
}

void lt_gui_draw_text(lt_gui_ctx_t* cx, i32 x, i32 y, lstr_t str, u32 clr) {
	char* cmd_str = &cx->cbuf->text_data[cx->cbuf->text_data_idx];
	memcpy(cmd_str, str.str, str.len);

	cx->cbuf->text_data_idx += str.len;

	usz count = cx->cbuf->text_count;
	cx->cbuf->text_clrs[count] = clr;
	cx->cbuf->text_points[count] = LT_GUI_POINT(x, y);
	cx->cbuf->text_strs[count] = LSTR(cmd_str, str.len);
	++cx->cbuf->text_count;
}

static
void seta_padded(lt_gui_ctx_t* cx, lt_gui_cont_t* c) {
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

void lt_gui_panel_begin(lt_gui_ctx_t* cx, isz w, isz h, u32 flags) {
	lt_gui_cont_t c;
	c.r.w = w;
	c.r.h = h;
	c.cols = 0;
	c.padding = cx->style->padding;
	c.spacing = cx->style->spacing;
	make_space(cx, &c.r, flags);
	seta_padded(cx, &c);

// 	set_scissor(cx, &c.r);

	cx->conts[cx->cont_top++] = c;
	lt_gui_draw_rect(cx, &c.r, cx->style->panel_bg_clr);
	lt_gui_draw_border(cx, &c.r, cx->style->panel_border_clr, flags);
}

void lt_gui_panel_end(lt_gui_ctx_t* cx) {
	cont_pop(cx);
// 	set_scissor(cx, &cont_top(cx)->r);
}

void lt_gui_row(lt_gui_ctx_t* cx, usz cols) {
	lt_gui_cont_t* c = cont_top(cx);
	c->cols = cols;
	c->ymax = 0;
}

void lt_gui_label(lt_gui_ctx_t* cx, lstr_t text, u32 flags) {
	lt_gui_rect_t r = { 0, 0, text.len * cx->glyph_width, cx->glyph_height };
	make_space(cx, &r, flags);

	lt_gui_draw_text(cx, r.x, r.y, text, cx->style->text_clr);
}

void lt_gui_text(lt_gui_ctx_t* cx, lstr_t text, u32 flags) {
	lt_gui_cont_t* c = lt_gui_get_container(cx);
	lt_gui_rect_t r = c->a;
	r.h = 0;

	char* end = text.str + text.len;

	usz max_chars = r.w / cx->glyph_width;
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

b8 lt_gui_textbox(lt_gui_ctx_t* cx, isz ew, isz eh, lt_gui_textbox_state_t* state, u32 flags) {
	lt_gui_rect_t r = {0, 0, ew, eh};
	make_space(cx, &r, flags);

	u32 bg = cx->style->panel_bg_clr - 0x1A1A1A;
	b8 hovered = is_hovered(cx, &r);
	if (hovered || state->selected)
		bg += 0x101010;

	if (mb_pressed(cx, 0))
		state->selected = hovered;

	lstr_t text = LSTR(state->buf, strnlen(state->buf, state->maxlen));

	usz max_chars = (r.w - cx->style->padding*2) / cx->glyph_width;;
	if (text.len > max_chars) {
		usz offs = text.len - max_chars;
		text = LSTR(text.str + offs, text.len - offs);
	}

	isz x = r.x + cx->style->padding;
	lt_gui_draw_text(cx, x, r.y + 1, text, cx->style->text_clr);

	lt_gui_draw_rect(cx, &r, bg);
	lt_gui_draw_border(cx, &r, cx->style->border_clr, flags);

	if (state->selected) {
		lt_gui_rect_t cr = { x + text.len * cx->glyph_width, r.y + 1, 1, cx->glyph_height - 1 };
		lt_gui_draw_rect(cx, &cr, cx->style->text_clr);
	}

	return state->selected;
}

u8 lt_gui_button(lt_gui_ctx_t* cx, lstr_t text, u32 flags) {
	isz text_w = text.len * cx->glyph_width;

	lt_gui_rect_t r = {0, 0, text_w + cx->style->button_hpad*2, cx->glyph_height + 2};
	make_space(cx, &r, flags);

	u32 bg = cx->style->ctrl_bg_clr;
	b8 hovered = is_hovered(cx, &r);
	if (hovered)
		bg += 0x202020;

	lt_gui_draw_rect(cx, &r, bg);
	lt_gui_draw_border(cx, &r, cx->style->border_clr, flags);

	lt_gui_draw_text(cx, r.x + cx->style->button_hpad, r.y + 1, text, cx->style->ctrl_text_clr);

	return hovered && mb_pressed(cx, 0);
}

u8 lt_gui_expandable(lt_gui_ctx_t* cx, lstr_t text, b8* expanded, u32 flags) {
	lt_gui_rect_t r = { 0, 0, 0, cx->glyph_height + 2 };
	make_space(cx, &r, flags);

	u32 bg = cx->style->ctrl_bg_clr;
	b8 hovered = is_hovered(cx, &r);
	if (hovered) {
		bg += 0x202020;
		if (mb_pressed(cx, 0))
			*expanded = !*expanded;
	}

	usz icon = LT_GUI_ICON_COLLAPSED;
	if (*expanded)
		icon = LT_GUI_ICON_EXPANDED;

	lt_gui_draw_rect(cx, &r, bg);
	lt_gui_draw_border(cx, &r, cx->style->border_clr, flags);

	lt_gui_draw_text(cx, r.x + r.w/2 - (text.len * cx->glyph_width)/2, r.y + 1, text, cx->style->ctrl_text_clr);

	lt_gui_rect_t ir = { r.x + 1, r.y + 1, cx->glyph_height, cx->glyph_height };
	lt_gui_draw_icon(cx, icon, &ir, cx->style->ctrl_text_clr);

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

	lt_gui_rect_t r = { 0, 0, text_w + cx->style->padding*3 + cx->glyph_height, cx->glyph_height + 2 };
	make_space(cx, &r, flags);

	u32 bg = cx->style->ctrl_bg_clr;
	b8 hovered = is_hovered(cx, &r);

	lt_gui_cont_t c;
	c.cols = 0;
	c.r = LT_GUI_RECT(r.x, r.y + r.h, ew, eh);
	c.a = c.r;
	c.padding = 0;
	c.spacing = 0;

	if (hovered) {
		bg += 0x202020;
		if (mb_pressed(cx, 0))
			*state = !*state;
	}

	if (!hovered && !is_hovered(cx, &c.r))
		*state = 0;

	lt_gui_draw_rect(cx, &r, bg);
	lt_gui_draw_border(cx, &r, cx->style->border_clr, flags);

	lt_gui_draw_text(cx, r.x + cx->style->padding, r.y + 1, text, cx->style->ctrl_text_clr);

	lt_gui_rect_t ir = { r.x + r.w - cx->glyph_height - cx->style->padding, r.y + 1, cx->glyph_height, cx->glyph_height };
	lt_gui_draw_icon(cx, LT_GUI_ICON_EXPANDED, &ir, cx->style->ctrl_text_clr);

	if (*state) {
		cx->conts[cx->cont_top++] = c;
		++cx->cbuf;

		lt_gui_draw_rect(cx, &c.r, cx->style->panel_bg_clr);
		lt_gui_draw_border(cx, &c.r, cx->style->panel_border_clr, 0);
	}

	return *state;
}

void lt_gui_dropdown_end(lt_gui_ctx_t* cx) {
	--cx->cbuf;
	cont_pop(cx);
	cx->mouse_x = -1;
	cx->mouse_y = -1;
// 	set_scissor(cx, &cont_top(cx)->r);
}

b8 lt_gui_checkbox(lt_gui_ctx_t* cx, lstr_t text, b8* state, u32 flags) {
	i32 text_w = text.len * cx->glyph_width;

	lt_gui_rect_t r = { 0, 0, text_w + cx->style->padding*2 + cx->glyph_height, cx->glyph_height };
	make_space(cx, &r, flags);

	u32 bg = cx->style->panel_bg_clr - 0x1A1A1A;
	b8 hovered = is_hovered(cx, &r);
	if (hovered) {
		bg += 0x101010;
		if (mb_pressed(cx, 0))
			*state = !*state;
	}

	lt_gui_draw_text(cx, r.x + cx->glyph_height + cx->style->padding*2, r.y, text, cx->style->text_clr);

	lt_gui_rect_t cr = { r.x, r.y, cx->glyph_height, cx->glyph_height };
	lt_gui_draw_rect(cx, &cr, bg);
	lt_gui_draw_border(cx, &cr, cx->style->border_clr, flags);

	if (*state)
		lt_gui_draw_icon(cx, LT_GUI_ICON_CHECK, &cr, cx->style->ctrl_text_clr);

	return *state;
}

