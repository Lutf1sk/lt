#include "window_def.h"

#if defined(LT_X11)
#	include <lt/mem.h>
#	include <lt/debug.h>
#	include <X11/Xlib-xcb.h>
#	include <X11/keysym.h>
#	include <X11/XKBlib.h>
#	include <xcb/randr.h>
#	include <xcb/xcb_ewmh.h>

#	include "xproto.h"

#	include <lt/io.h>
#	include <lt/internal.h>

static lt_keycode_t lt_lookup_key(lt_window_t* win, xcb_keycode_t keycode, u16 state);
static void lt_generate_keytab(lt_window_t* win, lt_alloc_t* alloc);

Display* lt_display = NULL;
xcb_connection_t* lt_conn = NULL;
xcb_ewmh_connection_t lt_ewmh;
xcb_screen_t* lt_screen = NULL;
u32 lt_screen_index = 0;

xcb_atom_t WM_DELETE_WINDOW;

int lt_output_count = 0;
lt_output_t* lt_outputs = NULL;

static xcb_atom_t CLIPBOARD, UTF8_STRING, __CLIP, TARGETS;
static b8 clipboard_initialized = 0;
static xcb_window_t clipboard_window = XCB_WINDOW_NONE;

#define CLIPBOARD_REPLY_MAXLEN LT_MB(8)

static
xcb_atom_t get_atom(lstr_t atom_name) {
	xcb_intern_atom_cookie_t atom_cookie = xcb_intern_atom(lt_conn, 1, atom_name.len, atom_name.str);
	xcb_intern_atom_reply_t* atom_reply = xcb_intern_atom_reply(lt_conn, atom_cookie, NULL);
	if (!atom_reply) {
		lt_werrf("atom reply is null\n");
		return XCB_ATOM_NONE;
	}

	xcb_atom_t atom = atom_reply->atom;
	if (atom == XCB_ATOM_NONE)
		lt_werrf("atom '%S' not found\n", atom_name);

	free(atom_reply);
	return atom;
}

static
double mode_rate(xcb_randr_mode_info_t* mode) {
	double vtotal = mode->vtotal;
	if (mode->mode_flags & XCB_RANDR_MODE_FLAG_DOUBLE_SCAN)
		vtotal *= 2;

	if (mode->mode_flags & XCB_RANDR_MODE_FLAG_INTERLACE)
		vtotal /= 2;

	if (!mode->htotal || !vtotal)
		return 0;

	return (double)mode->dot_clock / ((double)mode->htotal * vtotal);
}

lt_err_t lt_window_init(lt_alloc_t* alloc) {
	lt_err_t err;

	lt_xproto_init();

	lt_display = XOpenDisplay(NULL);
	if (!lt_display)
		fail_to(err = LT_ERR_UNKNOWN, err0); // !!

	lt_screen_index = DefaultScreen(lt_display);

	lt_conn = XGetXCBConnection(lt_display);
	if (!lt_conn)
		fail_to(err = LT_ERR_UNKNOWN, err1); // !!

	XkbSetDetectableAutoRepeat(lt_display, True, NULL);
	XSetEventQueueOwner(lt_display, XCBOwnsEventQueue);

	// Find screen
	xcb_screen_iterator_t it = xcb_setup_roots_iterator(xcb_get_setup(lt_conn));
	for (int i = 0; i < lt_screen_index; ++i)
		xcb_screen_next(&it);

	lt_screen = it.data;
	if (!lt_screen)
		fail_to(err = LT_ERR_UNKNOWN, err1); // !!

	// EWMH
	xcb_intern_atom_cookie_t* ewmh_cookie = xcb_ewmh_init_atoms(lt_conn, &lt_ewmh);
	if (!xcb_ewmh_init_atoms_replies(&lt_ewmh, ewmh_cookie, NULL))
		lt_werr(CLSTR("Failed to initialize EWMH atoms\n"));

	WM_DELETE_WINDOW = get_atom(CLSTR("WM_DELETE_WINDOW"));
	if (WM_DELETE_WINDOW == XCB_ATOM_NONE)
		lt_werr(CLSTR("WM_DELETE_WINDOW not found\n"));

	// Find outputs
	xcb_randr_get_screen_resources_current_cookie_t scrres_cookie =
			xcb_randr_get_screen_resources_current(lt_conn, lt_screen->root);
	xcb_randr_get_screen_resources_current_reply_t* scrres_reply =
			xcb_randr_get_screen_resources_current_reply(lt_conn, scrres_cookie, NULL);

	if (!scrres_reply)
		fail_to(err = LT_ERR_UNKNOWN, err1); // !!

	xcb_timestamp_t timestamp = scrres_reply->config_timestamp;
	int output_count = xcb_randr_get_screen_resources_current_outputs_length(scrres_reply);
	xcb_randr_output_t* outputs = xcb_randr_get_screen_resources_current_outputs(scrres_reply);

	int crtc_count = 0;

	int mode_count = xcb_randr_get_screen_resources_current_modes_length(scrres_reply);
	xcb_randr_mode_info_t* modes = xcb_randr_get_screen_resources_current_modes(scrres_reply);

	lt_outputs = lt_malloc(alloc, output_count * sizeof(lt_output_t));

	for (int i = 0; i < output_count; ++i) {
		xcb_randr_get_output_info_cookie_t outinf_cookie = xcb_randr_get_output_info(lt_conn, outputs[i], timestamp);
		xcb_randr_get_output_info_reply_t* outinf_reply = xcb_randr_get_output_info_reply(lt_conn, outinf_cookie, NULL);
		if (!outinf_reply)
			continue;

		if (outinf_reply->crtc == XCB_NONE || outinf_reply->connection == XCB_RANDR_CONNECTION_DISCONNECTED) {
			free(outinf_reply);
			continue;
		}

		xcb_randr_get_crtc_info_cookie_t crtcinf_cookie = xcb_randr_get_crtc_info(lt_conn, outinf_reply->crtc, timestamp);
		xcb_randr_get_crtc_info_reply_t* crtcinf_reply = xcb_randr_get_crtc_info_reply(lt_conn, crtcinf_cookie, NULL);

		if (!crtcinf_reply) {
			free(outinf_reply);
			continue;
		}

		char* out_name = (char*)xcb_randr_get_output_info_name(outinf_reply);
		int out_name_len = xcb_randr_get_output_info_name_length(outinf_reply);

		char* alloc_out_name = lt_malloc(alloc, out_name_len);
		memcpy(alloc_out_name, out_name, out_name_len);

		usz mode_index = 0;

		for (usz i = 0; i < mode_count; ++i) {
			if (crtcinf_reply->mode == modes[i].id) {
				mode_index = i;
				break;
			}
		}

		lt_outputs[crtc_count].x = crtcinf_reply->x;
		lt_outputs[crtc_count].y = crtcinf_reply->y;
		lt_outputs[crtc_count].w = crtcinf_reply->width;
		lt_outputs[crtc_count].h = crtcinf_reply->height;
		lt_outputs[crtc_count].name = LSTR(alloc_out_name, out_name_len);
		lt_outputs[crtc_count].rate = mode_rate(&modes[mode_index]);
		++crtc_count;

		free(crtcinf_reply);
		free(outinf_reply);
	}

	free(scrres_reply);

	lt_output_count = crtc_count;
	return LT_SUCCESS;

err1:	XCloseDisplay(lt_display);
err0:	return err;
}

void lt_window_terminate(lt_alloc_t* alloc) {
	if (clipboard_initialized)
		clipboard_initialized = 0;

	for (usz i = 0; i < lt_output_count; ++i)
		lt_mfree(alloc, lt_outputs[i].name.str);
	lt_mfree(alloc, lt_outputs);

	XCloseDisplay(lt_display);
}

static
void clipboard_initialize(void) {
	clipboard_initialized = 1;

	CLIPBOARD = get_atom(CLSTR("CLIPBOARD"));
	UTF8_STRING = get_atom(CLSTR("UTF8_STRING"));
	__CLIP = get_atom(CLSTR("__CLIP"));
	TARGETS = get_atom(CLSTR("TARGETS"));

	u32 win_mask = XCB_CW_EVENT_MASK;
	u32 win_list[] = { XCB_EVENT_MASK_STRUCTURE_NOTIFY | XCB_EVENT_MASK_SUBSTRUCTURE_NOTIFY };
	int visual = lt_screen->root_visual, depth = XCB_COPY_FROM_PARENT;

	clipboard_window = xcb_generate_id(lt_conn);
	xcb_void_cookie_t window_cookie = xcb_create_window(
		lt_conn, depth, clipboard_window, lt_screen->root, -10, -10, 1, 1, 0,
		XCB_WINDOW_CLASS_INPUT_OUTPUT, visual, win_mask, win_list
	);
	if (xcb_request_check(lt_conn, window_cookie))
		lt_werrf("failed to create clipboard window\n");
}

/*
void lt_window_set_clipboard(lstr_t str) {
	if (!clipboard_initialized)
		clipboard_initialize();

	xcb_void_cookie_t owner_cookie = xcb_set_selection_owner(lt_conn, clipboard_window, CLIPBOARD, XCB_CURRENT_TIME);
	if (xcb_request_check(lt_conn, owner_cookie)) {
		lt_werrf("failed to claim clipboard ownership\n");
		return;
	}

	for (;;) {
		xcb_generic_event_t* gev = xcb_wait_for_event(lt_conn);
		if (!gev)
			continue;

		switch (gev->response_type & ~0x80) {
		case XCB_SELECTION_CLEAR:
			lt_werrf("lost clipboard ownership\n");
			return;
		case XCB_SELECTION_REQUEST: {
			xcb_selection_request_event_t* ev = (xcb_selection_request_event_t*)gev;
			xcb_selection_notify_event_t sev;
			sev.response_type = XCB_SELECTION_NOTIFY;
			sev.requestor = ev->requestor;
			sev.selection = ev->selection;
			sev.target = ev->target;
			sev.property = ev->property;
			sev.time = ev->time;

			if (ev->target == TARGETS) {
				xcb_atom_t targets[] = {
					TARGETS,
					get_atom(CLSTR("MULTIPLE")),
					UTF8_STRING,
					get_atom(CLSTR("STRING")),
					get_atom(CLSTR("TEXT")),
				};
				usz target_count = sizeof(targets) / sizeof(*targets);

				xcb_void_cookie_t prop_cookie = xcb_change_property(lt_conn, XCB_PROP_MODE_REPLACE, ev->requestor, ev->property, TARGETS, 32, target_count, targets);
				if (xcb_request_check(lt_conn, prop_cookie))
					lt_werrf("failed to change property\n");

				lt_printf("TARGETS\n");

				u32 ev_mask = XCB_EVENT_MASK_SUBSTRUCTURE_REDIRECT | XCB_EVENT_MASK_SUBSTRUCTURE_NOTIFY;
				xcb_send_event(lt_conn, 0, ev->requestor, ev_mask, (const char*)&sev);

				xcb_flush(lt_conn);
				continue;
			}

			if (ev->target != UTF8_STRING || ev->property == XCB_ATOM_NONE) {
				sev.property = XCB_ATOM_NONE;

				xcb_get_atom_name_cookie_t name_cookie = xcb_get_atom_name(lt_conn, ev->target);
				xcb_get_atom_name_reply_t* name_reply = xcb_get_atom_name_reply(lt_conn, name_cookie, NULL);
				if (!name_reply) {
					lt_werrf("atom name reply is null\n");
					return;
				}
				char* name = xcb_get_atom_name_name(name_reply);
				usz length = xcb_get_atom_name_name_length(name_reply);

				lt_printf("denying request of type '%S'\n", LSTR(name, length));
				free(name_reply);

				u32 ev_mask = XCB_EVENT_MASK_SUBSTRUCTURE_REDIRECT | XCB_EVENT_MASK_SUBSTRUCTURE_NOTIFY;
				xcb_send_event(lt_conn, 0, ev->requestor, ev_mask, (const char*)&sev);
			}
			else {
				xcb_void_cookie_t prop_cookie = xcb_change_property(lt_conn, XCB_PROP_MODE_REPLACE, ev->requestor, ev->property, ev->target, 8, str.len, str.str);
				if (xcb_request_check(lt_conn, prop_cookie))
					lt_werrf("failed to change property\n");

				lt_printf("UTF8_STRING\n");

				u32 ev_mask = XCB_EVENT_MASK_SUBSTRUCTURE_REDIRECT | XCB_EVENT_MASK_SUBSTRUCTURE_NOTIFY;
				xcb_send_event(lt_conn, 0, ev->requestor, ev_mask, (const char*)&sev);
			}
			xcb_flush(lt_conn);
			return;
		}
		}
	}
}*/

lstr_t lt_window_get_clipboard(lt_alloc_t* alloc) {
	if (!clipboard_initialized)
		clipboard_initialize();

	xcb_get_selection_owner_cookie_t owner_cookie = xcb_get_selection_owner(lt_conn, CLIPBOARD);
	xcb_get_selection_owner_reply_t* owner_reply = xcb_get_selection_owner_reply(lt_conn, owner_cookie, NULL);
	if (owner_reply) {
		xcb_window_t owner = owner_reply->owner;
		free(owner_reply);
		if (owner == XCB_WINDOW_NONE)
			return NLSTR();
	}

	xcb_void_cookie_t convert_cookie = xcb_convert_selection(lt_conn, clipboard_window, CLIPBOARD, UTF8_STRING, __CLIP, XCB_CURRENT_TIME);
	if (xcb_request_check(lt_conn, convert_cookie))
		return NLSTR();

	// TODO: ew
	for (;;) {
		xcb_generic_event_t* gev = xcb_wait_for_event(lt_conn);
		if (!gev)
			continue;

		switch (gev->response_type & ~0x80) {
		case XCB_SELECTION_NOTIFY: {
			xcb_selection_notify_event_t* ev = (xcb_selection_notify_event_t*)gev;
			if (ev->property == XCB_ATOM_NONE)
				lt_werrf("conversion failed\n");

			LT_ASSERT(ev->requestor == clipboard_window);
			LT_ASSERT(ev->selection == CLIPBOARD);
			LT_ASSERT(ev->target == UTF8_STRING);

			xcb_get_property_cookie_t prop_cookie = xcb_get_property(lt_conn, 0, clipboard_window, __CLIP, XCB_ATOM_ANY, 0, CLIPBOARD_REPLY_MAXLEN/4);
			xcb_get_property_reply_t* prop_reply = xcb_get_property_reply(lt_conn, prop_cookie, NULL);
			if (!prop_reply)
				return NLSTR();

			void* value = xcb_get_property_value(prop_reply);
			int length = xcb_get_property_value_length(prop_reply);

			char* ret_value = lt_malloc(alloc, length);
			if (!ret_value) {
				free(prop_reply);
				return NLSTR();
			}
			memcpy(ret_value, value, length);

			free(prop_reply);
			return LSTR(ret_value, length);
		}
		}

		free(gev);
	}
}

int lt_window_output_count(void) {
	return lt_output_count;
}

lt_output_t* lt_window_outputs(void) {
	return lt_outputs;
}

lt_window_t* lt_window_create(lt_window_description_t* desc, lt_alloc_t* alloc) {
	int output = desc->output_index;

	// Calculate window dimensions
	int w = desc->w;
	int h = desc->h;
	if (w <= 0)
		w = lt_outputs[output].w;
	if (h <= 0)
		h = lt_outputs[output].h;

	int x = desc->x;
	if (x <= 0)
		x = lt_outputs[output].x + lt_outputs[output].w / 2 - w / 2;
	int y = desc->y;
	if (y <= 0)
		y = lt_outputs[output].y + lt_outputs[output].h / 2 - h / 2;

	xcb_window_t window;
	xcb_void_cookie_t window_cookie;

	u32 win_mask = XCB_CW_EVENT_MASK | XCB_CW_COLORMAP;
	u32 win_list[] = {
		XCB_EVENT_MASK_KEY_PRESS | XCB_EVENT_MASK_KEY_RELEASE |
		XCB_EVENT_MASK_BUTTON_PRESS | XCB_EVENT_MASK_BUTTON_RELEASE |
		XCB_EVENT_MASK_POINTER_MOTION |
		XCB_EVENT_MASK_ENTER_WINDOW | XCB_EVENT_MASK_LEAVE_WINDOW |
		XCB_EVENT_MASK_EXPOSURE |
		XCB_EVENT_MASK_STRUCTURE_NOTIFY,
		0
	};
	int visual = lt_screen->root_visual, depth = XCB_COPY_FROM_PARENT;

	// Get GL visual and create colormap
	XVisualInfo* vi = NULL;
	Colormap clrmap = 0;
	if (desc->type & LT_WIN_GL) {
		static GLint att[] = { GLX_RGBA, GLX_DEPTH_SIZE, 24, GLX_DOUBLEBUFFER, None };
		vi = glXChooseVisual(lt_display, 0, att);
		visual = vi->visualid;

		clrmap = XCreateColormap(lt_display, lt_screen->root, vi->visual, AllocNone);
		win_list[1] = clrmap;
	}

	window = xcb_generate_id(lt_conn);
	window_cookie = xcb_create_window(
		lt_conn, depth, window, lt_screen->root, x, y, w, h, 0,
		XCB_WINDOW_CLASS_INPUT_OUTPUT, visual, win_mask, win_list
	);

	// Set title
	xcb_void_cookie_t title_cookie = xcb_change_property(lt_conn, XCB_PROP_MODE_REPLACE, window,
		XCB_ATOM_WM_NAME, XCB_ATOM_STRING, 8, desc->title.len, desc->title.str);

	// Map window
	xcb_void_cookie_t map_cookie = xcb_map_window(lt_conn, window);

	if (xcb_request_check(lt_conn, window_cookie))
		return NULL;
	if (xcb_request_check(lt_conn, title_cookie))
		lt_werr(CLSTR("Failed to set window title\n"));
	if (xcb_request_check(lt_conn, map_cookie))
		goto setup_err;

	// Create graphics context
	u32 gc = 0;
	if (desc->type & LT_WIN_SOFT) {
		gc = xcb_generate_id(lt_conn);
		u32 gc_mask = XCB_GC_FOREGROUND | XCB_GC_BACKGROUND;
		u32 gc_list[] = { lt_screen->white_pixel, lt_screen->black_pixel };
		xcb_void_cookie_t gc_cookie = xcb_create_gc(lt_conn, gc, window, gc_mask, gc_list);

		if (xcb_request_check(lt_conn, gc_cookie))
			lt_werr(CLSTR("Failed to create graphics context\n"));
	}

	// Listen for close events
	xcb_change_property(lt_conn, XCB_PROP_MODE_REPLACE, window, lt_ewmh.WM_PROTOCOLS, XCB_ATOM_ATOM, 32, 1, &WM_DELETE_WINDOW);

	// Flush XCB connection
	xcb_flush(lt_conn);

	// Create GL context
	GLXContext glc = 0;
	if (desc->type & LT_WIN_GL) {
		glc = glXCreateContext(lt_display, vi, NULL, GL_TRUE);
		glXMakeCurrent(lt_display, window, glc);
	}

	// Create lt_window structure
	lt_window_t* win = lt_malloc(alloc, sizeof(lt_window_t));
	memset(win, 0, sizeof(lt_window_t));
	win->window = window;
	win->gc = gc;
	win->glctx = glc;
	win->clrmap = clrmap;
	win->closed = 0;
	win->type = desc->type;
	win->pos_x = x;
	win->pos_y = y;
	win->size_w = w;
	win->size_h = h;
	win->mpos_x = -1;
	win->mpos_y = -1;

	lt_generate_keytab(win, alloc);
	return win;

setup_err:
	xcb_destroy_window(lt_conn, window);
	return NULL;
}

void lt_window_destroy(lt_window_t* win, lt_alloc_t* alloc) {
	if (win->glctx) {
		XFreeColormap(lt_display, win->clrmap);
		glXMakeCurrent(lt_display, None, NULL);
		glXDestroyContext(lt_display, win->glctx);
	}

	xcb_destroy_window(lt_conn, win->window);

	lt_mfree(alloc, win->keytab);
	lt_mfree(alloc, win);
}

static lt_keycode_t btnmap[] = {
	[0] = 0,
	[1] = LT_KEY_MB1,
	[2] = LT_KEY_MB3,
	[3] = LT_KEY_MB2,
	[4] = LT_KEY_SCROLL_UP,
	[5] = LT_KEY_SCROLL_DOWN,
};

static
void handle_event(lt_window_t* win, xcb_generic_event_t* gev) {
	xcb_window_t window = win->window;

	switch (gev->response_type & ~0x80) {
	case XCB_DESTROY_NOTIFY: {
		xcb_destroy_notify_event_t* ev = (xcb_destroy_notify_event_t*)gev;
		if (ev->window == window)
			win->closed = 1;
	}	break;

	case XCB_EXPOSE:
	case XCB_ENTER_NOTIFY:
	case XCB_LEAVE_NOTIFY:
	case XCB_REPARENT_NOTIFY:
	case XCB_MAP_NOTIFY:
		break;

	case XCB_KEY_PRESS: {
		xcb_key_press_event_t* ev = (xcb_key_press_event_t*)gev;
		lt_keycode_t key = lt_lookup_key(win, ev->detail, ev->state);
		win->key_press_map[key] = 1;
	}	break;

	case XCB_KEY_RELEASE: {
		xcb_key_release_event_t* ev = (xcb_key_release_event_t*)gev;
		lt_keycode_t key = lt_lookup_key(win, ev->detail, ev->state);
		win->key_press_map[key] = 0;
	}	break;

	case XCB_CONFIGURE_NOTIFY: {
		xcb_configure_notify_event_t* ev = (xcb_configure_notify_event_t*)gev;
		win->pos_x = ev->x;
		win->pos_y = ev->y;
		win->size_w = ev->width;
		win->size_h = ev->height;
	}	break;

	case XCB_BUTTON_PRESS: {
		xcb_button_press_event_t* ev = (xcb_button_press_event_t*)gev;
		lt_keycode_t btn = btnmap[ev->detail];
		if (btn)
			win->key_press_map[btn] = 1;
	}	break;

	case XCB_BUTTON_RELEASE: {
		xcb_button_release_event_t* ev = (xcb_button_release_event_t*)gev;
		lt_keycode_t btn = btnmap[ev->detail];
		if (btn && btn != LT_KEY_SCROLL_UP && btn != LT_KEY_SCROLL_DOWN)
			win->key_press_map[btn] = 0;
	}	break;

	case XCB_MOTION_NOTIFY: {
		xcb_motion_notify_event_t* ev = (xcb_motion_notify_event_t*)gev;
		win->mpos_x = ev->event_x - 1;
		win->mpos_y = ev->event_y - 2;
	}	break;

	case XCB_CLIENT_MESSAGE: {
		xcb_client_message_event_t* ev = (xcb_client_message_event_t*)gev;

		if (ev->type == lt_ewmh.WM_PROTOCOLS) {
			xcb_atom_t prot = ev->data.data32[0];
			if (prot == WM_DELETE_WINDOW)
				win->closed = 1;
		}
		else if (ev->type != XCB_ATOM_NONE)
			lt_printf("Unhandled XCB client message\n");
	}	break;

	default:
		lt_printf("Unhandled XCB event %ud\n", gev->response_type & ~0x80);
		break;
	}
}

static
b8 translate_event(lt_window_t* win, xcb_generic_event_t* gev, lt_window_event_t* ltev) {
	switch (gev->response_type & ~0x80) {
	case XCB_KEY_PRESS: {
		xcb_key_press_event_t* ev = (xcb_key_press_event_t*)gev;
		lt_keycode_t key = lt_lookup_key(win, ev->detail, ev->state);
		ltev->type = LT_WIN_EVENT_KEY_PRESS;
		ltev->key = key;
	}	return 1;

	case XCB_KEY_RELEASE: {
		xcb_key_release_event_t* ev = (xcb_key_release_event_t*)gev;
		lt_keycode_t key = lt_lookup_key(win, ev->detail, ev->state);
		ltev->type = LT_WIN_EVENT_KEY_RELEASE;
		ltev->key = key;
	}	return 1;

	case XCB_BUTTON_PRESS: {
		xcb_button_press_event_t* ev = (xcb_button_press_event_t*)gev;
		lt_keycode_t btn = btnmap[ev->detail];
		ltev->type = LT_WIN_EVENT_BUTTON_PRESS;
		ltev->button = btn;
	}	return 1;

	case XCB_BUTTON_RELEASE: {
		xcb_button_release_event_t* ev = (xcb_button_release_event_t*)gev;
		lt_keycode_t btn = btnmap[ev->detail];
		ltev->type = LT_WIN_EVENT_BUTTON_RELEASE;
		ltev->button = btn;
	}	return 1;

	case XCB_MOTION_NOTIFY:
		ltev->type = LT_WIN_EVENT_MOTION;
		return 1;

	default:
		return 0;
	}
}

usz lt_window_poll_events(lt_window_t* win, lt_window_event_t* events, usz max_events) {
	win->key_press_map[LT_KEY_SCROLL_UP] = 0;
	win->key_press_map[LT_KEY_SCROLL_DOWN] = 0;
	memcpy(win->old_key_press_map, win->key_press_map, sizeof(win->key_press_map));

	usz ev_count = 0;

	// TODO: Only consume events belonging to the current window
	xcb_generic_event_t* gev = NULL;
	while ((gev = xcb_poll_for_event(lt_conn))) {
		handle_event(win, gev);
		if (events && ev_count < max_events && translate_event(win, gev, &events[ev_count]))
			++ev_count;
		free(gev);
	}

	xcb_flush(lt_conn);
	return ev_count;
}

usz lt_window_wait_events(lt_window_t* win, lt_window_event_t* events, usz max_events) {
	memcpy(win->old_key_press_map, win->key_press_map, sizeof(win->key_press_map));

	usz ev_count = 0;

	// TODO: Only consume events belonging to the current window
	xcb_generic_event_t* gev = xcb_wait_for_event(lt_conn);
	if (gev) {
		handle_event(win, gev);
		if (events && ev_count < max_events && translate_event(win, gev, &events[ev_count]))
			++ev_count;
		free(gev);
	}

	xcb_flush(lt_conn);
	return ev_count;
}

void lt_window_set_fullscreen(lt_window_t* win, lt_winstate_t state) {
	xcb_client_message_event_t ev;
	ev.response_type = XCB_CLIENT_MESSAGE;
	ev.type = lt_ewmh._NET_WM_STATE;
	ev.format = 32;
	ev.window = win->window;

	switch (state) {
	case LT_WIN_SET: ev.data.data32[0] = XCB_EWMH_WM_STATE_ADD; break;
	default:
	case LT_WIN_CLEAR: ev.data.data32[0] = XCB_EWMH_WM_STATE_REMOVE; break;
	case LT_WIN_TOGGLE: ev.data.data32[0] = XCB_EWMH_WM_STATE_TOGGLE; break;
	}

	ev.data.data32[1] = lt_ewmh._NET_WM_STATE_FULLSCREEN;
	ev.data.data32[2] = XCB_ATOM_NONE;
	ev.data.data32[3] = XCB_ATOM_NONE;

	u32 ev_mask = XCB_EVENT_MASK_SUBSTRUCTURE_REDIRECT | XCB_EVENT_MASK_SUBSTRUCTURE_NOTIFY;
	xcb_send_event(lt_conn, 0, lt_screen->root, ev_mask, (const char*)&ev);
}

void lt_window_set_pos(lt_window_t* win, int x, int y) {
	u32 values[2] = { x, y };
	xcb_configure_window(lt_conn, win->window, XCB_CONFIG_WINDOW_X | XCB_CONFIG_WINDOW_Y, values);
}

void lt_window_set_size(lt_window_t* win, int w, int h) {
	u32 values[2] = { w, h };
	xcb_configure_window(lt_conn, win->window, XCB_CONFIG_WINDOW_WIDTH | XCB_CONFIG_WINDOW_HEIGHT, values);
}

void lt_window_get_pos(lt_window_t* win, int* x, int* y) {
	*x = win->pos_x;
	*y = win->pos_y;
}

void lt_window_get_size(lt_window_t* win, int* w, int* h) {
	*w = win->size_w;
	*h = win->size_h;
}

extern int strncmp(const char*, const char*, usz);

static
void lt_generate_keytab(lt_window_t* win, lt_alloc_t* alloc) {
	const struct { int key; char* name; } keymap[] = {
		{ LT_KEY_TILDE, "TLDE" },
		{ LT_KEY_1, "AE01" }, { LT_KEY_2, "AE02" }, { LT_KEY_3, "AE03" }, { LT_KEY_4, "AE04" },
		{ LT_KEY_5, "AE05" }, { LT_KEY_6, "AE06" }, { LT_KEY_7, "AE07" }, { LT_KEY_8, "AE08" },
		{ LT_KEY_9, "AE09" }, { LT_KEY_0, "AE10" },
		{ LT_KEY_MINUS, "AE11" }, { LT_KEY_EQUAL, "AE12" },
		{ LT_KEY_Q, "AD01" }, { LT_KEY_W, "AD02" }, { LT_KEY_E, "AD03" }, { LT_KEY_R, "AD04" },
		{ LT_KEY_T, "AD05" }, { LT_KEY_Y, "AD06" }, { LT_KEY_U, "AD07" }, { LT_KEY_I, "AD08" },
		{ LT_KEY_O, "AD09" }, { LT_KEY_P, "AD10" },
		{ LT_KEY_LBRACKET, "AD11" }, { LT_KEY_RBRACKET, "AD12" },
		{ LT_KEY_A, "AC01" }, { LT_KEY_S, "AC02" }, { LT_KEY_D, "AC03" }, { LT_KEY_F, "AC04" },
		{ LT_KEY_G, "AC05" }, { LT_KEY_H, "AC06" }, { LT_KEY_J, "AC07" }, { LT_KEY_K, "AC08" },
		{ LT_KEY_L, "AC09" },
		{ LT_KEY_SEMICOLON, "AC10" }, { LT_KEY_APOSTROPHE, "AC11" },
		{ LT_KEY_Z, "AB01" }, { LT_KEY_X, "AB02" }, { LT_KEY_C, "AB03" }, { LT_KEY_V, "AB04" },
		{ LT_KEY_B, "AB05" }, { LT_KEY_N, "AB06" }, { LT_KEY_M, "AB07" },
		{ LT_KEY_COMMA, "AB08" }, { LT_KEY_PERIOD, "AB09" },
		{ LT_KEY_SLASH, "AB10" }, { LT_KEY_BACKSLASH, "BKSL" },
//		{ LT_KEY_WORLD_1, "LSGT" },
		{ LT_KEY_SPACE, "SPCE" }, { LT_KEY_ESC, "ESC" },
		{ LT_KEY_ENTER, "RTRN" }, { LT_KEY_TAB, "TAB" }, { LT_KEY_BACKSPACE, "BKSP" },
		{ LT_KEY_INSERT, "INS" }, { LT_KEY_DELETE, "DELE" },
		{ LT_KEY_RIGHT, "RGHT" }, { LT_KEY_LEFT, "LEFT" }, { LT_KEY_DOWN, "DOWN" }, { LT_KEY_UP, "UP" },
		{ LT_KEY_PAGE_UP, "PGUP" }, { LT_KEY_PAGE_DOWN, "PGDN" },
		{ LT_KEY_HOME, "HOME" }, { LT_KEY_END, "END" },
		{ LT_KEY_CAPS_LOCK, "CAPS" }, { LT_KEY_SCROLL_LOCK, "SCLK" }, { LT_KEY_NUM_LOCK, "NMLK" },
		{ LT_KEY_PRINT_SCREEN, "PRSC" },
		{ LT_KEY_PAUSE, "PAUS" },
		{ LT_KEY_F1, "FK01" }, { LT_KEY_F2, "FK02" }, { LT_KEY_F3, "FK03" }, { LT_KEY_F4, "FK04" },
		{ LT_KEY_F5, "FK05" }, { LT_KEY_F6, "FK06" }, { LT_KEY_F7, "FK07" }, { LT_KEY_F8, "FK08" },
		{ LT_KEY_F9, "FK09" }, { LT_KEY_F10, "FK10" }, { LT_KEY_F11, "FK11" }, { LT_KEY_F12, "FK12" },
		{ LT_KEY_NP_0, "KP0" }, { LT_KEY_NP_1, "KP1" }, { LT_KEY_NP_2, "KP2" }, { LT_KEY_NP_3, "KP3" },
		{ LT_KEY_NP_4, "KP4" }, { LT_KEY_NP_5, "KP5" }, { LT_KEY_NP_6, "KP6" }, { LT_KEY_NP_7, "KP7" },
		{ LT_KEY_NP_8, "KP8" }, { LT_KEY_NP_9, "KP9" },
		{ LT_KEY_NP_DECIMAL, "KPDL" }, { LT_KEY_NP_DIVIDE, "KPDV" }, { LT_KEY_NP_MULTIPLY, "KPMU" },
		{ LT_KEY_NP_SUBTRACT, "KPSU" }, { LT_KEY_NP_ADD, "KPAD" },
		{ LT_KEY_NP_ENTER, "KPEN" }, { LT_KEY_NP_EQUAL, "KPEQ" },
		{ LT_KEY_LEFT_SHIFT, "LFSH" }, { LT_KEY_LEFT_CTRL, "LCTL" },
		{ LT_KEY_LEFT_ALT, "LALT" }, { LT_KEY_LEFT_SUPER, "LWIN" },
		{ LT_KEY_RIGHT_SHIFT, "RTSH" }, { LT_KEY_RIGHT_CTRL, "RCTL" },
		{ LT_KEY_RIGHT_ALT, "RALT" }, { LT_KEY_RIGHT_ALT, "LVL3" }, { LT_KEY_RIGHT_ALT, "MDSW" },
		{ LT_KEY_RIGHT_SUPER, "RWIN" },
		{ LT_KEY_MENU, "MENU" }
	};

	XkbDescPtr desc = XkbGetMap(lt_display, 0, XkbUseCoreKbd);
	XkbGetNames(lt_display, XkbKeyNamesMask | XkbKeyAliasesMask, desc);

	int scan_min = desc->min_key_code;
	int scan_max = desc->max_key_code;
	int scan_count = scan_max - scan_min + 1;
	win->scan_min = scan_min;

	int* keytab = lt_malloc(alloc, scan_count * sizeof(int));
	win->keytab = keytab;
	memset(keytab, LT_KEY_INVALID, scan_count * sizeof(int));

	for (int scan = scan_min; scan <= scan_max; ++scan) {
		int key = LT_KEY_INVALID;

		// Name only
		for (int i = 0; i < sizeof(keymap) / sizeof(*keymap); ++i) {
			if (strncmp(desc->names->keys[scan].name, keymap[i].name, XkbKeyNameLength) == 0) {
				key = keymap[i].key;
				break;
			}
		}

		// Iterate aliases
		for (int i = 0; i < desc->names->num_key_aliases; ++i) {
			if (key != LT_KEY_INVALID)
				break;

			if (strncmp(desc->names->key_aliases[i].real, desc->names->keys[scan].name, XkbKeyNameLength) != 0)
				continue;

			for (int j = 0; j < sizeof(keymap) / sizeof(*keymap); ++j) {
				if (strncmp(desc->names->key_aliases[i].alias, keymap[j].name, XkbKeyNameLength) == 0) {
					key = keymap[j].key;
					break;
				}
			}
		}

		keytab[scan - scan_min] = key;
	}

	XkbFreeNames(desc, XkbKeyNamesMask, True);
	XkbFreeKeyboard(desc, 0, True);
}

static
lt_keycode_t lt_lookup_key(lt_window_t* win, xcb_keycode_t keycode, u16 state) {
	return win->keytab[keycode - win->scan_min];
}

void lt_window_gl_swap_buffers(lt_window_t* win) {
	glXSwapBuffers(lt_display, win->window);
}

#endif
