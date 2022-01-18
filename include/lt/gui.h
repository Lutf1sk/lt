#ifndef LT_GUI_H
#define LT_GUI_H 1

#include <lt/fwd.h>
#include <lt/lt.h>

#define LT_GUI_ICON_EXPANDED	0
#define LT_GUI_ICON_COLLAPSED	1
#define LT_GUI_ICON_CHECK		2
#define LT_GUI_ICON_MAX			3

#define LT_GUI_BORDER_OUTSET	1
#define LT_GUI_BORDER_INSET		2
#define LT_GUI_BORDER_TOGGLE	4
#define LT_GUI_ALIGN_RIGHT		8

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

	isz padding;
} lt_gui_cont_t;

#define LT_GUI_RECT(x, y, w, h) ((lt_gui_rect_t){ (x), (y), (w), (h) })

typedef void (*lt_gui_rect_callback_t)(void* usr, usz count, lt_gui_rect_t* r, u32* clr);
typedef void (*lt_gui_text_callback_t)(void* usr, i32 x, i32 y, lstr_t text, u32 clr);
typedef void (*lt_gui_icon_callback_t)(void* usr, usz icon, lt_gui_rect_t* r, u32 clr);
typedef void (*lt_gui_scissor_callback_t)(void* usr, lt_gui_rect_t* r);

typedef usz (*lt_gui_text_width_callback_t)(void* usr, lstr_t text);
typedef usz (*lt_gui_text_height_callback_t)(void* usr);

#define LT_GUI_MB1_PRESSED 1

typedef
struct lt_gui_ctx {
	usz cont_max;
	lt_gui_cont_t* conts;
	usz cont_top;

	void* user_data;

	lt_gui_rect_callback_t draw_rect;
	lt_gui_text_callback_t draw_text;
	lt_gui_icon_callback_t draw_icon;
	lt_gui_scissor_callback_t scissor;

	isz glyph_width;
	isz glyph_height;

	int mouse_x, mouse_y;
	u32 mouse_state, prev_mouse_state;
} lt_gui_ctx_t;

void lt_gui_ctx_init(lt_arena_t* arena, lt_gui_ctx_t* cx);

void lt_gui_begin(lt_gui_ctx_t* cx, isz w, isz h);
void lt_gui_end(lt_gui_ctx_t* cx);

lt_gui_cont_t* lt_gui_get_container(lt_gui_ctx_t* cx);

usz lt_gui_draw_border(lt_gui_rect_t* r, u32 clr, u32 flags, lt_gui_rect_t* out_r, u32* out_clr);

void lt_gui_panel_begin(lt_gui_ctx_t* cx, isz w, isz h, u32 flags);
void lt_gui_panel_end(lt_gui_ctx_t* cx);

void lt_gui_row(lt_gui_ctx_t* cx, usz cols);

void lt_gui_label(lt_gui_ctx_t* cx, lstr_t text, u32 flags);

b8 lt_gui_button(lt_gui_ctx_t* cx, lstr_t text, u32 flags);

b8 lt_gui_expandable(lt_gui_ctx_t* cx, lstr_t text, b8* expanded, u32 flags);

void lt_gui_vspace(lt_gui_ctx_t* cx, usz space, u32 flags);
void lt_gui_hspace(lt_gui_ctx_t* cx, usz space, u32 flags);

b8 lt_gui_dropdown_begin(lt_gui_ctx_t* cx, lstr_t text, isz w, isz h, u32* state, u32 flags);
void lt_gui_dropdown_end(lt_gui_ctx_t* cx);

b8 lt_gui_checkbox(lt_gui_ctx_t* cx, lstr_t text, b8* state, u32 flags);

#endif
