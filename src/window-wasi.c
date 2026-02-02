#include <lt2/common.h>

#ifdef ON_WASI
#	include <lt2/window.h>
#	include <lt2/log.h>

WASM_IMPORT("lt", "get_canvas_width")
i32 js_get_canvas_width();

WASM_IMPORT("lt", "get_canvas_height")
i32 js_get_canvas_height();


WASM_IMPORT("lt", "get_text_height")
void js_get_text_height();


WASM_IMPORT("lt", "draw_rect")
void js_draw_rect(i32 x, i32 y, i32 w, i32 h, u32 color);

void draw_rect(i32 x, i32 y, i32 w, i32 h, u32 color) {
	js_draw_rect(x, y, w, h, color);
}


WASM_IMPORT("lt", "fill_rect")
void js_fill_rect(i32 x, i32 y, i32 w, i32 h, u32 color);

void fill_rect(i32 x, i32 y, i32 w, i32 h, u32 color) {
	js_fill_rect(x, y, w, h, color);
}


WASM_IMPORT("lt", "draw_line")
void js_draw_line(i32 x, i32 y, i32 x2, i32 y2, u32 color);

void draw_line(i32 x1, i32 y1, i32 x2, i32 y2, u32 color) {
	js_draw_line(x1, y1, x2, y2, color);
}


WASM_IMPORT("lt", "fill_circle")
void js_fill_circle(i32 x, i32 y, i32 r, u32 color);

void fill_circle(i32 x, i32 y, i32 r, u32 color) {
	js_fill_circle(x, y, r, color);
}


WASM_IMPORT("lt", "draw_text")
void js_draw_text(const u8* ptr, usz size, i32 x, i32 y, u32 color);

void draw_text(ls str, i32 x, i32 y, u32 color) {
	js_draw_text(str.ptr, str.size, x, y, color);
}

WASM_IMPORT("lt", "measure_text")
void js_measure_text(const u8* ptr, usz size);

void measure_text(ls str) {
	js_measure_text(str.ptr, str.size);
}


void window_init(err* err) {
	window_width  = js_get_canvas_width();
	window_height = js_get_canvas_height();
}

void platform_poll_wevents() {
	// --
}

void window_present() {
	// --
}


void queue_resize(i32 w, i32 h) {
	window_width  = w;
	window_height = h;
	queue_wevent((wevent_t) {
		.type = WEV_RESIZE,

		.resize.width  = w,
		.resize.height = h,
	});
}

void queue_motion(i32 x, i32 y) {
	mouse_x = x;
	mouse_y = y;

	queue_wevent((wevent_t) {
		.type = WEV_MOTION,

		.motion.x = x,
		.motion.y = y,
	});
}

void queue_button_press(u8 button) {
	queue_wevent((wevent_t) {
		.type = WEV_BUTTON_PRESS,

		.button.code = button,
	});
}

void queue_button_release(u8 button) {
	queue_wevent((wevent_t) {
		.type = WEV_BUTTON_RELEASE,

		.button.code = button,
	});
}

void queue_key_press(u32 key) {
	queue_wevent((wevent_t) {
		.type = WEV_KEY_PRESS,

		.key.code = key,
	});
}

void queue_key_release(u32 key) {
	queue_wevent((wevent_t) {
		.type = WEV_KEY_RELEASE,

		.key.code = key,
	});
}

#endif // ON_WASI

