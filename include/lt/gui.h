#ifndef LT_GUI_H
#define LT_GUI_H 1

#include <lt/fwd.h>
#include <lt/lt.h>

#define LT_GUI_GROW_DOWN 0
#define LT_GUI_GROW_RIGHT 1

#define LT_GUI_ICON_EXPANDED 0
#define LT_GUI_ICON_COLLAPSED 1
#define LT_GUI_ICON_MAX 2

typedef
struct lt_gui_cont {
	isz x, y, w, h;
	isz xrem, yrem;
	usz cols;
	isz ymax;
} lt_gui_cont_t;

typedef void (*lt_gui_rect_callback_t)(void* usr, isz x, isz y, isz w, isz h, u32 clr);
typedef void (*lt_gui_text_callback_t)(void* usr, isz x, isz y, lstr_t text, u32 clr);
typedef void (*lt_gui_icon_callback_t)(void* usr, usz icon, isz x, isz y, isz w, isz h, u32 clr);
typedef void (*lt_gui_scissor_callback_t)(void* usr, isz x, isz y, isz w, isz h);

typedef usz (*lt_gui_text_width_callback_t)(void* usr, lstr_t text);
typedef usz (*lt_gui_text_height_callback_t)(void* usr);

#define LT_GUI_MB1_PRESSED 1

typedef
struct lt_gui_ctx {
	usz cont_max;
	lt_gui_cont_t* conts;
	usz cont_top;

	void* user_data;

	lt_gui_rect_callback_t draw_filled_rect;
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

void lt_gui_panel_begin(lt_gui_ctx_t* cx, isz w, isz h);
void lt_gui_panel_end(lt_gui_ctx_t* cx);

void lt_gui_row(lt_gui_ctx_t* cx, usz cols);

void lt_gui_label(lt_gui_ctx_t* cx, lstr_t text);

u8 lt_gui_button(lt_gui_ctx_t* cx, lstr_t text);

u8 lt_gui_expandable(lt_gui_ctx_t* cx, lstr_t text, b8* expanded);

void lt_gui_vspace(lt_gui_ctx_t* cx, usz space);
void lt_gui_hspace(lt_gui_ctx_t* cx, usz space);

#endif
