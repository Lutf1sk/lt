#pragma once

#include <lt2/common.h>

#define WEV_RESIZE         1
#define WEV_MOTION         2
#define WEV_BUTTON_PRESS   3
#define WEV_BUTTON_RELEASE 4
#define WEV_KEY_PRESS      5
#define WEV_KEY_RELEASE    6
#define WEV_QUIT           7

#define KEY_UNKNOWN   0
#define KEY_BACKSPACE 8
#define KEY_TAB       9
#define KEY_ENTER     10
#define KEY_ESCAPE    27
#define KEY_SPACE     32
#define KEY_DELETE    0x7F
#define KEY_LSHIFT    0x80
#define KEY_RSHIFT    0x81
#define KEY_LCTRL     0x82
#define KEY_RCTRL     0x83
#define KEY_LALT      0x84
#define KEY_RALT      0x85
#define KEY_LMETA     0x86
#define KEY_RMETA     0x87

#define KEY_PAUSE     0x90
#define KEY_CAPSLOCK  0x91
#define KEY_PGUP      0x92
#define KEY_PGDOWN    0x93
#define KEY_HOME      0x94
#define KEY_END       0x95
#define KEY_UP        0x96
#define KEY_LEFT      0x97
#define KEY_DOWN      0x98
#define KEY_RIGHT     0x99
#define KEY_PRTSCR    0x9A
#define KEY_INSERT    0x9B

#define KEY_F1        0xA1
#define KEY_F2        0xA2
#define KEY_F3        0xA3
#define KEY_F4        0xA4
#define KEY_F5        0xA5
#define KEY_F6        0xA6
#define KEY_F7        0xA7
#define KEY_F8        0xA8
#define KEY_F9        0xA9
#define KEY_F10       0xAA
#define KEY_F11       0xAB
#define KEY_F12       0xAC

typedef struct wevent {
	u8 type;
	union {
		struct {
			i32 width;
			i32 height;
		} resize;

		struct {
			i32 x;
			i32 y;
		} motion;

		struct {
			u8 code;
		} button;

		struct {
			u8 code;
		} key;
	};
} wevent_t;

extern i32 window_height;
extern i32 window_width;

extern f64 mouse_x;
extern f64 mouse_y;

void window_init(err* err);
void queue_wevent(wevent_t event);
usz poll_wevents(wevent_t* events, usz max_events);
void window_present();

void draw_rect(i32 x, i32 y, i32 w, i32 h, u32 color);
void fill_rect(i32 x, i32 y, i32 w, i32 h, u32 color);
void draw_line(i32 x, i32 y, i32 x2, i32 y2, u32 color);
void draw_vline(i32 x, i32 y, i32 y2, u32 color);
void draw_hline(i32 x, i32 y, i32 x2, u32 color);
void fill_circle(i32 x, i32 y, i32 r, u32 color);

void draw_text(ls str, i32 x, i32 y, u32 color);
void measure_text(ls str);

