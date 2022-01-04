#include "window_def.h"

b8 lt_window_exposed(lt_window_t* win) {
	return win->exposed;
}

b8 lt_window_closed(lt_window_t* win) {
	return win->closed;
}

b8 lt_window_key_pressed(lt_window_t* win, lt_keycode_t keycode) {
	return win->key_press_map[keycode] && !win->old_key_press_map[keycode];
}

b8 lt_window_key_released(lt_window_t* win, lt_keycode_t keycode) {
	return !win->key_press_map[keycode] && win->old_key_press_map[keycode];
}

b8 lt_window_key_held(lt_window_t* win, lt_keycode_t keycode) {
	return win->key_press_map[keycode] != 0;
}

