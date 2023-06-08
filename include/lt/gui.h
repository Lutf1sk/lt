#ifndef LT_GUI_H
#define LT_GUI_H 1

#include <lt/fwd.h>
#include <lt/err.h>

#define LT_GUI_ICON_EXPANDED	0
#define LT_GUI_ICON_COLLAPSED	1
#define LT_GUI_ICON_CHECK		2
#define LT_GUI_ICON_MAX			3

#define LT_GUI_BORDER_OUTSET	1
#define LT_GUI_BORDER_INSET		2
#define LT_GUI_BORDER_TOGGLE	4
#define LT_GUI_ALIGN_RIGHT		8
#define LT_GUI_GROW_X			16

typedef
struct lt_gui_rect {
	i32 x, y, w, h;
} lt_gui_rect_t;

typedef
struct lt_gui_cont {
	lt_gui_rect_t r; // Initial
	lt_gui_rect_t a; // Available
	isz ymax;
	usz cols;
	u32 depth;

	isz padding, spacing;
} lt_gui_cont_t;

#define LT_GUI_RECT(x, y, w, h) ((lt_gui_rect_t){ (x), (y), (w), (h) })

typedef void (*lt_gui_rect_callback_t)(void* usr, lt_gui_rect_t* r, u32 clr, u32 depth);
typedef void (*lt_gui_text_callback_t)(void* usr, i32 x, i32 y, lstr_t str, u32 clr, u32 depth);
typedef void (*lt_gui_icon_callback_t)(void* usr, lt_gui_rect_t* r, u32 clr, usz icon, u32 depth);
typedef void (*lt_gui_scissor_callback_t)(void* usr, lt_gui_rect_t* r);
typedef usz (*lt_gui_text_width_callback_t)(void* usr, lstr_t text);

typedef
struct lt_gui_style {
	u32 panel_bg_clr;
	u32 panel_border_clr;

	u32 text_clr;
	u32 border_clr;

	u32 ctrl_bg_clr;
	u32 ctrl_text_clr;
	u32 ctrl_hover_bg_clr;
	u32 ctrl_hover_text_clr;

	u32 checkbox_bg_clr;
	u32 checkbox_hover_bg_clr;
	u32 checkbox_icon_clr;

	u16 button_hpad;

	u16 padding;
	u16 spacing;
	u16 border;
} lt_gui_style_t;

#define LT_GUI_MB1_PRESSED 1

typedef
struct lt_gui_ctx {
	lt_gui_style_t* style;

	usz cont_max;
	lt_gui_cont_t* conts;
	usz cont_top;

	void* user_data;
	lt_gui_rect_callback_t draw_rect;
	lt_gui_text_callback_t draw_text;
	lt_gui_icon_callback_t draw_icon;
	lt_gui_scissor_callback_t scissor;
	lt_gui_text_width_callback_t text_width;

	isz glyph_height;

	int mouse_x, mouse_y;
	u32 mouse_state, prev_mouse_state;

	b8 hovered;
} lt_gui_ctx_t;

typedef
struct lt_gui_scroll_state {
	u32 vscroll;
} lt_gui_scroll_state_t;

typedef
struct lt_gui_textbox_state {
	char* buf;
	usz maxlen;
	u8 selected;
} lt_gui_textbox_state_t;

extern lt_gui_style_t* lt_gui_default_style;

lt_err_t lt_gui_ctx_init(lt_gui_ctx_t* cx, lt_alloc_t* alloc);
void lt_gui_ctx_free(lt_gui_ctx_t* cx, lt_alloc_t* alloc);

void lt_gui_begin(lt_gui_ctx_t* cx, isz x, isz y, isz w, isz h);
void lt_gui_end(lt_gui_ctx_t* cx);

lt_gui_cont_t* lt_gui_get_container(lt_gui_ctx_t* cx);

void lt_gui_draw_border(lt_gui_ctx_t* cx, lt_gui_rect_t* r, u32 clr, u32 flags);
void lt_gui_draw_rect(lt_gui_ctx_t* cx, lt_gui_rect_t* r, u32 clr);
void lt_gui_draw_icon(lt_gui_ctx_t* cx, lt_gui_rect_t* r, u32 clr, u32 icon);
void lt_gui_draw_text(lt_gui_ctx_t* cx, i32 x, i32 y, lstr_t str, u32 clr);

void lt_gui_panel_begin(lt_gui_ctx_t* cx, isz w, isz h, u32 flags);
void lt_gui_panel_end(lt_gui_ctx_t* cx);

void lt_gui_row(lt_gui_ctx_t* cx, usz cols);

void lt_gui_label(lt_gui_ctx_t* cx, lstr_t text, u32 flags);
void lt_gui_text(lt_gui_ctx_t* cx, lstr_t text, u32 flags);

b8 lt_gui_textbox(lt_gui_ctx_t* cx, isz ew, isz eh, lt_gui_textbox_state_t* state, u32 flags);

b8 lt_gui_button(lt_gui_ctx_t* cx, lstr_t text, u32 flags);

b8 lt_gui_expandable(lt_gui_ctx_t* cx, lstr_t text, b8* expanded, u32 flags);

void lt_gui_vspace(lt_gui_ctx_t* cx, usz space, u32 flags);
void lt_gui_hspace(lt_gui_ctx_t* cx, usz space, u32 flags);

b8 lt_gui_dropdown_begin(lt_gui_ctx_t* cx, lstr_t text, isz w, isz h, u32* state, u32 flags);
void lt_gui_dropdown_end(lt_gui_ctx_t* cx);

b8 lt_gui_checkbox(lt_gui_ctx_t* cx, lstr_t text, b8* state, u32 flags);

#endif
