#ifndef LT_WINDOW_DEF_H
#define LT_WINDOW_DEF_H 1

#include <lt/lt.h>
#include <lt/window.h>

#if defined(LT_UNIX)
#	define LT_X11 1
#elif defined(LT_WINDOWS)
#	define LT_WIN32 1
#endif

#if defined(LT_X11)
#	include <xcb/xcb.h>
#	include <X11/Xlib.h>
#	include <GL/glx.h>

extern xcb_connection_t* lt_conn;

typedef
struct lt_window {
	xcb_window_t window;
	xcb_gc_t gc;

	GLXContext glctx;
	Colormap clrmap;

	lt_wintype_t type;

	u32 scan_min;
	int* keytab;

	int pos_x, pos_y;
	int size_w, size_h;

	int mpos_x, mpos_y;

	b8 exposed;
	b8 closed;
	b8 key_press_map[LT_KEY_MAX];
	b8 old_key_press_map[LT_KEY_MAX];
} lt_window_t;

#elif defined(LT_WIN32)
#	define WIN32_LEAN_AND_MEAN 1
#	include <windows.h>

extern HINSTANCE lt_hinst;

typedef struct lt_window {
	HWND hwnd;
	WINDOWPLACEMENT wp;

	b8 fullscreen;

	int mpos_x, mpos_y;

	b8 closed;
	b8 exposed;
	b8 key_press_map[LT_KEY_MAX];
	b8 old_key_press_map[LT_KEY_MAX];
} lt_window_t;

#endif

#endif
