#include "window_def.h"

#if defined(LT_WIN32)
#	include <lt/mem.h>
#	include <lt/io.h>
#	include <windowsx.h>

HINSTANCE lt_hinst = NULL;
static char* lt_class_name = "Sample Window Class";
static int lt_output_count = 0;
static lt_output_t* lt_outputs = NULL;

static int* lt_keytab = NULL;

EXTERN_C IMAGE_DOS_HEADER __ImageBase;
#define HINST_THISCOMPONENT ((HINSTANCE)&__ImageBase)

static inline
lt_window_t* lt_window_from_hwnd(HWND hwnd) {
	return (lt_window_t*)GetWindowLongPtr(hwnd, GWLP_USERDATA);
}

static
LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM w_param, LPARAM l_param) {
	lt_window_t* win = lt_window_from_hwnd(hwnd);

	switch (msg) {
	case WM_CREATE: {
		CREATESTRUCT* create = (CREATESTRUCT*)l_param;
		lt_window_t* win = (lt_window_t*)create->lpCreateParams;
		SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)win);

		if (win->with_opengl) {
			PIXELFORMATDESCRIPTOR pfd = {0};
			pfd.nSize = sizeof(pfd);
			pfd.nVersion = 1;
			pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
			pfd.iPixelType = PFD_TYPE_RGBA;
			pfd.cColorBits = 32;
			pfd.cDepthBits = 24;
			pfd.cStencilBits = 8;
			pfd.iLayerType = PFD_MAIN_PLANE;

			HDC dc = GetDC(hwnd);
			int px_fmt = ChoosePixelFormat(dc, &pfd);
			LT_ASSERT(px_fmt);
			LT_ASSERT(SetPixelFormat(dc, px_fmt, &pfd));

			HGLRC glc = wglCreateContext(dc);
			LT_ASSERT(glc);
			LT_ASSERT(wglMakeCurrent(dc, glc));
			win->glctx = glc;
		}
	}	return 0;

	case WM_CLOSE:
		DestroyWindow(hwnd);
		return 0;

	case WM_DESTROY:
		win->closed = 1;
		if (win->glctx) {
			wglMakeCurrent(GetDC(hwnd), NULL);
			wglDeleteContext(win->glctx);
		}
		return 0;

	case WM_MOUSEMOVE:
		win->mpos_x = GET_X_LPARAM(l_param);
		win->mpos_y = GET_Y_LPARAM(l_param);

	default:
		return DefWindowProc(hwnd, msg, w_param, l_param);
	}
}

static void lt_generate_keytab(lt_arena_t* arena);

b8 lt_window_init(lt_arena_t* arena) {
	lt_hinst = GetModuleHandle(NULL);

	WNDCLASSEX wc;
	wc.cbSize = sizeof(wc);
	wc.style = CS_OWNDC;
	wc.lpfnWndProc = WindowProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = lt_hinst;
	wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = GetStockObject(WHITE_BRUSH);
	wc.lpszMenuName = NULL;
	wc.lpszClassName = lt_class_name;
	wc.hIconSm = LoadIcon(NULL, IDI_APPLICATION);

	if (!RegisterClassEx(&wc))
		return 0;


	DISPLAY_DEVICE dev;
	memset(&dev, 0, sizeof(dev));
	dev.cb = sizeof(dev);

	int output_count = 0;
	while (EnumDisplayDevices(NULL, output_count, &dev, 0)) {
		++output_count;
		memset(&dev, 0, sizeof(dev));
		dev.cb = sizeof(dev);
	}

	lt_output_count = output_count;
	lt_outputs = lt_arena_reserve(arena, output_count * sizeof(lt_output_t));

	for (int i = 0; i < output_count; ++i) {
		EnumDisplayDevices(NULL, i, &dev, 0);

		int dev_name_len = strlen(dev.DeviceName);
		char* dev_name = lt_arena_reserve(arena, dev_name_len);
		memcpy(dev_name, dev.DeviceName, dev_name_len);

		DEVMODE dev_mode;
		memset(&dev_mode, 0, sizeof(dev_mode));
		dev_mode.dmSize = sizeof(dev_mode);
		EnumDisplaySettingsEx(dev.DeviceName, ENUM_CURRENT_SETTINGS, &dev_mode, 0);

		lt_outputs[i].x = dev_mode.dmPosition.x;
		lt_outputs[i].y = dev_mode.dmPosition.y;
		lt_outputs[i].w = dev_mode.dmPelsWidth;
		lt_outputs[i].h = dev_mode.dmPelsHeight;
		lt_outputs[i].name = LSTR(dev_name, dev_name_len);
		lt_outputs[i].rate = dev_mode.dmDisplayFrequency;

		memset(&dev, 0, sizeof(dev));
		dev.cb = sizeof(dev);
	}

	lt_generate_keytab(arena);
	return 1;
}

void lt_window_terminate(void) {
	UnregisterClass(lt_class_name, lt_hinst);
	PostQuitMessage(0);
}

int lt_window_output_count(void) {
	return lt_output_count;
}

lt_output_t* lt_window_outputs(void) {
	return lt_outputs;
}

lt_window_t* lt_window_create(lt_arena_t* arena, lt_window_description_t* desc) {
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

	lt_window_t* win = lt_arena_reserve(arena, sizeof(lt_window_t));
	win->closed = 0;
	win->fullscreen = 0;
	win->glctx = NULL;
	win->with_opengl = !!(desc->type & LT_WIN_GL);

	// Create window
	HWND hwnd = CreateWindowEx(0, lt_class_name, desc->title.str, WS_OVERLAPPEDWINDOW,
			x, y, w, h, NULL, NULL, lt_hinst, win);
	if (!hwnd)
		return NULL;

	win->hwnd = hwnd;

	// Show window
	ShowWindow(hwnd, SW_SHOWDEFAULT);
	UpdateWindow(hwnd);
	return win;
}

void lt_window_destroy(lt_window_t* win) {

}

usz lt_window_poll_events(lt_window_t* win, lt_window_event_t* evs, usz ev_max) {
	memcpy(win->old_key_press_map, win->key_press_map, sizeof(win->key_press_map));

	usz ev_it = 0;

	MSG msg;
	memset(&msg, 0, sizeof(msg));
	while (PeekMessage(&msg, win->hwnd, 0, 0, PM_REMOVE)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);

		int action = 0, button = 0;

		// TODO: Handle WM_XBUTTONDOWN events

		switch (msg.message) {
		case WM_LBUTTONDOWN: button = LT_KEY_MB1, action = 1; goto mb_common;
		case WM_MBUTTONDOWN: button = LT_KEY_MB3, action = 1; goto mb_common;
		case WM_RBUTTONDOWN: button = LT_KEY_MB2, action = 1; goto mb_common;

		case WM_LBUTTONUP: button = LT_KEY_MB1, action = 0; goto mb_common;
		case WM_MBUTTONUP: button = LT_KEY_MB3, action = 0; goto mb_common;
		case WM_RBUTTONUP: button = LT_KEY_MB2, action = 0; goto mb_common;

		mb_common:
			win->key_press_map[button] = action;
			if (ev_it < ev_max) {
				evs[ev_it].type = action ? LT_WIN_EVENT_BUTTON_PRESS : LT_WIN_EVENT_BUTTON_RELEASE;
				evs[ev_it].button = button;
				++ev_it;
			}
			break;

		default:
			break;
		}

		if (msg.message == WM_KEYDOWN || msg.message == WM_SYSKEYDOWN ||
			msg.message == WM_KEYUP || msg.message == WM_SYSKEYUP)
		{
			const int action = (HIWORD(msg.lParam) & KF_UP) ? 0 : 1;
			//const int mods = getKeyMods();
			int scancode = (HIWORD(msg.lParam) & (KF_EXTENDED | 0xff));
			if (!scancode)
				scancode = MapVirtualKeyW((UINT)msg.wParam, MAPVK_VK_TO_VSC);

			int key = lt_keytab[scancode];

			if (msg.wParam == VK_CONTROL) {
				if (HIWORD(msg.lParam) & KF_EXTENDED)
					key = LT_KEY_RIGHT_CTRL;
				else {
					MSG next;
					const DWORD time = GetMessageTime();
					if (PeekMessage(&next, NULL, 0, 0, PM_NOREMOVE)) {
						if (msg.message == WM_KEYDOWN || msg.message == WM_SYSKEYDOWN ||
							msg.message == WM_KEYUP || msg.message == WM_SYSKEYUP)
						{
							if (next.wParam == VK_MENU && HIWORD(next.lParam) & KF_EXTENDED && next.time == time)
								break;
						}
					}

					key = LT_KEY_LEFT_CTRL;
				}
			}

			if (action == 0 && msg.wParam == VK_SHIFT) {
				win->key_press_map[LT_KEY_RIGHT_SHIFT] = 0;
				win->key_press_map[LT_KEY_LEFT_SHIFT] = 0;
			}
			else {
				win->key_press_map[key] = action;
				if (ev_it < ev_max) {
					evs[ev_it].type = action ? LT_WIN_EVENT_KEY_PRESS : LT_WIN_EVENT_KEY_RELEASE;
					evs[ev_it].key = key;
					++ev_it;
				}
			}
		}
	}

	return ev_it;
}

void lt_window_set_fullscreen(lt_window_t* win, lt_winstate_t state) {
	if (state == LT_WIN_SET) {
		win->fullscreen = 1;

		DWORD style = GetWindowLong(win->hwnd, GWL_STYLE);
		SetWindowLong(win->hwnd, GWL_STYLE, style & ~WS_MAXIMIZE);

		MONITORINFO mi;
		mi.cbSize = sizeof(mi);

		memset(&win->wp, 0, sizeof(win->wp));
		win->wp.length = sizeof(win->wp);
		if (GetWindowPlacement(win->hwnd, &win->wp) &&
			GetMonitorInfo(MonitorFromWindow(win->hwnd, MONITOR_DEFAULTTOPRIMARY), &mi))
		{
			DWORD style = GetWindowLong(win->hwnd, GWL_STYLE);
			SetWindowLong(win->hwnd, GWL_STYLE, style & ~WS_OVERLAPPEDWINDOW);

			int x = mi.rcMonitor.left, y = mi.rcMonitor.top;
			int w = mi.rcMonitor.right - mi.rcMonitor.left, h = mi.rcMonitor.bottom - mi.rcMonitor.top;
			SetWindowPos(win->hwnd, NULL, x, y, w, h, SWP_NOOWNERZORDER | SWP_FRAMECHANGED);
		}
	}
	else if (state == LT_WIN_CLEAR) {
		win->fullscreen = 0;

		DWORD style = GetWindowLong(win->hwnd, GWL_STYLE);
		SetWindowLong(win->hwnd, GWL_STYLE, style | WS_OVERLAPPEDWINDOW);
		SetWindowPlacement(win->hwnd, &win->wp);
		SetWindowPos(win->hwnd, NULL, 0, 0, 0, 0,
				SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_NOOWNERZORDER | SWP_FRAMECHANGED);
	}
	else if (state == LT_WIN_TOGGLE)
		lt_window_set_fullscreen(win, !win->fullscreen);
}

void lt_window_gl_swap_buffers(lt_window_t* win) {
	wglSwapLayerBuffers(GetDC(win->hwnd), WGL_SWAP_MAIN_PLANE);
}

void lt_window_set_pos(lt_window_t* win, int x, int y) {
	SetWindowPos(win->hwnd, NULL, x, y, 0, 0, SWP_NOSIZE);
}

void lt_window_set_size(lt_window_t* win, int w, int h) {
	SetWindowPos(win->hwnd, NULL, 0, 0, w, h, SWP_NOMOVE);
}

void lt_window_get_pos(lt_window_t* win, int* x, int* y) {
	RECT rect;
	GetWindowRect(win->hwnd, &rect);
	*x = rect.left;
	*y = rect.top;
}

void lt_window_get_size(lt_window_t* win, int* w, int* h) {
	RECT rect;
	GetClientRect(win->hwnd, &rect);
	*w = rect.right - rect.left;
	*h = rect.bottom - rect.top;
}


static
void lt_generate_keytab(lt_arena_t* arena) {
	lt_keytab = lt_arena_reserve(arena, 512 * sizeof(int));
	memset(lt_keytab, LT_KEY_INVALID, 512 * sizeof(int));

	lt_keytab[0x00B] = LT_KEY_0; lt_keytab[0x002] = LT_KEY_1; lt_keytab[0x003] = LT_KEY_2;
	lt_keytab[0x004] = LT_KEY_3; lt_keytab[0x005] = LT_KEY_4; lt_keytab[0x006] = LT_KEY_5;
	lt_keytab[0x007] = LT_KEY_6; lt_keytab[0x008] = LT_KEY_7; lt_keytab[0x009] = LT_KEY_8;
	lt_keytab[0x00A] = LT_KEY_9;

	lt_keytab[0x01E] = LT_KEY_A; lt_keytab[0x030] = LT_KEY_B; lt_keytab[0x02E] = LT_KEY_C;
	lt_keytab[0x020] = LT_KEY_D; lt_keytab[0x012] = LT_KEY_E; lt_keytab[0x021] = LT_KEY_F;
	lt_keytab[0x022] = LT_KEY_G; lt_keytab[0x023] = LT_KEY_H; lt_keytab[0x017] = LT_KEY_I;
	lt_keytab[0x024] = LT_KEY_J; lt_keytab[0x025] = LT_KEY_K; lt_keytab[0x026] = LT_KEY_L;
	lt_keytab[0x032] = LT_KEY_M; lt_keytab[0x031] = LT_KEY_N; lt_keytab[0x018] = LT_KEY_O;
	lt_keytab[0x019] = LT_KEY_P; lt_keytab[0x010] = LT_KEY_Q; lt_keytab[0x013] = LT_KEY_R;
	lt_keytab[0x01F] = LT_KEY_S; lt_keytab[0x014] = LT_KEY_T; lt_keytab[0x016] = LT_KEY_U;
	lt_keytab[0x02F] = LT_KEY_V; lt_keytab[0x011] = LT_KEY_W; lt_keytab[0x02D] = LT_KEY_X;
	lt_keytab[0x015] = LT_KEY_Y; lt_keytab[0x02C] = LT_KEY_Z;

	lt_keytab[0x028] = LT_KEY_APOSTROPHE; lt_keytab[0x02B] = LT_KEY_BACKSLASH;
	lt_keytab[0x033] = LT_KEY_COMMA; lt_keytab[0x00D] = LT_KEY_EQUAL;
	lt_keytab[0x029] = LT_KEY_TILDE; lt_keytab[0x01A] = LT_KEY_LBRACKET;
	lt_keytab[0x00C] = LT_KEY_MINUS; lt_keytab[0x034] = LT_KEY_PERIOD;
	lt_keytab[0x01B] = LT_KEY_RBRACKET; lt_keytab[0x027] = LT_KEY_SEMICOLON;
	lt_keytab[0x035] = LT_KEY_SLASH;
//	lt_keytab[0x056] = LT_KEY_WORLD_2;

	lt_keytab[0x00E] = LT_KEY_BACKSPACE; lt_keytab[0x153] = LT_KEY_DELETE;
	lt_keytab[0x14F] = LT_KEY_END; lt_keytab[0x01C] = LT_KEY_ENTER;
	lt_keytab[0x001] = LT_KEY_ESC; lt_keytab[0x147] = LT_KEY_HOME;
	lt_keytab[0x152] = LT_KEY_INSERT; lt_keytab[0x15D] = LT_KEY_MENU;
	lt_keytab[0x151] = LT_KEY_PAGE_DOWN; lt_keytab[0x149] = LT_KEY_PAGE_UP;
	lt_keytab[0x045] = LT_KEY_PAUSE; lt_keytab[0x146] = LT_KEY_PAUSE;
	lt_keytab[0x039] = LT_KEY_SPACE; lt_keytab[0x00F] = LT_KEY_TAB;
	lt_keytab[0x03A] = LT_KEY_CAPS_LOCK; lt_keytab[0x145] = LT_KEY_NUM_LOCK;
	lt_keytab[0x046] = LT_KEY_SCROLL_LOCK;

	lt_keytab[0x03B] = LT_KEY_F1; lt_keytab[0x03C] = LT_KEY_F2; lt_keytab[0x03D] = LT_KEY_F3;
	lt_keytab[0x03E] = LT_KEY_F4; lt_keytab[0x03F] = LT_KEY_F5; lt_keytab[0x040] = LT_KEY_F6;
	lt_keytab[0x041] = LT_KEY_F7; lt_keytab[0x042] = LT_KEY_F8; lt_keytab[0x043] = LT_KEY_F9;
	lt_keytab[0x044] = LT_KEY_F10; lt_keytab[0x057] = LT_KEY_F11; lt_keytab[0x058] = LT_KEY_F12;

	lt_keytab[0x038] = LT_KEY_LEFT_ALT; lt_keytab[0x01D] = LT_KEY_LEFT_CTRL;
	lt_keytab[0x02A] = LT_KEY_LEFT_SHIFT; lt_keytab[0x15B] = LT_KEY_LEFT_SUPER;
	lt_keytab[0x137] = LT_KEY_PRINT_SCREEN;
	lt_keytab[0x138] = LT_KEY_RIGHT_ALT; lt_keytab[0x11D] = LT_KEY_RIGHT_CTRL;
	lt_keytab[0x036] = LT_KEY_RIGHT_SHIFT; lt_keytab[0x15C] = LT_KEY_RIGHT_SUPER;
	lt_keytab[0x150] = LT_KEY_DOWN; lt_keytab[0x14B] = LT_KEY_LEFT;
	lt_keytab[0x14D] = LT_KEY_RIGHT; lt_keytab[0x148] = LT_KEY_UP;

	lt_keytab[0x052] = LT_KEY_NP_0; lt_keytab[0x04F] = LT_KEY_NP_1; lt_keytab[0x050] = LT_KEY_NP_2;
	lt_keytab[0x051] = LT_KEY_NP_3; lt_keytab[0x04B] = LT_KEY_NP_4; lt_keytab[0x04C] = LT_KEY_NP_5;
	lt_keytab[0x04D] = LT_KEY_NP_6; lt_keytab[0x047] = LT_KEY_NP_7; lt_keytab[0x048] = LT_KEY_NP_8;
	lt_keytab[0x049] = LT_KEY_NP_9;
	lt_keytab[0x04E] = LT_KEY_NP_ADD; lt_keytab[0x053] = LT_KEY_NP_DECIMAL;
	lt_keytab[0x135] = LT_KEY_NP_DIVIDE; lt_keytab[0x11C] = LT_KEY_NP_ENTER;
	lt_keytab[0x059] = LT_KEY_NP_EQUAL; lt_keytab[0x037] = LT_KEY_NP_MULTIPLY;
	lt_keytab[0x04A] = LT_KEY_NP_SUBTRACT;
}

#endif
