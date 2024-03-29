#ifndef LT_XPROTO_H
#define LT_XPROTO_H 1

#include <lt/primitives.h>

#include <xcb/xcb.h>
#include <xcb/xcb_ewmh.h>
#include <xcb/randr.h>

#include <X11/Xlib.h>
#include <X11/Xlib-xcb.h>
#include <X11/XKBlib.h>

#define XPFN_FWD(ret, name, args) extern ret(*lt_xproto_##name)args

// XCB

XPFN_FWD(const xcb_setup_t*, xcb_get_setup, (xcb_connection_t*));
#define xcb_get_setup(...) lt_xproto_xcb_get_setup(__VA_ARGS__)

XPFN_FWD(xcb_void_cookie_t, xcb_map_window, (xcb_connection_t*, xcb_window_t));
#define xcb_map_window(...) lt_xproto_xcb_map_window(__VA_ARGS__)

XPFN_FWD(void, xcb_screen_next, (xcb_screen_iterator_t*));
#define xcb_screen_next(...) lt_xproto_xcb_screen_next(__VA_ARGS__)

XPFN_FWD(xcb_void_cookie_t, xcb_change_property, (xcb_connection_t*, u8, xcb_window_t, xcb_atom_t, xcb_atom_t, u8, u32, const void*));
#define xcb_change_property(...) lt_xproto_xcb_change_property(__VA_ARGS__)

XPFN_FWD(xcb_void_cookie_t, xcb_destroy_window, (xcb_connection_t*, xcb_window_t));
#define xcb_destroy_window(...) lt_xproto_xcb_destroy_window(__VA_ARGS__)

XPFN_FWD(xcb_void_cookie_t, xcb_configure_window, (xcb_connection_t*, xcb_window_t, u16, const void*));
#define xcb_configure_window(...) lt_xproto_xcb_configure_window(__VA_ARGS__)

XPFN_FWD(void, xcb_flush, (xcb_connection_t*));
#define xcb_flush(...) lt_xproto_xcb_flush(__VA_ARGS__)

XPFN_FWD(xcb_generic_error_t*, xcb_request_check, (xcb_connection_t*, xcb_void_cookie_t));
#define xcb_request_check(...) lt_xproto_xcb_request_check(__VA_ARGS__)

XPFN_FWD(xcb_void_cookie_t, xcb_create_window, (xcb_connection_t*, u8, xcb_window_t, xcb_window_t, i16, i16, u16, u16, u16, u16, xcb_visualid_t, u32, const void*));
#define xcb_create_window(...) lt_xproto_xcb_create_window(__VA_ARGS__)

XPFN_FWD(xcb_screen_iterator_t, xcb_setup_roots_iterator, (const xcb_setup_t*));
#define xcb_setup_roots_iterator(...) lt_xproto_xcb_setup_roots_iterator(__VA_ARGS__)

XPFN_FWD(xcb_void_cookie_t, xcb_send_event, (xcb_connection_t*, u8, xcb_window_t, u32, const char*));
#define xcb_send_event(...) lt_xproto_xcb_send_event(__VA_ARGS__)

XPFN_FWD(u32, xcb_generate_id, (xcb_connection_t*));
#define xcb_generate_id(...) lt_xproto_xcb_generate_id(__VA_ARGS__)

XPFN_FWD(xcb_generic_event_t*, xcb_poll_for_event, (xcb_connection_t*));
#define xcb_poll_for_event(...) lt_xproto_xcb_poll_for_event(__VA_ARGS__)

XPFN_FWD(xcb_generic_event_t*, xcb_wait_for_event, (xcb_connection_t*));
#define xcb_wait_for_event(...) lt_xproto_xcb_wait_for_event(__VA_ARGS__)

XPFN_FWD(xcb_void_cookie_t, xcb_create_gc, (xcb_connection_t*, xcb_gc_t, xcb_drawable_t, u32, u32*));
#define xcb_create_gc(...) lt_xproto_xcb_create_gc(__VA_ARGS__)

XPFN_FWD(xcb_void_cookie_t, xcb_change_gc, (xcb_connection_t*, xcb_gc_t, u32, u32*));
#define xcb_change_gc(...) lt_xproto_xcb_change_gc(__VA_ARGS__)

XPFN_FWD(xcb_void_cookie_t, xcb_poly_rectangle, (xcb_connection_t*, xcb_drawable_t, xcb_gc_t, u32, xcb_rectangle_t*));
#define xcb_poly_rectangle(...) lt_xproto_xcb_poly_rectangle(__VA_ARGS__)

XPFN_FWD(xcb_void_cookie_t, xcb_poly_fill_rectangle, (xcb_connection_t*, xcb_drawable_t, xcb_gc_t, u32, xcb_rectangle_t*));
#define xcb_poly_fill_rectangle(...) lt_xproto_xcb_poly_fill_rectangle(__VA_ARGS__)

XPFN_FWD(xcb_intern_atom_cookie_t, xcb_intern_atom, (xcb_connection_t*, u8, u16, char*));
#define xcb_intern_atom(...) lt_xproto_xcb_intern_atom(__VA_ARGS__)

XPFN_FWD(xcb_intern_atom_reply_t*, xcb_intern_atom_reply, (xcb_connection_t*, xcb_intern_atom_cookie_t, xcb_generic_error_t**));
#define xcb_intern_atom_reply(...) lt_xproto_xcb_intern_atom_reply(__VA_ARGS__)

XPFN_FWD(xcb_void_cookie_t, xcb_convert_selection, (xcb_connection_t*, xcb_window_t, xcb_atom_t, xcb_atom_t, xcb_atom_t, xcb_timestamp_t));
#define xcb_convert_selection(...) lt_xproto_xcb_convert_selection(__VA_ARGS__)

XPFN_FWD(xcb_get_selection_owner_cookie_t, xcb_get_selection_owner, (xcb_connection_t*, xcb_atom_t));
#define xcb_get_selection_owner(...) lt_xproto_xcb_get_selection_owner(__VA_ARGS__)

XPFN_FWD(xcb_get_selection_owner_reply_t*, xcb_get_selection_owner_reply,
		(xcb_connection_t*, xcb_get_selection_owner_cookie_t, xcb_generic_error_t**));
#define xcb_get_selection_owner_reply(...) lt_xproto_xcb_get_selection_owner_reply(__VA_ARGS__)

XPFN_FWD(xcb_void_cookie_t, xcb_set_selection_owner, (xcb_connection_t*, xcb_window_t, xcb_atom_t, xcb_timestamp_t));
#define xcb_set_selection_owner(...) lt_xproto_xcb_set_selection_owner(__VA_ARGS__)

XPFN_FWD(xcb_get_property_cookie_t, xcb_get_property, (xcb_connection_t*, u8, xcb_window_t, xcb_atom_t, xcb_atom_t, u32, u32));
#define xcb_get_property(...) lt_xproto_xcb_get_property(__VA_ARGS__)

XPFN_FWD(xcb_get_property_reply_t*, xcb_get_property_reply, (xcb_connection_t*, xcb_get_property_cookie_t, xcb_generic_error_t**));
#define xcb_get_property_reply(...) lt_xproto_xcb_get_property_reply(__VA_ARGS__)

XPFN_FWD(void*, xcb_get_property_value, (const xcb_get_property_reply_t*));
#define xcb_get_property_value(...) lt_xproto_xcb_get_property_value(__VA_ARGS__)

XPFN_FWD(int, xcb_get_property_value_length, (const xcb_get_property_reply_t*));
#define xcb_get_property_value_length(...) lt_xproto_xcb_get_property_value_length(__VA_ARGS__)

XPFN_FWD(xcb_get_atom_name_cookie_t, xcb_get_atom_name, (xcb_connection_t*, xcb_atom_t));
#define xcb_get_atom_name(...) lt_xproto_xcb_get_atom_name(__VA_ARGS__)

XPFN_FWD(xcb_get_atom_name_reply_t*, xcb_get_atom_name_reply, (xcb_connection_t*, xcb_get_atom_name_cookie_t, xcb_generic_error_t**));
#define xcb_get_atom_name_reply(...) lt_xproto_xcb_get_atom_name_reply(__VA_ARGS__)

XPFN_FWD(char*, xcb_get_atom_name_name, (const xcb_get_atom_name_reply_t*));
#define xcb_get_atom_name_name(...) lt_xproto_xcb_get_atom_name_name(__VA_ARGS__)

XPFN_FWD(int, xcb_get_atom_name_name_length, (const xcb_get_atom_name_reply_t*));
#define xcb_get_atom_name_name_length(...) lt_xproto_xcb_get_atom_name_name_length(__VA_ARGS__)

XPFN_FWD(xcb_void_cookie_t, xcb_create_pixmap, (xcb_connection_t*, u8, xcb_pixmap_t, xcb_drawable_t, u16, u16));
#define xcb_create_pixmap(...) lt_xproto_xcb_create_pixmap(__VA_ARGS__)

XPFN_FWD(xcb_void_cookie_t, xcb_create_cursor, (xcb_connection_t*, xcb_cursor_t, xcb_pixmap_t, xcb_pixmap_t, u16, u16, u16, u16, u16, u16, u16, u16));
#define xcb_create_cursor(...) lt_xproto_xcb_create_cursor(__VA_ARGS__)

XPFN_FWD(xcb_void_cookie_t, xcb_change_window_attributes, (xcb_connection_t*, xcb_window_t, u32, const void*));
#define xcb_change_window_attributes(...) lt_xproto_xcb_change_window_attributes(__VA_ARGS__)

// XCB-EWMH

XPFN_FWD(xcb_intern_atom_cookie_t*, xcb_ewmh_init_atoms, (xcb_connection_t*, xcb_ewmh_connection_t*));
#define xcb_ewmh_init_atoms(...) lt_xproto_xcb_ewmh_init_atoms(__VA_ARGS__)

XPFN_FWD(u8, xcb_ewmh_init_atoms_replies, (xcb_ewmh_connection_t*, xcb_intern_atom_cookie_t*, xcb_generic_error_t**));
#define xcb_ewmh_init_atoms_replies(...) lt_xproto_xcb_ewmh_init_atoms_replies(__VA_ARGS__)

XPFN_FWD(xcb_void_cookie_t, xcb_ewmh_set_supported, (xcb_ewmh_connection_t*, int, u32, xcb_atom_t*));
#define xcb_ewmh_set_supported(...) lt_xproto_xcb_ewmh_set_supported(__VA_ARGS__)

// XCB-Randr

XPFN_FWD(xcb_randr_get_screen_resources_current_cookie_t, xcb_randr_get_screen_resources_current,
		(xcb_connection_t*, xcb_window_t));
#define xcb_randr_get_screen_resources_current(...) lt_xproto_xcb_randr_get_screen_resources_current(__VA_ARGS__)

XPFN_FWD(xcb_randr_get_screen_resources_current_reply_t*, xcb_randr_get_screen_resources_current_reply,
		(xcb_connection_t*, xcb_randr_get_screen_resources_current_cookie_t, xcb_generic_error_t**));
#define xcb_randr_get_screen_resources_current_reply(...) lt_xproto_xcb_randr_get_screen_resources_current_reply(__VA_ARGS__)

XPFN_FWD(int, xcb_randr_get_screen_resources_current_outputs_length,
		(const xcb_randr_get_screen_resources_current_reply_t*));
#define xcb_randr_get_screen_resources_current_outputs_length(...) \
		lt_xproto_xcb_randr_get_screen_resources_current_outputs_length(__VA_ARGS__)

XPFN_FWD(xcb_randr_output_t*, xcb_randr_get_screen_resources_current_outputs,
		(const xcb_randr_get_screen_resources_current_reply_t*));
#define xcb_randr_get_screen_resources_current_outputs(...) lt_xproto_xcb_randr_get_screen_resources_current_outputs(__VA_ARGS__)

XPFN_FWD(int, xcb_randr_get_screen_resources_current_modes_length,
		(const xcb_randr_get_screen_resources_current_reply_t*));
#define xcb_randr_get_screen_resources_current_modes_length(...) \
		lt_xproto_xcb_randr_get_screen_resources_current_modes_length(__VA_ARGS__)

XPFN_FWD(xcb_randr_mode_info_t*, xcb_randr_get_screen_resources_current_modes,
		(const xcb_randr_get_screen_resources_current_reply_t*));
#define xcb_randr_get_screen_resources_current_modes(...) lt_xproto_xcb_randr_get_screen_resources_current_modes(__VA_ARGS__)

XPFN_FWD(xcb_randr_get_output_info_cookie_t, xcb_randr_get_output_info, (xcb_connection_t*, xcb_randr_output_t, xcb_timestamp_t));
#define xcb_randr_get_output_info(...) lt_xproto_xcb_randr_get_output_info(__VA_ARGS__)

XPFN_FWD(xcb_randr_get_output_info_reply_t*, xcb_randr_get_output_info_reply,
		(xcb_connection_t*, xcb_randr_get_output_info_cookie_t, xcb_generic_error_t**));
#define xcb_randr_get_output_info_reply(...) lt_xproto_xcb_randr_get_output_info_reply(__VA_ARGS__)

XPFN_FWD(xcb_randr_get_crtc_info_cookie_t, xcb_randr_get_crtc_info, (xcb_connection_t*, xcb_randr_crtc_t, xcb_timestamp_t));
#define xcb_randr_get_crtc_info(...) lt_xproto_xcb_randr_get_crtc_info(__VA_ARGS__)

XPFN_FWD(xcb_randr_get_crtc_info_reply_t*, xcb_randr_get_crtc_info_reply,
		(xcb_connection_t*, xcb_randr_get_crtc_info_cookie_t, xcb_generic_error_t**));
#define xcb_randr_get_crtc_info_reply(...) lt_xproto_xcb_randr_get_crtc_info_reply(__VA_ARGS__)

XPFN_FWD(int, xcb_randr_get_output_info_name_length, (const xcb_randr_get_output_info_reply_t*));
#define xcb_randr_get_output_info_name_length(...) lt_xproto_xcb_randr_get_output_info_name_length(__VA_ARGS__)

XPFN_FWD(u8*, xcb_randr_get_output_info_name, (const xcb_randr_get_output_info_reply_t*));
#define xcb_randr_get_output_info_name(...) lt_xproto_xcb_randr_get_output_info_name(__VA_ARGS__)

// XLib

XPFN_FWD(Display*, XOpenDisplay, (_Xconst char*));
#define XOpenDisplay(...) lt_xproto_XOpenDisplay(__VA_ARGS__)

XPFN_FWD(int, XCloseDisplay, (Display*));
#define XCloseDisplay(...) lt_xproto_XCloseDisplay(__VA_ARGS__)

XPFN_FWD(Colormap, XCreateColormap, (Display*, Window, Visual*, int));
#define XCreateColormap(...) lt_xproto_XCreateColormap(__VA_ARGS__)

XPFN_FWD(Colormap, XFreeColormap, (Display*, Colormap));
#define XFreeColormap(...) lt_xproto_XFreeColormap(__VA_ARGS__)

XPFN_FWD(Bool, XkbSetDetectableAutoRepeat, (Display*, Bool, Bool*));
#define XkbSetDetectableAutoRepeat(...) lt_xproto_XkbSetDetectableAutoRepeat(__VA_ARGS__)

XPFN_FWD(Atom, XInternAtom, (Display*, char*, Bool));
#define XInternAtom(...) lt_xproto_XInternAtom(__VA_ARGS__)

XPFN_FWD(Window, XCreateSimpleWindow, (Display*, Window, int, int, unsigned int, unsigned int, unsigned int, unsigned long, unsigned long));
#define XCreateSimpleWindow(...) lt_xproto_XCreateSimpleWindow(__VA_ARGS__)

XPFN_FWD(int, XDestroyWindow, (Display*, Window));
#define XDestroyWindow(...) lt_xproto_XDestroyWindow(__VA_ARGS__)

XPFN_FWD(int, XConvertSelection, (Display*, Atom, Atom, Atom, Window, Time));
#define XConvertSelection(...) lt_xproto_XConvertSelection(__VA_ARGS__)

XPFN_FWD(int, XGetWindowProperty, (Display*, Window, Atom, long, long, Bool, Atom, Atom*, int*, unsigned long*, unsigned long*, unsigned char**));
#define XGetWindowProperty(...) lt_xproto_XGetWindowProperty(__VA_ARGS__)

XPFN_FWD(int, XDeleteProperty, (Display*, Window, Atom));
#define XDeleteProperty(...) lt_xproto_XDeleteProperty(__VA_ARGS__)

XPFN_FWD(int, XFree, (void*));
#define XFree(...) lt_xproto_XFree(__VA_ARGS__)

XPFN_FWD(int, XNextEvent, (Display*, XEvent*));
#define XNextEvent(...) lt_xproto_XNextEvent(__VA_ARGS__)

XPFN_FWD(Atom, XGetSelectionOwner, (Display*, Atom));
#define XGetSelectionOwner(...) lt_xproto_XGetSelectionOwner(__VA_ARGS__)

XPFN_FWD(int, XGetErrorText, (Display*, int, char*, int));
#define XGetErrorText(...) lt_xproto_XGetErrorText(__VA_ARGS__)

XPFN_FWD(Status, XkbGetNames, (Display*, unsigned int, XkbDescPtr));
#define XkbGetNames(...) lt_xproto_XkbGetNames(__VA_ARGS__)

XPFN_FWD(XkbDescPtr, XkbGetMap, (Display*, unsigned int, unsigned int));
#define XkbGetMap(...) lt_xproto_XkbGetMap(__VA_ARGS__)

XPFN_FWD(void, XkbFreeKeyboard, (XkbDescPtr, unsigned int, Bool));
#define XkbFreeKeyboard(...) lt_xproto_XkbFreeKeyboard(__VA_ARGS__)

XPFN_FWD(void, XkbFreeNames, (XkbDescPtr, unsigned int, Bool));
#define XkbFreeNames(...) lt_xproto_XkbFreeNames(__VA_ARGS__)

// XLib-XCB

XPFN_FWD(xcb_connection_t*, XGetXCBConnection, (Display*));
#define XGetXCBConnection(...) lt_xproto_XGetXCBConnection(__VA_ARGS__)

XPFN_FWD(void, XSetEventQueueOwner, (Display*, enum XEventQueueOwner));
#define XSetEventQueueOwner(...) lt_xproto_XSetEventQueueOwner(__VA_ARGS__)

// GLX

XPFN_FWD(XVisualInfo*, glXChooseVisual, (Display*, int, int*));
#define glXChooseVisual(...) lt_xproto_glXChooseVisual(__VA_ARGS__)

XPFN_FWD(GLXContext, glXCreateContext, (Display*, XVisualInfo*, GLXContext, Bool));
#define glXCreateContext(...) lt_xproto_glXCreateContext(__VA_ARGS__)

XPFN_FWD(void, glXDestroyContext, (Display*, GLXContext));
#define glXDestroyContext(...) lt_xproto_glXDestroyContext(__VA_ARGS__)

XPFN_FWD(Bool, glXMakeCurrent, (Display*, GLXDrawable, GLXContext));
#define glXMakeCurrent(...) lt_xproto_glXMakeCurrent(__VA_ARGS__)

XPFN_FWD(void, glXSwapBuffers, (Display*, GLXDrawable));
#define glXSwapBuffers(...) lt_xproto_glXSwapBuffers(__VA_ARGS__)

#undef XPFN_FWD

void lt_xproto_init(void);

#endif
