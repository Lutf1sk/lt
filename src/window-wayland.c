#include <lt2/common.h>

#ifdef WAYLAND
#	include <wayland-client-protocol.h>
#	include <wayland-client-core.h>
#	include <wayland-util.h>

#	include <lt2/window.h>
#	include <lt2/time.h>
#	include <lt2/log.h>
#	include <lt2/pixbuf.h>
#	include <lt2/str.h>

#	include <wayland-client.h>
#	include <lt2/wayland/xdg-shell-client.h>

#	include <string.h>
#	include <time.h>
#	include <errno.h>

#	ifndef _POSIX_C_SOURCE
#		define _POSIX_C_SOURCE 200112L
#	endif
#	include <fcntl.h>
#	include <unistd.h>
#	include <sys/mman.h>


typedef struct window_state {
	struct wl_surface*   wl_surface;
	struct xdg_surface*  xdg_surface;
	struct xdg_toplevel* xdg_toplevel;
	struct wl_buffer*    buffers[2];
	u32* buffer_data;
	pixbuf_t pb;
	u8 active_buffer;
	b8 fullscreen;
	b8 maximized;
} window_state;

static window_state win = {0};


static struct wl_shm*        shm;
static struct wl_compositor* compositor;
static struct xdg_wm_base*   xdg_wm_base;
static struct wl_display*    display;
static struct wl_registry*   registry;
static struct wl_seat*       seat;
static struct wl_pointer*    pointer;
static struct wl_keyboard*   keyboard;
static struct wl_output*     output;

static
void xdg_wm_base_ping(void* userdata, struct xdg_wm_base* xdg_wm_base, u32 serial) {
	xdg_wm_base_pong(xdg_wm_base, serial);
}

static
struct xdg_wm_base_listener xdg_wm_base_listener = {
	.ping = xdg_wm_base_ping,
};

void recreate_buffer(i32 width, i32 height) {
	if (win.pb.data && window_width && window_height) {
		munmap(win.pb.data, window_width * window_height * sizeof(u32) * 2);
		win.pb.data = NULL;
		if (win.buffers[0]) {
			wl_buffer_destroy(win.buffers[0]);
			win.buffers[0] = NULL;
		}
		if (win.buffers[1]) {
			wl_buffer_destroy(win.buffers[1]);
			win.buffers[1] = NULL;
		}
	}

	window_width  = width;
	window_height = height;

	if (!window_width || !window_height) {
		win.pb.width  = width;
		win.pb.height = height;
		return;
	}

	llogf(NULL, LOG_INFO, "resizing to {u32}x{u32}\n", width, height);

	const usz buf_size = window_width * window_height * sizeof(u32);
	const usz pool_size = buf_size * 2;

	constexpr usz max_attempts = 10;

	int fd;
	char path[512];
	u64 t = time(NULL);
	for (usz i = 0; i < max_attempts; ++i) {
		lsprintf(lls(path, sizeof(path)), "/lt2-wl-shm-{u64}{char}", t * 10 + i, 0); // !! should be randomized

		fd = shm_open(path, O_RDWR | O_CREAT | O_EXCL, 0x0600);
		if (fd >= 0)
			break;
		if (errno == EEXIST && i != max_attempts - 1)
			continue;
		throw_errno(err_fail);
		return;
	}
	shm_unlink(path);

	if (ftruncate(fd, pool_size) < 0) {
		throw_errno(err_fail);
		return;
	}

	u32* data = mmap(NULL, pool_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
	if (data == MAP_FAILED) {
		throw_errno(err_fail);
		return;
	}

	struct wl_shm_pool* pool = wl_shm_create_pool(shm, fd, pool_size);
	if (!pool) {
		throw(err_fail, ERR_UNKNOWN, "wl_shm_create_pool() failed");
		return;
	}
	win.buffers[0] = wl_shm_pool_create_buffer(pool, 0,        window_width, window_height, window_width * sizeof(u32), WL_SHM_FORMAT_XRGB8888);
	win.buffers[1] = wl_shm_pool_create_buffer(pool, buf_size, window_width, window_height, window_width * sizeof(u32), WL_SHM_FORMAT_XRGB8888);
	win.buffer_data = data;
	wl_shm_pool_destroy(pool);
	close(fd);

	win.pb = (pixbuf_t) {
		.width  = window_width,
		.height = window_height,
		.data   = data,
	};
}

static
void xdg_toplevel_configure(void* userdata, struct xdg_toplevel* toplevel, i32 width, i32 height, struct wl_array* states) {
	llogf(NULL, LOG_DEBUG, "got configure event for toplevel {void*}", toplevel);

	if (width == window_width && height == window_height)
		return;

	win.fullscreen = 0;
	win.maximized  = 0;
	u32* state;
	wl_array_for_each(state, states) {
		if (*state == XDG_TOPLEVEL_STATE_FULLSCREEN)
			win.fullscreen = 1;
		else if (*state == XDG_TOPLEVEL_STATE_MAXIMIZED)
			win.maximized = 1;
	}

	recreate_buffer(width, height);

	queue_wevent((wevent_t) {
		.type = WEV_RESIZE,

		.resize.width  = window_width,
		.resize.height = window_height,
	});
}

static
void xdg_toplevel_close(void* userdata, struct xdg_toplevel* xdg_toplevel) {
	queue_wevent((wevent_t) {
		.type = WEV_QUIT
	});
}

static
struct xdg_toplevel_listener xdg_toplevel_listener = {
	.configure = xdg_toplevel_configure,
	.close     = xdg_toplevel_close,
};


static
void xdg_surface_configure(void* userdata, struct xdg_surface* xdg_surface, u32 serial) {
	window_state* win = userdata;

	llogf(NULL, LOG_DEBUG, "configuring surface {void*}", xdg_surface);

	wl_surface_attach(win->wl_surface, win->buffers[win->active_buffer], 0, 0);
	xdg_surface_ack_configure(xdg_surface, serial);
	wl_surface_commit(win->wl_surface);
}
static
struct xdg_surface_listener xdg_surface_listener = {
	.configure = xdg_surface_configure,
};


static
void pointer_enter(void* userdata, struct wl_pointer* pointer, u32 time, struct wl_surface* surface, wl_fixed_t surface_x, wl_fixed_t surface_y) {
	mouse_x = wl_fixed_to_double(surface_x);
	mouse_y = wl_fixed_to_double(surface_y);

	queue_wevent((wevent_t) {
		.type = WEV_MOTION,

		.motion.x = mouse_x,
		.motion.y = mouse_y,
	});
}

static
void pointer_leave(void* userdata, struct wl_pointer* pointer, u32 time, struct wl_surface* surface) {

}

static
void pointer_motion(void* userdata, struct wl_pointer* pointer, u32 time, wl_fixed_t x, wl_fixed_t y) {
	mouse_x = wl_fixed_to_double(x);
	mouse_y = wl_fixed_to_double(y);

	queue_wevent((wevent_t) {
		.type = WEV_MOTION,

		.motion.x = mouse_x,
		.motion.y = mouse_y,
	});
}

#include <linux/input-event-codes.h>

static
void pointer_button(void* userdata, struct wl_pointer* pointer, u32 serial, u32 time, u32 button, u32 state) {
	u8 type;
	switch (state) {
	case WL_POINTER_BUTTON_STATE_PRESSED:  type = WEV_BUTTON_PRESS;   break;
	case WL_POINTER_BUTTON_STATE_RELEASED: type = WEV_BUTTON_RELEASE; break;
	default:
		llogf(NULL, LOG_WARN, "unknown wayland pointer button state {u32}", state);
		return;
	}

	u8 mapped_button = 0;
	switch (button) {
	case BTN_LEFT:    mapped_button = 1; break;
	case BTN_RIGHT:   mapped_button = 2; break;
	case BTN_MIDDLE:  mapped_button = 3; break;
	case BTN_SIDE:    mapped_button = 4; break;
	case BTN_EXTRA:   mapped_button = 5; break;
	case BTN_FORWARD: mapped_button = 6; break;
	case BTN_BACK:    mapped_button = 7; break;
	}

	queue_wevent((wevent_t) {
		.type        = type,

		.button.code = mapped_button
	});
}

static
void pointer_axis(void* userdata, struct wl_pointer* pointer, u32 time, u32 axis, wl_fixed_t value) {

}

static
void pointer_axis_source(void* userdata, struct wl_pointer* pointer, u32 axis_source) {

}

static
void pointer_axis_stop(void* userdata, struct wl_pointer* pointer, u32 time, u32 axis) {

}

static
void pointer_frame(void* userdata, struct wl_pointer* pointer) {

}

static
struct wl_pointer_listener pointer_listener = {
	.enter       = pointer_enter,
	.leave       = pointer_leave,
	.motion      = pointer_motion,
	.button      = pointer_button,
	.axis        = pointer_axis,
	.axis_source = pointer_axis_source,
	.axis_stop   = pointer_axis_stop,
	.frame       = pointer_frame,
};

static
void keyboard_keymap(void* userdata, struct wl_keyboard* keyboard, u32 format, i32 fd, u32 size) {
	close(fd);
}

static
void keyboard_enter(void* userdata, struct wl_keyboard* keyboard, u32 serial, struct wl_surface* surface, struct wl_array* keys) {

}

static
void keyboard_leave(void* userdata, struct wl_keyboard* keyboard, u32 serial, struct wl_surface* surface) {

}

static u32 keymap[256] = {
	[1]  = KEY_ESCAPE,
	[2]  = '1',
	[3]  = '2',
	[4]  = '3',
	[5]  = '4',
	[6]  = '5',
	[7]  = '6',
	[8]  = '7',
	[9]  = '8',
	[10] = '9',
	[11] = '0',
	[12] = '-',
	[13] = '=',
	[14] = '\b',
	[15] = '\t',
	[16] = 'Q',
	[17] = 'W',
	[18] = 'E',
	[19] = 'R',
	[20] = 'T',
	[21] = 'Y',
	[22] = 'U',
	[23] = 'I',
	[24] = 'O',
	[25] = 'P',
	[26] = '[',
	[27] = ']',
	[28] = '\n',
	[29] = KEY_LCTRL,
	[30] = 'A',
	[31] = 'S',
	[32] = 'D',
	[33] = 'F',
	[34] = 'G',
	[35] = 'H',
	[36] = 'J',
	[37] = 'K',
	[38] = 'L',
	[39] = ';',
	[40] = '\'',
	[41] = '`',
	[42] = KEY_LSHIFT,
	[43] = '\\',
	[44] = 'Z',
	[45] = 'X',
	[46] = 'C',
	[47] = 'V',
	[48] = 'B',
	[49] = 'N',
	[50] = 'M',
	[51] = ',',
	[52] = '.',
	[53] = '/',
	[54] = KEY_RSHIFT,

	[56] = KEY_LALT,
	[57] = ' ',
	[58] = KEY_CAPSLOCK,

	[59] = KEY_F1,
	[60] = KEY_F2,
	[61] = KEY_F3,
	[62] = KEY_F4,
	[63] = KEY_F5,
	[64] = KEY_F6,
	[65] = KEY_F7,
	[66] = KEY_F8,
	[67] = KEY_F9,
	[68] = KEY_F10,

	[87] = KEY_F11,
	[88] = KEY_F12,

	[97] = KEY_RCTRL,
	[100] = KEY_RALT,

	[102] = KEY_HOME,
	[103] = KEY_UP,
	[104] = KEY_PGUP,
	[105] = KEY_LEFT,
	[106] = KEY_RIGHT,
	[107] = KEY_END,
	[108] = KEY_DOWN,
	[109] = KEY_PGDOWN,
	[110] = KEY_INSERT,

	[125] = KEY_LMETA,
	[126] = KEY_RMETA,
};

static
void keyboard_key(void* userdata, struct wl_keyboard* keyboard, u32 serial, u32 time, u32 key, u32 state) {
	u8 type;
	switch (state) {
	case WL_KEYBOARD_KEY_STATE_PRESSED:  type = WEV_KEY_PRESS;   break;
	case WL_KEYBOARD_KEY_STATE_RELEASED: type = WEV_KEY_RELEASE; break;
	default:
		llogf(NULL, LOG_WARN, "unknown wayland key state {u32}", state);
		return;
	}

	u32 keycode = 0;
	if (key < 256)
		keycode = keymap[key];
	if (!keycode)
		llogf(NULL, LOG_WARN, "got event for unmapped evdev keycode {u32}", key);

	queue_wevent((wevent_t) {
		.type = type,

		.key.code = keycode,
	});
}

static
void keyboard_modifiers(void* userdata, struct wl_keyboard* keyboard, u32 serial, u32 mods_depressed, u32 mods_latched, u32 mods_locked, u32 group) {

}

static
void keyboard_repeat_info(void* userdata, struct wl_keyboard* keyboard, i32 rate, i32 delay) {

}

static struct wl_keyboard_listener keyboard_listener = {
	.keymap      = keyboard_keymap,
	.enter       = keyboard_enter,
	.leave       = keyboard_leave,
	.key         = keyboard_key,
	.modifiers   = keyboard_modifiers,
	.repeat_info = keyboard_repeat_info,
};

static
void seat_capabilities(void* userdata, struct wl_seat* seat, u32 capabilities) {
	llogf(NULL, LOG_DEBUG, "got seat capabilities '{u32}'", capabilities);

	if (pointer) {
		wl_pointer_destroy(pointer);
		pointer = NULL;
	}
	if (keyboard) {
		wl_keyboard_destroy(keyboard);
		keyboard = NULL;
	}

	if (capabilities & WL_SEAT_CAPABILITY_POINTER) {
		pointer = wl_seat_get_pointer(seat);
		wl_pointer_add_listener(pointer, &pointer_listener, NULL);
	}

	if (capabilities & WL_SEAT_CAPABILITY_KEYBOARD) {
		keyboard = wl_seat_get_keyboard(seat);
		wl_keyboard_add_listener(keyboard, &keyboard_listener, NULL);
	}
}

static
void seat_name(void* userdata, struct wl_seat*, const char* name) {
	llogf(NULL, LOG_DEBUG, "got seat name '{char*}'", name);
}

static
struct wl_seat_listener seat_listener = {
	.capabilities = seat_capabilities,
	.name         = seat_name,
};

static
void output_description(void* userdata, struct wl_output* output, const char* description) {
	llogf(NULL, LOG_DEBUG, "got output description '{char*}'", description);
}

static
void output_done(void* userdata, struct wl_output* output) {
	llogf(NULL, LOG_DEBUG, "got output done event");
}

static
void output_geometry(void* userdata, struct wl_output* output, i32 x, i32 y, i32 phys_width, i32 phys_height, i32 subpixel, const char* make, const char* model, i32 transform) {
	llogf(NULL, LOG_DEBUG, "got output geometry x={i32}, y={i32}, phys_width={i32}, phys_height={i32}, subpixel={i32}, make='{char*}', model='{char*}', transform={i32}",
		x, y, phys_width, phys_height, subpixel, make, model, transform);
}

static
void output_mode(void* userdata, struct wl_output* output, u32 flags, i32 width, i32 height, i32 refresh) {
	llogf(NULL, LOG_DEBUG, "got output mode {i32}x{i32}@{i32}.{i32}hz", width, height, refresh / 1000, refresh % 1000);
}

static
void output_name(void* userdata, struct wl_output* output, const char* name) {
	llogf(NULL, LOG_DEBUG, "got output name '{char*}'", name);
}

static
void output_scale(void* userdata, struct wl_output* output, i32 scale) {
	llogf(NULL, LOG_DEBUG, "got output scale {i32}", scale);
}

static
struct wl_output_listener output_listener = {
	.description = output_description,
	.done        = output_done,
	.geometry    = output_geometry,
	.mode        = output_mode,
	.name        = output_name,
	.scale       = output_scale,
};

static
void global(void* userdata, struct wl_registry* registry, u32 name, const char* interface, u32 version) {
	if (strcmp(interface, wl_shm_interface.name) == 0) {
		shm = wl_registry_bind(registry, name, &wl_shm_interface, 1);
	}
	else if (strcmp(interface, wl_compositor_interface.name) == 0) {
		compositor = wl_registry_bind(registry, name, &wl_compositor_interface, 4);
	}
	else if (strcmp(interface, xdg_wm_base_interface.name) == 0) {
		xdg_wm_base = wl_registry_bind(registry, name, &xdg_wm_base_interface, 1);
		xdg_wm_base_add_listener(xdg_wm_base, &xdg_wm_base_listener, NULL);
	}
	else if (strcmp(interface, wl_seat_interface.name) == 0) {
		seat = wl_registry_bind(registry, name, &wl_seat_interface, 7);
		wl_seat_add_listener(seat, &seat_listener, NULL);
	}
	else if (strcmp(interface, wl_output_interface.name) == 0) {
		output = wl_registry_bind(registry, name, &wl_output_interface, 4);
		wl_output_add_listener(output, &output_listener, NULL);
	}
	else {
		llogf(NULL, LOG_DEBUG, "ignoring global interface of type '{char*}'", interface);
	}
}

static
void global_remove(void* user, struct wl_registry* registry, u32 name) {

}

static
struct wl_registry_listener listener = {
	.global        = global,
	.global_remove = global_remove,
};

void window_init(const window_info_t info[static 1], err* err) {
	display = wl_display_connect(NULL);
	if (!display) {
		throw(err, ERR_UNKNOWN, "wl_display_connect() failed");
		return;
	}

	registry = wl_display_get_registry(display);
	if (!registry) {
		throw(err, ERR_UNKNOWN, "wl_display_get_registry() failed");
		goto err0;
	}
	wl_registry_add_listener(registry, &listener, NULL);

	wl_display_roundtrip(display);

	if (!compositor || !xdg_wm_base) {
		throw(err, ERR_UNKNOWN, "missing required wayland global(s)");
		goto err1;
	}

	win.wl_surface = wl_compositor_create_surface(compositor);
	if (!win.wl_surface) {
		throw(err, ERR_UNKNOWN, "wl_compositor_create_surface() failed");
		goto err1;
	}

	win.xdg_surface = xdg_wm_base_get_xdg_surface(xdg_wm_base, win.wl_surface);
	if (!win.xdg_surface) {
		throw(err, ERR_UNKNOWN, "xdg_wm_base_get_xdg_surface() failed");
		goto err2;
	}
	xdg_surface_add_listener(win.xdg_surface, &xdg_surface_listener, &win);

	win.xdg_toplevel = xdg_surface_get_toplevel(win.xdg_surface);
	if (!win.xdg_toplevel) {
		throw(err, ERR_UNKNOWN, "xdg_surface_get_toplevel() failed");
		goto err3;
	}
	char* title_cstr = lstos(info->title, err);
	if (title_cstr) {
		xdg_toplevel_set_title(win.xdg_toplevel, title_cstr);
		free(title_cstr);
	}
	char* app_id_cstr = lstos(info->app_id, err);
	if (app_id_cstr) {
		xdg_toplevel_set_app_id(win.xdg_toplevel, app_id_cstr);
		free(app_id_cstr);
	}
	xdg_toplevel_add_listener(win.xdg_toplevel, &xdg_toplevel_listener, &win);

	wl_surface_commit(win.wl_surface);

	wl_display_roundtrip(display);

	recreate_buffer(info->width, info->height);
	return;

err3:
	xdg_surface_destroy(win.xdg_surface);
err2:
	wl_surface_destroy(win.wl_surface);
err1:
	wl_registry_destroy(registry);
err0:
	wl_display_disconnect(display);
}

void window_resize(i32 width, i32 height) {
	struct wl_array states;
	wl_array_init(&states);

	if (win.fullscreen) {
		u32* p = wl_array_add(&states, sizeof(u32));
		if (p) *p = XDG_TOPLEVEL_STATE_FULLSCREEN;
	}

	if (win.maximized) {
		u32* p = wl_array_add(&states, sizeof(u32));
		if (p) *p = XDG_TOPLEVEL_STATE_MAXIMIZED;
	}

	xdg_toplevel_configure(&win, win.xdg_toplevel, width, height, &states);

	wl_array_release(&states);
}

void set_fullscreen(b8 fullscreen) {
	if (fullscreen)
		xdg_toplevel_set_fullscreen(win.xdg_toplevel, NULL);
	else
		xdg_toplevel_unset_fullscreen(win.xdg_toplevel);
}

b8 get_fullscreen(void) {
	return win.fullscreen;
}

void set_maximized(b8 maximized) {
	if (maximized)
		xdg_toplevel_set_maximized(win.xdg_toplevel);
	else
		xdg_toplevel_unset_maximized(win.xdg_toplevel);
}

b8 get_maximized(void) {
	return win.maximized;
}

void platform_poll_wevents() {
	wl_display_dispatch(display);
}

void window_present() {
	wl_surface_damage(win.wl_surface, 0, 0, window_width, window_height);
	wl_surface_attach(win.wl_surface, win.buffers[win.active_buffer], 0, 0);
	wl_surface_commit(win.wl_surface);
	wl_display_flush(display);

	win.active_buffer = !win.active_buffer;
	win.pb.data = win.buffer_data + (window_width * window_height * win.active_buffer);
}

void draw_vline(i32 x, i32 y, i32 y2, u32 color) {
	pb_draw_vline(&win.pb, x, y, y2, color);
}

void draw_hline(i32 x, i32 y, i32 x2, u32 color) {
	pb_draw_hline(&win.pb, x, y, x2, color);
}

void fill_circle(i32 x, i32 y, i32 r, u32 color) {
	pb_fill_circle(&win.pb, x, y, r, color);
}

void draw_rect(i32 x, i32 y, i32 w, i32 h, u32 color) {
	pb_draw_rect(&win.pb, x, y, w, h, color);
}

void fill_rect(i32 x, i32 y, i32 w, i32 h, u32 color) {
	pb_fill_rect(&win.pb, x, y, w, h, color);
}

void draw_line(i32 x, i32 y, i32 x2, i32 y2, u32 color) {
	pb_draw_line(&win.pb, x, y, x2, y2, color);
}

void draw_text(ls str, i32 x, i32 y, u32 color) {

}

void measure_text(ls str) {

}

void put_pixbuf(i32 x, i32 y, const pixbuf_t other[static 1]) {
	pb_blit(&win.pb, x, y, other);
}

#endif // WAYLAND

