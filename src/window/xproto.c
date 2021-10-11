#include "xproto.h"

#include <lt/lt.h>

#define XPFN_DEF(ret, name, args) ret (*lt_xproto_##name)args = NULL

// XCB
XPFN_DEF(const xcb_setup_t*, xcb_get_setup, (xcb_connection_t*));

XPFN_DEF(xcb_void_cookie_t, xcb_map_window, (xcb_connection_t*, xcb_window_t));

XPFN_DEF(void, xcb_screen_next, (xcb_screen_iterator_t*));

XPFN_DEF(xcb_void_cookie_t, xcb_change_property, (xcb_connection_t*, u8, xcb_window_t, xcb_atom_t, xcb_atom_t, u8, u32, const void*));

XPFN_DEF(xcb_void_cookie_t, xcb_destroy_window, (xcb_connection_t*, xcb_window_t));

XPFN_DEF(xcb_void_cookie_t, xcb_configure_window, (xcb_connection_t*, xcb_window_t, u16, const void*));

XPFN_DEF(void, xcb_flush, (xcb_connection_t*));

XPFN_DEF(xcb_generic_error_t*, xcb_request_check, (xcb_connection_t*, xcb_void_cookie_t));

XPFN_DEF(xcb_void_cookie_t, xcb_create_window, (xcb_connection_t*, u8, xcb_window_t, xcb_window_t, i16, i16, u16, u16, u16, u16, xcb_visualid_t, u32, const void*));

XPFN_DEF(xcb_screen_iterator_t, xcb_setup_roots_iterator, (const xcb_setup_t*));

XPFN_DEF(xcb_void_cookie_t, xcb_send_event, (xcb_connection_t*, u8, xcb_window_t, u32, const char*));

XPFN_DEF(u32, xcb_generate_id, (xcb_connection_t*));

XPFN_DEF(xcb_generic_event_t*, xcb_poll_for_event, (xcb_connection_t*));

// XCB-EWMH

XPFN_DEF(xcb_intern_atom_cookie_t*, xcb_ewmh_init_atoms, (xcb_connection_t*, xcb_ewmh_connection_t*));

XPFN_DEF(u8, xcb_ewmh_init_atoms_replies, (xcb_ewmh_connection_t*, xcb_intern_atom_cookie_t*, xcb_generic_error_t**));

// XCB-Randr

XPFN_DEF(xcb_randr_get_screen_resources_current_cookie_t, xcb_randr_get_screen_resources_current,
		(xcb_connection_t*, xcb_window_t));

XPFN_DEF(xcb_randr_get_screen_resources_current_reply_t*, xcb_randr_get_screen_resources_current_reply,
		(xcb_connection_t*, xcb_randr_get_screen_resources_current_cookie_t, xcb_generic_error_t**));

XPFN_DEF(int, xcb_randr_get_screen_resources_current_outputs_length,
		(const xcb_randr_get_screen_resources_current_reply_t*));

XPFN_DEF(xcb_randr_output_t*, xcb_randr_get_screen_resources_current_outputs,
		(const xcb_randr_get_screen_resources_current_reply_t*));

XPFN_DEF(xcb_randr_get_output_info_cookie_t, xcb_randr_get_output_info, (xcb_connection_t*, xcb_randr_output_t, xcb_timestamp_t));

XPFN_DEF(xcb_randr_get_output_info_reply_t*, xcb_randr_get_output_info_reply,
		(xcb_connection_t*, xcb_randr_get_output_info_cookie_t, xcb_generic_error_t**));

XPFN_DEF(xcb_randr_get_crtc_info_cookie_t, xcb_randr_get_crtc_info, (xcb_connection_t*, xcb_randr_crtc_t, xcb_timestamp_t));

XPFN_DEF(xcb_randr_get_crtc_info_reply_t*, xcb_randr_get_crtc_info_reply,
		(xcb_connection_t*, xcb_randr_get_crtc_info_cookie_t, xcb_generic_error_t**));

XPFN_DEF(int, xcb_randr_get_output_info_name_length, (const xcb_randr_get_output_info_reply_t*));

XPFN_DEF(u8*, xcb_randr_get_output_info_name, (const xcb_randr_get_output_info_reply_t*));

// XLib

XPFN_DEF(Display*, XOpenDisplay, (_Xconst char*));

XPFN_DEF(int, XCloseDisplay, (Display*));

XPFN_DEF(Bool, XkbSetDetectableAutoRepeat, (Display*, Bool, Bool*));

XPFN_DEF(Status, XkbGetNames, (Display*, unsigned int, XkbDescPtr));

XPFN_DEF(XkbDescPtr, XkbGetMap, (Display*, unsigned int, unsigned int));

XPFN_DEF(void, XkbFreeKeyboard, (XkbDescPtr, unsigned int, Bool));

XPFN_DEF(void, XkbFreeNames, (XkbDescPtr, unsigned int, Bool));

// XLib-XCB

XPFN_DEF(xcb_connection_t*, XGetXCBConnection, (Display*));

XPFN_DEF(void, XSetEventQueueOwner, (Display*, enum XEventQueueOwner));


#define XPFN_LOAD(name) lt_xproto_##name = lt_dynl_sym(hnd, #name);

void lt_xproto_init(void) {
	{
		void* hnd = lt_dynl_open("libxcb.so");
		if (!hnd)
			lt_ferrf("Failed to load xcb: %s\n", lt_dynl_err_str());

		XPFN_LOAD(xcb_get_setup);
		XPFN_LOAD(xcb_map_window);
		XPFN_LOAD(xcb_screen_next);
		XPFN_LOAD(xcb_change_property);
		XPFN_LOAD(xcb_destroy_window);
		XPFN_LOAD(xcb_configure_window);
		XPFN_LOAD(xcb_flush);
		XPFN_LOAD(xcb_request_check);
		XPFN_LOAD(xcb_create_window);
		XPFN_LOAD(xcb_setup_roots_iterator);
		XPFN_LOAD(xcb_send_event);
		XPFN_LOAD(xcb_generate_id);
		XPFN_LOAD(xcb_poll_for_event);
	}

	{
		void* hnd = lt_dynl_open("libxcb-ewmh.so");
		if (!hnd)
			lt_ferrf("Failed to load xcb-ewmh: %s\n", lt_dynl_err_str());

		XPFN_LOAD(xcb_ewmh_init_atoms);
		XPFN_LOAD(xcb_ewmh_init_atoms_replies);
	}

	{
		void* hnd = lt_dynl_open("libxcb-randr.so");
		if (!hnd)
			lt_ferrf("Failed to load xcb-randr: %s\n", lt_dynl_err_str());

		XPFN_LOAD(xcb_randr_get_screen_resources_current);
		XPFN_LOAD(xcb_randr_get_screen_resources_current_reply);
		XPFN_LOAD(xcb_randr_get_screen_resources_current_outputs_length);
		XPFN_LOAD(xcb_randr_get_screen_resources_current_outputs);
		XPFN_LOAD(xcb_randr_get_output_info);
		XPFN_LOAD(xcb_randr_get_output_info_reply);
		XPFN_LOAD(xcb_randr_get_crtc_info);
		XPFN_LOAD(xcb_randr_get_crtc_info_reply);
		XPFN_LOAD(xcb_randr_get_output_info_name_length);
		XPFN_LOAD(xcb_randr_get_output_info_name);
	}

	{
		void* hnd = lt_dynl_open("libX11.so");
		if (!hnd)
			lt_ferrf("Failed to load Xlib: %s\n", lt_dynl_err_str());

		XPFN_LOAD(XOpenDisplay);
		XPFN_LOAD(XCloseDisplay);
		XPFN_LOAD(XkbSetDetectableAutoRepeat);
		XPFN_LOAD(XkbGetNames);
		XPFN_LOAD(XkbGetMap);
		XPFN_LOAD(XkbFreeKeyboard);
		XPFN_LOAD(XkbFreeNames);
	}

	{
		void* hnd = lt_dynl_open("libX11-xcb.so");
		if (!hnd)
			lt_ferrf("Failed to load Xlib-xcb: %s\n", lt_dynl_err_str());

		XPFN_LOAD(XGetXCBConnection);
		XPFN_LOAD(XSetEventQueueOwner);
	}
}
