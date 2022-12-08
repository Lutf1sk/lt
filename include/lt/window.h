#ifndef LT_WINDOW_H
#define LT_WINDOW_H 1

#include <lt/fwd.h>
#include <lt/err.h>

typedef struct lt_window lt_window_t;

typedef
enum lt_window_flags {
	LT_WIN_RESIZEABLE = 1,
	LT_WIN_FULLSCREEN = 2,
} lt_window_flags_t;

typedef
enum lt_keycode {
	LT_KEY_INVALID = 0,

	/* Printable keys */
	LT_KEY_PRINTABLE_MIN = ' ',
	LT_KEY_SPACE = ' ', LT_KEY_APOSTROPHE = '\'', LT_KEY_COMMA = ',',
	LT_KEY_MINUS = '-', LT_KEY_PERIOD = '.', LT_KEY_SLASH = '/',

	LT_KEY_0 = '0', LT_KEY_1 = '1', LT_KEY_2 = '2', LT_KEY_3 = '3',
	LT_KEY_4 = '4', LT_KEY_5 = '5', LT_KEY_6 = '6', LT_KEY_7 = '7',
	LT_KEY_8 = '8', LT_KEY_9 = '9',

	LT_KEY_SEMICOLON = ';', LT_KEY_EQUAL = '=',

	LT_KEY_A = 'A', LT_KEY_B = 'B', LT_KEY_C = 'C', LT_KEY_D = 'D',
	LT_KEY_E = 'E', LT_KEY_F = 'F', LT_KEY_G = 'G', LT_KEY_H = 'H',
	LT_KEY_I = 'I', LT_KEY_J = 'J', LT_KEY_K = 'K', LT_KEY_L = 'L',
	LT_KEY_M = 'M', LT_KEY_N = 'N', LT_KEY_O = 'O', LT_KEY_P = 'P',
	LT_KEY_Q = 'Q', LT_KEY_R = 'R', LT_KEY_S = 'S', LT_KEY_T = 'T',
	LT_KEY_U = 'U', LT_KEY_V = 'V', LT_KEY_W = 'W', LT_KEY_X = 'X',
	LT_KEY_Y = 'Y', LT_KEY_Z = 'Z',

	LT_KEY_LBRACKET = '[', LT_KEY_BACKSLASH = '\\', LT_KEY_RBRACKET = ']',
	LT_KEY_TILDE = '`',

	LT_KEY_PRINTABLE_MAX = '`',

	/* Non-printable keys */
	LT_KEY_ESC = 256, LT_KEY_ENTER = 257, LT_KEY_TAB = 258,
	LT_KEY_BACKSPACE = 259, LT_KEY_INSERT = 260, LT_KEY_DELETE = 261,

	LT_KEY_RIGHT = 262, LT_KEY_LEFT = 263, LT_KEY_DOWN = 264, LT_KEY_UP = 265,

	LT_KEY_PAGE_UP = 266, LT_KEY_PAGE_DOWN = 267,
	LT_KEY_HOME = 268, LT_KEY_END = 269,

	LT_KEY_CAPS_LOCK = 280, LT_KEY_SCROLL_LOCK = 281, LT_KEY_NUM_LOCK = 282,
	LT_KEY_PRINT_SCREEN = 283, LT_KEY_PAUSE = 284,

	LT_KEY_F1 = 290, LT_KEY_F2 = 291, LT_KEY_F3 = 292, LT_KEY_F4 = 293,
	LT_KEY_F5 = 294, LT_KEY_F6 = 295, LT_KEY_F7 = 296, LT_KEY_F8 = 297,
	LT_KEY_F9 = 298, LT_KEY_F10 = 299, LT_KEY_F11 = 300, LT_KEY_F12 = 301,

	LT_KEY_NP_0 = 320, LT_KEY_NP_1 = 321, LT_KEY_NP_2 = 322, LT_KEY_NP_3 = 323,
	LT_KEY_NP_4 = 324, LT_KEY_NP_5 = 325, LT_KEY_NP_6 = 326, LT_KEY_NP_7 = 327,
	LT_KEY_NP_8 = 328, LT_KEY_NP_9 = 329,
	LT_KEY_NP_DECIMAL = 330, LT_KEY_NP_DIVIDE = 331, LT_KEY_NP_MULTIPLY = 332,
	LT_KEY_NP_SUBTRACT = 333, LT_KEY_NP_ADD = 334, LT_KEY_NP_ENTER = 335,
	LT_KEY_NP_EQUAL = 336,

	LT_KEY_LEFT_SHIFT = 340, LT_KEY_LEFT_CTRL = 341, LT_KEY_LEFT_ALT = 342,
	LT_KEY_LEFT_SUPER = 343,

	LT_KEY_RIGHT_SHIFT = 344, LT_KEY_RIGHT_CTRL = 345, LT_KEY_RIGHT_ALT = 346,
	LT_KEY_RIGHT_SUPER = 347,

	LT_KEY_MENU = 348,

	LT_KEY_MB1 = 349, LT_KEY_MB2 = 350, LT_KEY_MB3 = 351, LT_KEY_MB4 = 352,
	LT_KEY_MB5 = 353, LT_KEY_MB6 = 354, LT_KEY_MB7 = 355, LT_KEY_MB8 = 356,

	LT_KEY_MAX = 357,
} lt_keycode_t;

typedef
enum lt_winstate {
	LT_WIN_CLEAR = 0,
	LT_WIN_SET = 1,
	LT_WIN_TOGGLE = 2,
} lt_winstate_t;

typedef
enum lt_wintype {
	LT_WIN_GL = 1,
	LT_WIN_VK = 2,
	LT_WIN_SOFT = 4,
} lt_wintype_t;

typedef
struct lt_output {
	int x, y;
	int w, h;
	short rate;
	lstr_t name;
} lt_output_t;

typedef
struct lt_window_description {
	lstr_t title;
	int x, y;
	int w, h;
	int output_index;
	lt_wintype_t type;
} lt_window_description_t;

typedef
enum lt_window_event_type {
	LT_WIN_EVENT_KEY_PRESS,
	LT_WIN_EVENT_KEY_RELEASE,
	LT_WIN_EVENT_BUTTON_PRESS,
	LT_WIN_EVENT_BUTTON_RELEASE,
	LT_WIN_EVENT_MOTION,
} lt_window_event_type_t;

typedef
struct lt_window_event {
	lt_window_event_type_t type;
	union {
		lt_keycode_t key;
		lt_keycode_t button;
	};
} lt_window_event_t;

// x11.c / win32.c
lt_err_t lt_window_init(lt_alloc_t* alloc);
void lt_window_terminate(lt_alloc_t* alloc);

/*void lt_window_set_clipboard(lstr_t str);*/
lstr_t lt_window_get_clipboard(lt_alloc_t* alloc);

int lt_window_output_count(void);
lt_output_t* lt_window_outputs(void);

lt_window_t* lt_window_create(lt_window_description_t* desc, lt_alloc_t* alloc);
void lt_window_destroy(lt_window_t* win, lt_alloc_t* alloc);

usz lt_window_poll_events(lt_window_t* win, lt_window_event_t* events, usz max_events);
usz lt_window_wait_events(lt_window_t* win, lt_window_event_t* events, usz max_events);

void lt_window_set_fullscreen(lt_window_t* win, lt_winstate_t fullscreen);
void lt_window_set_pos(lt_window_t* win, int x, int y);
void lt_window_set_size(lt_window_t* win, int w, int h);

void lt_window_get_pos(lt_window_t* win, int* x, int* y);
void lt_window_get_size(lt_window_t* win, int* w, int* h);

void lt_window_gl_swap_buffers(lt_window_t* win);

// window.c
b8 lt_window_closed(lt_window_t* win);

void lt_window_mouse_pos(lt_window_t* win, int* x, int* y);
b8 lt_window_key_pressed(lt_window_t* win, lt_keycode_t keycode);
b8 lt_window_key_released(lt_window_t* win, lt_keycode_t keycode);
b8 lt_window_key_held(lt_window_t* win, lt_keycode_t keycode);

#endif
