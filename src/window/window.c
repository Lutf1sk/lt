#include "window_def.h"

#if defined(LT_X11)

b8 lt_window_closed(const lt_window_t* win) {
	return win->closed;
}

void lt_window_mouse_pos(const lt_window_t* win, int x[static 1], int y[static 1]) {
	*x = win->mpos_x;
	*y = win->mpos_y;
}

b8 lt_window_key_pressed(const lt_window_t* win, lt_keycode_t keycode) {
	return win->key_press_map[keycode] && !win->old_key_press_map[keycode];
}

b8 lt_window_key_released(const lt_window_t* win, lt_keycode_t keycode) {
	return !win->key_press_map[keycode] && win->old_key_press_map[keycode];
}

b8 lt_window_key_held(const lt_window_t* win, lt_keycode_t keycode) {
	return win->key_press_map[keycode] != 0;
}

#endif
