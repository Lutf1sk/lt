#ifndef LT_GUI_H
#define LT_GUI_H 1

#include <lt/fwd.h>
#include <lt/err.h>

#include <lt/linalg.h>

// ----- rectangle

typedef
union lt_grect {
	struct { i32 x, y, w, h; };
	struct { lt_vec2i_t pos, size; };
	i32 data[4];
} lt_grect_t;

#define LT_GRECT(x, y, w, h) ((lt_grect_t){ .data = { (x), (y), (w), (h) } })
#define LT_GRECTV(pos_, size_) ((lt_grect_t){ .pos = (pos_), .size = (size_) })

// ----- default icons

typedef
enum lt_gicon {
	LT_GICON_EXPANDED  = 0,
	LT_GICON_COLLAPSED = 1,
	LT_GICON_CHECK     = 2,
	LT_GICON_MAX       = 3,
} lt_gicon_t;

// ----- styles

typedef
enum lt_gstyle_flag {
	LT_GSTYLE_OUTSET   = 0x0001,
	LT_GSTYLE_INSET    = 0x0002,
	LT_GSTYLE_RALIGN   = 0x0004,
	LT_GSTYLE_FILLW    = 0x0008,
	LT_GSTYLE_FILLH    = 0x0010,
	LT_GSTYLE_UNERLINE = 0x0020,
	LT_GSTYLE_FIXEDW   = 0x0800,
	LT_GSTYLE_FIXEDH   = 0x1000,
} lt_gstyle_flag_t;

typedef
enum lt_galign {
	LT_GALIGN_LEFT   = 0,
	LT_GALIGN_RIGHT  = 1,
	LT_GALIGN_CENTER = 2,
} lt_galign_t;

typedef
struct lt_gstyle {
	lt_gstyle_flag_t flags;

	u32 padding;
	u32 spacing;

	u32 border;
	u32 border_color;

	lt_font_t* font;
	u32 text_color;
	lt_galign_t text_align;

	u32 bg_color;

	u32 icon_color;

	struct lt_gstyle* hover_style;
} lt_gstyle_t;

// ----- gui context

typedef void (*lt_grect_fn_t)(void* usr, const lt_grect_t r[static 1], u32 clr, u32 depth);
typedef void (*lt_gtext_fn_t)(void* usr, lt_font_t* font, lt_vec2i_t pos, lstr_t str, u32 clr, u32 depth);
typedef void (*lt_gicon_fn_t)(void* usr, const lt_grect_t r[static 1], u32 clr, u32 icon, u32 depth);
typedef void (*lt_gscissor_fn_t)(void* usr, const lt_grect_t r[static 1]);
typedef usz (*lt_gtextw_fn_t)(void* usr, lt_font_t* font, lstr_t text);
typedef usz (*lt_gtexth_fn_t)(void* usr, lt_font_t* font, lstr_t text);

#define LT_GUI_LMB_DOWN 0x01
#define LT_GUI_RMB_DOWN 0x02
#define LT_GUI_LMB_PRESSED 0x04
#define LT_GUI_RMB_PRESSED 0x08

typedef
struct lt_gcontainer {
	const lt_gstyle_t* style;
	lt_grect_t bounds;
	lt_grect_t free;
	u32 cols;
	u32 col;
	u32 row;
	u32 depth;
	b8 full;
} lt_gcontainer_t;

typedef
struct lt_gcontext {
	lt_alloc_t* alloc;

	lt_gstyle_t* panel_style;
	lt_gstyle_t* panel_style_dark;
	lt_gstyle_t* panel_style_bright;
	lt_gstyle_t* header_style;
	lt_gstyle_t* button_style;
	lt_gstyle_t* text_style;
	lt_gstyle_t* label_style;
	lt_gstyle_t* checkbox_style;
	lt_gstyle_t* textbox_style;
	lt_gstyle_t* slider_style;

	lt_gcontainer_t* containers;
	usz max_container_count;
	lt_gcontainer_t* curr_container;

	void* usr;
	lt_grect_fn_t    draw_rect;
	lt_gtext_fn_t    draw_text;
	lt_gicon_fn_t    draw_icon;
	lt_gscissor_fn_t scissor;
	lt_gtextw_fn_t   text_width;
	lt_gtexth_fn_t   text_height;

	lt_font_t* font;

	int mouse_x, mouse_y;
	u32 mouse_state;
} lt_gcontext_t;

lt_err_t lt_ginit(lt_gcontext_t cx[static 1]);
void lt_gterminate(lt_gcontext_t cx[static 1]);

void lt_gcontext_begin(lt_gcontext_t cx[static 1], const lt_grect_t* bounds);
void lt_gcontext_end(lt_gcontext_t cx[static 1]);

lt_gcontainer_t* lt_gpush_container(lt_gcontext_t cx[static 1], const lt_gstyle_t style[static 1], const lt_grect_t bounds[static 1]);
void lt_gpop_container(lt_gcontext_t cx[static 1]);

// ----- default controls

typedef
struct lt_gpanel {
	lt_gstyle_t* style;
	u32 scroll_pos;
} lt_gpanel_t;

void lt_gpanel_begin(lt_gcontext_t cx[static 1], lt_gpanel_t panel[static 1]);
void lt_gpanel_end(lt_gcontext_t cx[static 1]);
#define lt_gpanel(cx, panel) for (b8 __run = (lt_gpanel_begin((cx), (panel)), 1); __run; lt_gpanel_end(cx), __run = 0)

typedef
struct lt_gheader {
	lt_gstyle_t* style;
	b8 expanded;
	lstr_t text;
} lt_gheader_t;

b8 lt_gheader(lt_gcontext_t cx[static 1], lt_gheader_t header[static 1]);

typedef
struct lt_gbutton {
	lt_gstyle_t* style;
	lstr_t text;
} lt_gbutton_t;

b8 lt_gbutton(lt_gcontext_t cx[static 1], const lt_gbutton_t button[static 1]);

#define LT_GBUTTONC(text_) (&(lt_gbutton_t) { .text = CLSTR(text_) })

typedef
struct lt_gcheckbox {
	lt_gstyle_t* style;
	lstr_t text;
	b8 checked;
} lt_gcheckbox_t;

b8 lt_gcheckbox(lt_gcontext_t cx[static 1], lt_gcheckbox_t checkbox[static 1]);

typedef
struct lt_gslider {
	lt_gstyle_t* style;
	union {
		struct { i64 mini, maxi, vali; };
		struct { f64 minf, maxf, valf; };
	};
	lstr_t unit_str;
} lt_gslider_t;

i64 lt_gslideri(lt_gcontext_t cs[static 1], lt_gslider_t slider[static 1]);
f64 lt_gsliderf(lt_gcontext_t cs[static 1], lt_gslider_t slider[static 1]);

typedef
struct lt_glabel {
	lt_gstyle_t* style;
	lstr_t text;
} lt_glabel_t;

void lt_glabel(lt_gcontext_t cx[static 1], const lt_glabel_t label[static 1]);

#define LT_GLABELC(text_) (&(lt_glabel_t) { .text = CLSTR(text_) })

typedef
struct lt_gtext {
	lt_gstyle_t* style;
	lstr_t text;
} lt_gtext_t;

void lt_gtext(lt_gcontext_t cx[static 1], const lt_gtext_t text[static 1]);

#define LT_GTEXTC(text_) (&(lt_gtext_t) { .text = CLSTR(text_) })

typedef
struct lt_gtextbox {
	lt_gstyle_t* style;
	usz max_len;
	char* text_buffer;
	lstr_t placeholder;
} lt_gtextbox_t;

b8 lt_gtextbox(lt_gcontext_t cx[static 1], lt_gtextbox_t textbox[static 1]);

#endif
