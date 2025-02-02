#include <lt/io.h>
#include <lt/gui.h>
#include <lt/mem.h>
#include <lt/str.h>
#include <lt/debug.h>

static lt_gstyle_t root_panel_style = {};

static lt_gstyle_t default_panel_style = {
	.bg_color     = 0x202020FF,
	.border_color = 0x050505FF,
	.border       = 3,
	.padding      = 3,
	.spacing      = 3,
};

static lt_gstyle_t default_panel_style_dark = {
	.bg_color     = 0x161616FF,
	.border_color = 0x050505FF,
	.border       = 3,
	.padding      = 3,
};

static lt_gstyle_t default_panel_style_bright = {
	.bg_color     = 0x282828FF,
	.border_color = 0x050505FF,
	.border       = 3,
	.padding      = 3,
};

static lt_gstyle_t default_header_hover_style = {
	.text_color   = 0xFF9161FF,
	.icon_color   = 0xFF9161FF,
};

static lt_gstyle_t default_header_style = {
	.text_color   = 0xFF9161FF,
	.icon_color   = 0xFF9161FF,
	.hover_style  = &default_header_hover_style,
};

static lt_gstyle_t default_button_hover_style = {
	.bg_color     = 0x252525FF,
	.border_color = 0x050505FF,
	.text_color   = 0xD2B49FFF,
	.border       = 2,
	.padding      = 2,
};

static lt_gstyle_t default_button_style = {
	.bg_color     = 0x161616FF,
	.border_color = 0x050505FF,
	.text_color   = 0xD2B49FFF,
	.border       = 2,
	.padding      = 2,
	.hover_style  = &default_button_hover_style,
};

static lt_gstyle_t default_text_style = {
	.text_color   = 0xD2B49FFF,
};

static lt_gstyle_t default_label_style = {
	.text_color   = 0xD2B49FFF,
};

static lt_gstyle_t default_checkbox_hover_style = {
	.text_color   = 0xD2B49FFF,
	.icon_color   = 0xD2B49FFF,
	.bg_color     = 0x252525FF,
	.border_color = 0x050505FF,
	.spacing      = 6,
	.border       = 2,
};

static lt_gstyle_t default_checkbox_style = {
	.text_color   = 0xD2B49FFF,
	.icon_color   = 0xD2B49FFF,
	.bg_color     = 0x161616FF,
	.border_color = 0x050505FF,
	.spacing      = 6,
	.border       = 2,
	.hover_style  = &default_checkbox_hover_style,
};

static lt_gstyle_t default_textbox_hover_style = {
	.text_color   = 0xD2B49FFF,
	.icon_color   = 0xD2B49FFF,
	.bg_color     = 0x252525FF,
	.border_color = 0x050505FF,
	.border       = 2,
	.padding      = 2,  
};

static lt_gstyle_t default_textbox_style = {
	.text_color   = 0xD2B49FFF,
	.icon_color   = 0xD2B49FFF,
	.bg_color     = 0x161616FF,
	.border_color = 0x050505FF,
	.border       = 2,
	.padding      = 2,  
	.hover_style  = &default_textbox_hover_style,
};

static lt_gstyle_t default_slider_hover_style = {
	.text_color   = 0xD2B49FFF,
	.icon_color   = 0xD2B49FFF,
	.bg_color     = 0x252525FF,
	.border       = 2,
	.border_color = 0x050505FF,
	.spacing      = 6,
};

static lt_gstyle_t default_slider_style = {
	.text_color   = 0xD2B49FFF,
	.icon_color   = 0xD2B49FFF,
	.bg_color     = 0x161616FF,
	.border       = 2,
	.border_color = 0x050505FF,
	.spacing      = 6,
	.hover_style  = &default_slider_hover_style,
};

lt_err_t lt_ginit(lt_gcontext_t cx[static 1]) {
	if (!cx->alloc) {
		cx->alloc = lt_libc_heap;
	}

	if (!cx->panel_style) {
		cx->panel_style = &default_panel_style;
	}
	if (!cx->panel_style_dark) {
		cx->panel_style_dark = &default_panel_style_dark;
	}
	if (!cx->panel_style_bright) {
		cx->panel_style_bright = &default_panel_style_bright;
	}
	if (!cx->header_style) {
		cx->header_style = &default_header_style;
	}
	if (!cx->button_style) {
		cx->button_style = &default_button_style;
	}
	if (!cx->text_style) {
		cx->text_style = &default_text_style;
	}
	if (!cx->label_style) {
		cx->label_style = &default_label_style;
	}
	if (!cx->checkbox_style) {
		cx->checkbox_style = &default_checkbox_style;
	}
	if (!cx->textbox_style) {
		cx->textbox_style = &default_textbox_style;
	}
	if (!cx->slider_style) {
		cx->slider_style = &default_slider_style;
	}

	if (!cx->containers) {
		if (!cx->max_container_count) {
			cx->max_container_count = 64;
		}

		cx->containers = lt_malloc(cx->alloc, cx->max_container_count * sizeof(*cx->containers));
		if (!cx->containers) {
			return LT_ERR_OUT_OF_MEMORY;
		}
	}
	cx->curr_container = NULL;

	LT_ASSERT(cx->max_container_count > 0);

	LT_ASSERT(cx->draw_rect);
	LT_ASSERT(cx->draw_text);
	LT_ASSERT(cx->draw_icon);

	cx->mouse_x = 0;
	cx->mouse_y = 0;
	cx->mouse_state = 0;

	return LT_SUCCESS;
}

void lt_gcontext_begin(lt_gcontext_t cx[static 1], const lt_grect_t* bounds) {
	cx->curr_container = cx->containers;
	*cx->curr_container = (lt_gcontainer_t) { .style = &root_panel_style, .cols = 1, .depth = 0, };
	if (bounds) {
		cx->curr_container->bounds = *bounds;
		cx->curr_container->free = cx->curr_container->bounds;
	}
}

void lt_gcontext_end(lt_gcontext_t cx[static 1]) {
	if (cx->curr_container != cx->containers) {
		lt_werrf("imbalanced container stack\n");
	}
}

static
lt_grect_t reserve_space(lt_gcontext_t cx[static 1], u32 flags, lt_vec2i_t size) {
	lt_gcontainer_t* parent = cx->curr_container;

	i32 w = lt_u32_min(size.x, parent->free.w);
	i32 h = size.y; //lt_u32_min(size.y, parent->free.h);

	if (parent->row) {
		parent->free.y += parent->style->spacing;
		parent->free.h -= parent->style->spacing;
	}
	if (parent->col) {
		parent->free.x += parent->style->spacing;
		parent->free.w -= parent->style->spacing;
	}

	lt_grect_t res = {
		.x = parent->free.x,
		.y = parent->free.y,
		.w = w,
		.h = h
	};

	i32 vgrow = -(parent->free.h - h);
	if (vgrow > 0) {
		parent->full = 1;

		parent->free.h += vgrow;
		parent->bounds.h += vgrow;

		lt_gcontainer_t* it = parent - 1;
		while (it >= cx->containers) { // !! this does not grow all ancestors
			it->free.y += vgrow;
			it->free.h -= vgrow;

			it->full = (it->free.h <= 0);
			--it;
		}
	}

	if (++parent->col == parent->cols) {
		parent->free.x = parent->bounds.x + (parent->style->border + parent->style->padding);
		parent->free.w = parent->bounds.w - (parent->style->border + parent->style->padding) * 2;
		parent->col = 0;
		++parent->row;

		parent->free.y += h;
		parent->free.h -= h;
	}
	else {
		parent->free.x += w;
		parent->free.w -= w;
	}

	return res;
}

lt_gcontainer_t* lt_gpush_container(lt_gcontext_t cx[static 1], const lt_gstyle_t style[static 1], const lt_grect_t bounds[static 1]) {
	LT_ASSERT(cx->curr_container < cx->containers + cx->max_container_count);

	lt_grect_t free = {
		.x = bounds->x + (style->padding + style->border),
		.y = bounds->y + (style->padding + style->border),
		.w = bounds->w - (style->padding + style->border) * 2,
		.h = bounds->h - (style->padding + style->border) * 2,
	};

	lt_gcontainer_t* c = ++cx->curr_container;
	*c = (lt_gcontainer_t) {
		.style = style,
		.cols = 1,
		.depth = 0,
		.bounds = *bounds,
		.free = free };
	return c;
}

void lt_gpop_container(lt_gcontext_t cx[static 1]) {
	LT_ASSERT(cx->curr_container > cx->containers);
	--cx->curr_container;
}

static LT_INLINE
b8 is_hovered(lt_gcontext_t cx[static 1], lt_grect_t rect) {
	return cx->mouse_x >= rect.x && cx->mouse_x < rect.x + rect.w && cx->mouse_y >= rect.y && cx->mouse_y < rect.y + rect.h;
}

static
void draw_border(lt_gcontext_t cx[static 1], const lt_grect_t r[static 1], const lt_gstyle_t style[static 1], u32 depth) {
	if (!(style->border_color & 0xFF) || !style->border) {
		return;
	}
	u32 width = style->border;
	u32 color = style->border_color;
	cx->draw_rect(cx->usr, &LT_GRECT(r->x, r->y, r->w, width), color, depth);
	cx->draw_rect(cx->usr, &LT_GRECT(r->x, r->y + r->h - width, r->w, width), color, depth);
	cx->draw_rect(cx->usr, &LT_GRECT(r->x, r->y, width, r->h), color, depth);
	cx->draw_rect(cx->usr, &LT_GRECT(r->x + r->w - width, r->y, width, r->h), color, depth);
}

static
void draw_bg(lt_gcontext_t cx[static 1], const lt_grect_t r[static 1], const lt_gstyle_t style[static 1], u32 depth) {
	if (!(style->bg_color & 0xFF) || r->w < 0 || r->h < 0) {
		style = cx->curr_container->style;
	}
	if (!(style->bg_color & 0xFF) || r->w < 0 || r->h < 0) {
		return;
	}
	cx->draw_rect(cx->usr, r, style->bg_color, depth);
}

static
void draw_text(lt_gcontext_t cx[static 1], lt_vec2i_t pos, lstr_t str, const lt_gstyle_t style[static 1], u32 depth) {
	if (!(style->text_color & 0xFF)) {
		return;
	}
	lt_font_t* font = (style->font) ? style->font : cx->font;
	cx->draw_text(cx->usr, font, pos, str, style->text_color, depth);
}

static
void draw_icon(lt_gcontext_t cx[static 1], const lt_grect_t r[static 1], lt_gicon_t icon, const lt_gstyle_t style[static 1], u32 depth) {
	if (!(style->icon_color & 0xFF)) {
		return;
	}
	cx->draw_icon(cx->usr, r, style->icon_color, icon, depth);
}

static LT_INLINE
lt_vec2i_t box_size(lt_vec2i_t size, const lt_gstyle_t style[static 1]) {
	lt_vec2i_t ret = lt_vaddi(size, (style->border + style->padding) * 2);
	if (size.x < 0) ret.x = size.x;
	if (size.y < 0) ret.y = size.y;
	return ret;
}

static LT_INLINE
lt_vec2i_t box_offset(lt_vec2i_t pos, const lt_gstyle_t style[static 1]) {
	return lt_vaddi(pos, style->padding + style->border);
}

static LT_INLINE
i32 box_offset_single(i32 v, const lt_gstyle_t style[static 1]) {
	return v + style->padding + style->border;
}

static LT_INLINE
lt_vec2i_t str_size(lt_gcontext_t cx[static 1], lstr_t str, const lt_gstyle_t style[static 1]) {
	lt_font_t* font = (style->font) ? style->font : cx->font;
	return LT_VEC2I(cx->text_width(cx->usr, font, str), cx->text_height(cx->usr, font, str));
}

static
lt_vec2i_t str_box_size(lt_gcontext_t cx[static 1], lstr_t str, const lt_gstyle_t style[static 1]) {
	return box_size(str_size(cx, str, style), style);
}

void lt_gpanel_begin(lt_gcontext_t cx[static 1], lt_gpanel_t panel[static 1]) {
	lt_gstyle_t* style = (panel->style) ? panel->style: cx->panel_style;
	lt_grect_t r = reserve_space(cx, style->flags, LT_VEC2I(-1, 1));

	lt_gcontainer_t* c = lt_gpush_container(cx, style, &r);
}

void lt_gpanel_end(lt_gcontext_t cx[static 1]) {
	lt_gcontainer_t* c = cx->curr_container;

	draw_bg(cx, &c->bounds, c->style, c->depth);
	draw_border(cx, &c->bounds, c->style, c->depth + 1);

	lt_gpop_container(cx);
}

b8 lt_gheader(lt_gcontext_t cx[static 1], lt_gheader_t header[static 1]) {
	lt_gstyle_t* style = (header->style) ? header->style : cx->header_style;

	lt_vec2i_t text_size = str_size(cx, header->text, style);
	lt_grect_t r = reserve_space(cx, style->flags, box_size(LT_VEC2I(-1, text_size.y), style));

	b8 hovered = is_hovered(cx, r);
	if (hovered && style->hover_style) {
		style = style->hover_style;
	}
	if (hovered && (cx->mouse_state & LT_GUI_LMB_PRESSED)) {
		header->expanded = !header->expanded;
	}

	draw_bg(cx, &r, style, 2);
	draw_border(cx, &r, style, 3);

	lt_grect_t icon_r = { .pos = box_offset(r.pos, style), .size = LT_VEC2I(text_size.y, text_size.y) };
	draw_icon(cx, &icon_r, (header->expanded) ? LT_GICON_EXPANDED : LT_GICON_COLLAPSED, style, 3);

	draw_text(cx, LT_VEC2I(icon_r.x + icon_r.w, icon_r.y), header->text, style, 3);

	return header->expanded;
}

b8 lt_gbutton(lt_gcontext_t cx[static 1], const lt_gbutton_t button[static 1]) {
	lt_gstyle_t* style = (button->style) ? button->style : cx->button_style;

	lt_grect_t r = reserve_space(cx, style->flags, str_box_size(cx, button->text, style));
	b8 hovered = is_hovered(cx, r);
	if (hovered && style->hover_style) {
		style = style->hover_style;
	}

	draw_bg(cx, &r, style, 2);
	draw_border(cx, &r, style, 3);

	draw_text(cx, box_offset(r.pos, style), button->text, style, 3);

	return hovered && (cx->mouse_state & LT_GUI_LMB_PRESSED);
}

b8 lt_gcheckbox(lt_gcontext_t cx[static 1], lt_gcheckbox_t checkbox[static 1]) {
	lt_gstyle_t* style = (checkbox->style) ? checkbox->style : cx->checkbox_style;

	lt_vec2i_t text_size = str_size(cx, checkbox->text, style);
	lt_grect_t r = reserve_space(cx, style->flags, LT_VEC2I(text_size.x + text_size.y + style->spacing, text_size.y));

	b8 hovered = is_hovered(cx, r);
	if (hovered && style->hover_style) {
		style = style->hover_style;
	}
	if (hovered && (cx->mouse_state & LT_GUI_LMB_PRESSED)) {
		checkbox->checked = !checkbox->checked;
	}

	lt_grect_t box_r = {
		.pos = r.pos,
		.size = LT_VEC2I(text_size.y, text_size.y),
	};
	draw_bg(cx, &box_r, style, 2);
	draw_border(cx, &box_r, style, 3);
	if (checkbox->checked) {
		draw_icon(cx, &box_r, LT_GICON_CHECK, style, 3);
	}

	draw_text(cx, LT_VEC2I(box_r.x + box_r.w + style->spacing, box_r.y), checkbox->text, style, 3);

	return checkbox->checked;
}

#include <lt/time.h>

static
void generic_slider(lt_gcontext_t cx[static 1], lstr_t text, f64* normalized_marker_x, lt_gslider_t slider[static 1]) {
	lt_gstyle_t* style = (slider->style) ? slider->style : cx->slider_style;

	lt_vec2i_t text_size = str_size(cx, text, style);
	lt_vec2i_t bar_size = LT_VEC2I(300, 6);
	lt_vec2i_t container_size = LT_VEC2I(bar_size.x + text_size.x + style->spacing, lt_max(text_size.y, bar_size.y));

	lt_grect_t r = reserve_space(cx, style->flags, container_size);

	b8 hovered = is_hovered(cx, r);
	if (hovered && style->hover_style) {
		style = style->hover_style;
	}

	lt_grect_t bar_r = {
		.pos  = LT_VEC2I(r.pos.x, r.pos.y + (container_size.y >> 1) - (bar_size.y >> 1)),
		.size = bar_size,
	};
	draw_bg(cx, &bar_r, style, 2);
	draw_border(cx, &bar_r, style, 3);

	lt_grect_t mark_r = {
		.pos  = LT_VEC2I(r.pos.x + *normalized_marker_x * bar_size.x - (bar_size.y >> 1), r.pos.y),
		.size = LT_VEC2I(bar_size.y, container_size.y),
	};

	u32 old_bg = style->bg_color;
	style->bg_color = style->icon_color;
	draw_bg(cx, &mark_r, style, 4);
	draw_border(cx, &mark_r, style, 5);
	style->bg_color = old_bg;

	draw_text(cx, box_offset(LT_VEC2I(r.pos.x + bar_size.x + style->spacing, r.pos.y), style), text, style, 3);

	if (hovered && (cx->mouse_state & LT_GUI_LMB_DOWN) && bar_size.x) {
		*normalized_marker_x = lt_clamp((f64)(cx->mouse_x - r.pos.x) / (f64)bar_size.x, 0.0, 1.0);
	}
}

i64 lt_gslideri(lt_gcontext_t cx[static 1], lt_gslider_t slider[static 1]) {
	LT_ASSERT(slider->unit_str.len <= 32);
	char buf[64];
	lstr_t text = LSTR(buf, lt_sprintf(buf, "%iq%S", slider->vali, slider->unit_str));

	f64 wi = slider->maxi - slider->mini;
	f64 mark_x = wi ? (f64)(slider->vali - slider->mini) / wi : 0.0;

	generic_slider(cx, text, &mark_x, slider);

	return slider->vali = round(mark_x * wi + (f64)slider->mini);
}

f64 lt_gsliderf(lt_gcontext_t cx[static 1], lt_gslider_t slider[static 1]) {
	LT_ASSERT(slider->unit_str.len <= 32);
	char buf[64];
	lstr_t text = LSTR(buf, lt_sprintf(buf, "%iq%S", (i64)slider->valf, slider->unit_str)); // !! should show decimals

	f64 wi = slider->maxf - slider->minf;
	f64 mark_x = wi ? (slider->valf - slider->minf) / wi : 0.0;

	generic_slider(cx, text, &mark_x, slider);

	return slider->valf = mark_x * wi + slider->minf;
}

void lt_glabel(lt_gcontext_t cx[static 1], const lt_glabel_t label[static 1]) {
	lt_gstyle_t* style = (label->style) ? label->style : cx->label_style;

	lt_grect_t r = reserve_space(cx, style->flags, str_box_size(cx, label->text, style));

	draw_bg(cx, &r, style, 2);
	draw_border(cx, &r, style, 3);

	draw_text(cx, box_offset(r.pos, style), label->text, style, 3);
}

void lt_gtext(lt_gcontext_t cx[static 1], const lt_gtext_t text[static 1]) {
	lt_gstyle_t* style = (text->style) ? text->style : cx->text_style;

	lt_grect_t r = reserve_space(cx, style->flags, str_box_size(cx, text->text, style));

	draw_bg(cx, &r, style, 2);
	draw_border(cx, &r, style, 3);

	draw_text(cx, box_offset(r.pos, style), text->text, style, 3);
}

b8 lt_gtextbox(lt_gcontext_t cx[static 1], lt_gtextbox_t textbox[static 1]) {
	lt_gstyle_t* style = (textbox->style) ? textbox->style : cx->textbox_style;

	lstr_t text = lt_lsfroms(textbox->text_buffer);
	lt_vec2i_t text_size = str_size(cx, text, style);

	lt_grect_t r = reserve_space(cx, style->flags, box_size(LT_VEC2I(-1, text_size.y), style));

	draw_bg(cx, &r, style, 2);
	draw_border(cx, &r, style, 3);

	lt_vec2i_t text_pos = box_offset(r.pos, style);
	draw_text(cx, text_pos, text, style, 3);
	if (lt_time_msec() / 650 % 2 == 0)
		cx->draw_rect(cx->usr, &LT_GRECT(text_pos.x + text_size.x, text_pos.y, 2, text_size.y), style->text_color, 3);

	return 1;
}


