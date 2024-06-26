#include <lt/lt.h>
#include <lt/term.h>
#include <lt/text.h>
#include <lt/internal.h>

#if defined(LT_UNIX)
#	include <termios.h>
#	include <unistd.h>
#	include <poll.h>
#	include <signal.h>
#	include <sys/ioctl.h>
#	include <ctype.h>
#	include <fcntl.h>

static lt_term_flags_t term_flags;
static struct termios old_term;

u32 lt_term_width = 0, lt_term_height = 0;
i32 lt_term_mouse_x = 0, lt_term_mouse_y = 0;

static volatile u8 resized = 0;

static
void lt_handle_winch(int sig) {
	resized = 1;
}

lt_err_t lt_term_init(lt_term_flags_t flags) {
	lt_err_t err;

	term_flags = flags;

	struct termios term;
	if (tcgetattr(STDOUT_FILENO, &term) < 0)
		return lt_errno();
	old_term = term;

	if (!(flags & LT_TERM_CANON)) {
		term.c_iflag &= ~(IGNBRK | BRKINT | PARMRK | ISTRIP | INLCR | IGNCR | IXON);
		term.c_iflag |= ICRNL;
		term.c_lflag &= ~(ECHO | ECHONL | ICANON | ISIG | IEXTEN);
		term.c_cflag &= ~(CSIZE | PARENB);
		term.c_cflag |= CS8;
	}
	else {
		term.c_lflag |= ICANON;
	}

	if (!(flags & LT_TERM_ECHO))
		term.c_lflag &= ~ECHO;
	else
		term.c_lflag |= ECHO;

	if (tcsetattr(STDOUT_FILENO, TCSANOW, &term) < 0)
		return lt_errno();
	if (tcflush(STDOUT_FILENO, TCIOFLUSH) < 0)
		fail_to(err = lt_errno(), err0);

	if ((flags & LT_TERM_MOUSE) && lt_term_write_direct("\x1B[?1003h", 8) < 0)
		fail_to(err = lt_errno(), err0);
	if ((flags & LT_TERM_BPASTE) && lt_term_write_direct("\x1b[?2004h", 8) < 0)
		fail_to(err = lt_errno(), err0);
	if ((flags & LT_TERM_ALTBUF) && lt_term_write_direct("\x1b[?1049h", 8) < 0)
		fail_to(err = lt_errno(), err0);

	struct sigaction sact = {0};
	sact.sa_handler = lt_handle_winch;
	if (sigaction(SIGWINCH, &sact, NULL) < 0)
		fail_to(err = lt_errno(), err0);

	return lt_update_term_dimensions();

err0:	lt_term_restore();
		return err;
}

void lt_term_restore(void) {
	tcsetattr(STDOUT_FILENO, TCSANOW, &old_term);
	tcflush(STDOUT_FILENO, TCIOFLUSH);

	if (term_flags & LT_TERM_MOUSE)
		lt_term_write_direct("\x1B[?1003l", 8);
	if (term_flags & LT_TERM_BPASTE)
		lt_term_write_direct("\x1b[?2004l", 8);
	if (term_flags & LT_TERM_ALTBUF)
		lt_term_write_direct("\x1b[?1049l", 8);
}

lt_err_t lt_update_term_dimensions(void) {
	int fd = open("/dev/tty", O_RDWR);
	if (fd < 0) {
		return lt_errno();
	}

	struct winsize wsz;
	int res = ioctl(fd, TIOCGWINSZ, &wsz);
	close(fd);
	if (res < 0) {
		return lt_errno();
	}

	lt_term_width = wsz.ws_col;
	lt_term_height = wsz.ws_row;
	return LT_SUCCESS;
}

static
int lt_getc(void) {
	unsigned char c;
	if (read(STDIN_FILENO, &c, 1) < 0)
		return 0;
	return c;
}

static
u8 is_param_byte(u8 c) {
	return (c >= 0x30) && (c <= 0x3F);
}

static
u8 is_intermediate_byte(u8 c) {
	return (c >= 0x20) && (c <= 0x2F);
}

static
u32 convert_xterm_key(u32 c) {
	switch (c) {
	case 'A': return LT_TERM_KEY_UP;
	case 'B': return LT_TERM_KEY_DOWN;
	case 'C': return LT_TERM_KEY_RIGHT;
	case 'D': return LT_TERM_KEY_LEFT;
	case 'H': return LT_TERM_KEY_HOME;
	case 'F': return LT_TERM_KEY_END;
	case 'P': return LT_TERM_KEY_F1;
	case 'Q': return LT_TERM_KEY_F2;
	case 'R': return LT_TERM_KEY_F3;
	case 'S': return LT_TERM_KEY_F4;
	case 'Z': return LT_TERM_KEY_TAB | LT_TERM_MOD_SHIFT;
	default: return 0;
	}
}

static
u32 convert_vt_key(u32 c) {
	switch (c) {
	case 1: return LT_TERM_KEY_HOME;
	case 2: return LT_TERM_KEY_INSERT;
	case 3: return LT_TERM_KEY_DELETE;
	case 4: return LT_TERM_KEY_END;
	case 5: return LT_TERM_KEY_PAGEUP;
	case 6: return LT_TERM_KEY_PAGEDN;
	case 7: return LT_TERM_KEY_HOME;
	case 8: return LT_TERM_KEY_END;
	case 9: return 0;
	case 10: return LT_TERM_KEY_F0;
	case 11: return LT_TERM_KEY_F1;
	case 12: return LT_TERM_KEY_F2;
	case 13: return LT_TERM_KEY_F3;
	case 14: return LT_TERM_KEY_F4;
	case 15: return LT_TERM_KEY_F5;
	case 16: return 0;
	case 17: return LT_TERM_KEY_F6;
	case 18: return LT_TERM_KEY_F7;
	case 19: return LT_TERM_KEY_F8;
	case 20: return LT_TERM_KEY_F9;
	case 21: return LT_TERM_KEY_F10;
	case 22: return 0;
	case 23: return LT_TERM_KEY_F11;
	case 24: return LT_TERM_KEY_F12;
	case 25: return LT_TERM_KEY_F13;
	case 26: return LT_TERM_KEY_F14;
	case 27: return 0;
	case 28: return LT_TERM_KEY_F15;
	case 29: return LT_TERM_KEY_F16;
	case 30: return 0;
	case 31: return LT_TERM_KEY_F17;
	case 32: return LT_TERM_KEY_F18;
	case 33: return LT_TERM_KEY_F19;
	case 34: return LT_TERM_KEY_F20;
	case 35: return 0;
	case 200: return LT_TERM_KEY_BPASTE;
	case 201: return LT_TERM_KEY_NBPASTE;
	default: return 0;
	}
}

static
u32 convert_xterm_mod(u32 mod) {
	switch (mod) {
	case 1: return 0;
	case 2: return LT_TERM_MOD_SHIFT;
	case 3: return LT_TERM_MOD_ALT;
	case 4: return LT_TERM_MOD_SHIFT | LT_TERM_MOD_ALT;
	case 5: return LT_TERM_MOD_CTRL;
	case 6: return LT_TERM_MOD_CTRL | LT_TERM_MOD_SHIFT;
	case 7: return LT_TERM_MOD_CTRL | LT_TERM_MOD_ALT;
	case 8: return LT_TERM_MOD_CTRL | LT_TERM_MOD_SHIFT | LT_TERM_MOD_ALT;
	default: return 0;
	}
}

b8 lt_term_key_available(void) {
	struct pollfd pfd;
	pfd.fd = STDIN_FILENO;
	pfd.events = POLLIN;

	return poll(&pfd, 1, 1) != 0;
}

u32 lt_term_getkey(void) {
	if (resized) {
		resized = 0;
		lt_update_term_dimensions();
		return LT_TERM_KEY_RESIZE;
	}

	u32 c = lt_getc();
	u32 mod = 0;

	if (c == 0x1B) {
		struct pollfd pfd;
		pfd.fd = STDIN_FILENO;
		pfd.events = POLLIN;

		if (!poll(&pfd, 1, 1))
			return LT_TERM_KEY_ESC;

		c = lt_getc();
		if (c == 0x1B)
			return LT_TERM_KEY_ESC;

		if (c == 'O') { // Parse F1 - F4
			if (!poll(&pfd, 1, 1))
				return 'O' | LT_TERM_MOD_ALT;
			c = lt_getc();
			return convert_xterm_key(c);
		}

		if (c != '[' || !poll(&pfd, 1, 1)) {
			mod = LT_TERM_MOD_ALT;
			goto unescaped;
		}

		u32 param[16];
		u16 param_count = 0;
		u8 param_len = 0;

		c = lt_getc();
		// Read all parameter bytes
		while (is_param_byte(c)) {
			if (isdigit(c)) {
				if (!param_len++)
					param[param_count++] = 0;
				param[param_count - 1] *= 10;
				param[param_count - 1] += c - '0';
			}
			else if (c == ';') {
				if (!param_len)
					param[param_count++] = 1;
				param_len = 0;
			}
			c = lt_getc();
		}

		// Skip all intermediate bytes (if there are any)
		while (is_intermediate_byte(c))
			c = lt_getc();

		// Convert modifiers if possible
		if (param_count >= 2)
			mod = convert_xterm_mod(param[1]);

		// Convert special characters
		if (param_count < 1 || param[0] == 1) {
			if (c == 'M') { // Mouse event
				u8 b = lt_getc() - 32;
				u8 x = lt_getc() - 32;
				u8 y = lt_getc() - 32;
				lt_term_mouse_x = x - 1;
				lt_term_mouse_y = y - 1;

				switch (b & 3) {
				case 0: c = LT_TERM_KEY_MB1_DN; break;
				case 1: c = LT_TERM_KEY_MB2_DN; break;
				case 2: c = LT_TERM_KEY_MB3_DN; break;
				case 3: c = LT_TERM_KEY_MB1_UP; break;
				}

				if (b >= 32) {
					b -= 32;
					c = LT_TERM_KEY_MPOS;
				}

				if (b & 4) mod |= LT_TERM_MOD_SHIFT;
				if (b & 8) mod |= LT_TERM_MOD_ALT;
				if (b & 16) mod |= LT_TERM_MOD_CTRL;
				return c | mod;
			}
			return convert_xterm_key(c) | mod;
		}

		if (param_count >= 1 || c == '~')
			return convert_vt_key(param[0]) | mod;

		return 0;
	}
unescaped:

	// Sort out CTRL+alpha
	if (c < 32 && c) {
		switch (c) {
		case 0: case '\t': case '\n': case LT_TERM_KEY_ESC: return c | mod;
		case '\b': return LT_TERM_KEY_BSPACE | mod | LT_TERM_MOD_CTRL;
		case 0x1F: return '/' | mod | LT_TERM_MOD_CTRL;

		default:
			c += 64;
			mod |= LT_TERM_MOD_CTRL;
			break;
		}
	}

	if (term_flags & LT_TERM_UTF8) {
		u32 utf8_len = lt_utf8_decode_len(c);

		char recv_buf[4] = { c, 0, 0, 0 };
		char* it = recv_buf;
		switch (utf8_len) {
		case 4: *++it = lt_getc();
		case 3: *++it = lt_getc();
		case 2: *++it = lt_getc();
		case 1:
			lt_utf8_decode(recv_buf, &c);
			return c | mod;
		}
	}
	return c | mod;
}

isz lt_term_write_direct(const char* str, usz len) {
	write(STDOUT_FILENO, str, len);
	fsync(STDOUT_FILENO);
	return 0;
}

#elif defined(LT_WINDOWS)
#	include <lt/io.h>
#	include <windows.h>

static lt_term_flags_t term_flags;
static DWORD old_output_mode = 0;
static DWORD old_input_mode = 0;

u32 lt_term_width, lt_term_height;
i32 lt_term_mouse_x, lt_term_mouse_y;

lt_err_t lt_term_init(lt_term_flags_t flags) {
	lt_err_t err;

	term_flags = flags;

	DWORD mode;
	if (!GetConsoleMode(lt_stdout->hnd, &mode))
		return LT_ERR_UNKNOWN;
	old_output_mode = mode;

	mode |= ENABLE_PROCESSED_OUTPUT;
	mode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
	mode |= DISABLE_NEWLINE_AUTO_RETURN;
	if (!SetConsoleMode(lt_stdout->hnd, mode))
		return LT_ERR_UNKNOWN;

	if (!GetConsoleMode(lt_stdin->hnd, &mode))
		return LT_ERR_UNKNOWN;
	old_input_mode = mode;

	mode |= ENABLE_WINDOW_INPUT;
	mode &= ENABLE_ECHO_INPUT;
	mode &= ENABLE_PROCESSED_INPUT;
	if (!SetConsoleMode(lt_stdin->hnd, mode))
		return LT_ERR_UNKNOWN;

// 	LT_TERM_ECHO	= 0x01,
// 	LT_TERM_CANON	= 0x02,
// 	LT_TERM_UTF8	= 0x08,

	if ((flags & LT_TERM_MOUSE) && lt_term_write_direct("\x1B[?1003h", 8) < 0)
		return LT_ERR_UNKNOWN;
	if ((flags & LT_TERM_BPASTE) && lt_term_write_direct("\x1b[?2004h", 8) < 0)
		return LT_ERR_UNKNOWN;
	if ((flags & LT_TERM_ALTBUF) && lt_term_write_direct("\x1b[?1049h", 8) < 0)
		return LT_ERR_UNKNOWN;

	if ((err = lt_update_term_dimensions()))
		return err;

	return LT_SUCCESS;
}

void lt_term_restore(void) {
	if (term_flags & LT_TERM_MOUSE)
		lt_term_write_direct("\x1B[?1003l", 8);
	if (term_flags & LT_TERM_BPASTE)
		lt_term_write_direct("\x1b[?2004l", 8);
	if (term_flags & LT_TERM_ALTBUF)
		lt_term_write_direct("\x1b[?1049l", 8);

	SetConsoleMode(lt_stdout->hnd, old_output_mode);
	SetConsoleMode(lt_stdin->hnd, old_input_mode);
}

lt_err_t lt_update_term_dimensions(void) {
	CONSOLE_SCREEN_BUFFER_INFO csbi;
    if (!GetConsoleScreenBufferInfo(lt_stdout->hnd, &csbi))
    	return LT_ERR_UNKNOWN; // !!
    lt_term_width = csbi.srWindow.Right - csbi.srWindow.Left + 1;
    lt_term_height = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;

	return LT_SUCCESS;
}

b8 lt_term_key_available(void) {
	return 1;
}

u32 lt_term_getkey(void) {
	INPUT_RECORD rec;
	DWORD read = 0;
	if (!ReadConsoleInput(lt_stdin->hnd, &rec, 1, &read) || !read)
		return 0;

	switch (rec.EventType) {
	case KEY_EVENT: {
		if (!rec.Event.KeyEvent.bKeyDown)
			return 0;

		u32 mod = 0;
		u32 event_mod = rec.Event.KeyEvent.dwControlKeyState;
		if ((event_mod & LEFT_CTRL_PRESSED) || (event_mod & RIGHT_CTRL_PRESSED))
			mod |= LT_TERM_MOD_CTRL;
		if ((event_mod & LEFT_ALT_PRESSED) || (event_mod & RIGHT_ALT_PRESSED))
			mod |= LT_TERM_MOD_ALT;
		if (event_mod & SHIFT_PRESSED)
			mod |= LT_TERM_MOD_SHIFT;

		u32 c = rec.Event.KeyEvent.uChar.AsciiChar;
		switch (rec.Event.KeyEvent.wVirtualKeyCode) {
		case VK_BACK:	return LT_TERM_KEY_BSPACE | mod;
		case VK_TAB:	return '\t' | mod;
		case VK_RETURN:	return '\n' | mod;
		case VK_ESCAPE:	return LT_TERM_KEY_ESC | mod;
		case VK_PRIOR:	return LT_TERM_KEY_PAGEUP | mod;
		case VK_NEXT:	return LT_TERM_KEY_PAGEDN | mod;
		case VK_END:	return LT_TERM_KEY_END | mod;
		case VK_HOME:	return LT_TERM_KEY_HOME | mod;
		case VK_LEFT:	return LT_TERM_KEY_LEFT | mod;
		case VK_UP:		return LT_TERM_KEY_UP | mod;
		case VK_RIGHT:	return LT_TERM_KEY_RIGHT | mod;
		case VK_DOWN:	return LT_TERM_KEY_DOWN | mod;
		case VK_INSERT:	return LT_TERM_KEY_INSERT | mod;
		case VK_DELETE:	return LT_TERM_KEY_DELETE | mod;
		case VK_F1:		return LT_TERM_KEY_F1 | mod;
		case VK_F2:		return LT_TERM_KEY_F2 | mod;
		case VK_F3:		return LT_TERM_KEY_F3 | mod;
		case VK_F4:		return LT_TERM_KEY_F4 | mod;
		case VK_F5:		return LT_TERM_KEY_F5 | mod;
		case VK_F6:		return LT_TERM_KEY_F6 | mod;
		case VK_F7:		return LT_TERM_KEY_F7 | mod;
		case VK_F8:		return LT_TERM_KEY_F8 | mod;
		case VK_F9:		return LT_TERM_KEY_F9 | mod;
		case VK_F10:	return LT_TERM_KEY_F10 | mod;
		case VK_F11:	return LT_TERM_KEY_F11 | mod;
		case VK_F12:	return LT_TERM_KEY_F12 | mod;
		case VK_F13:	return LT_TERM_KEY_F13 | mod;
		case VK_F14:	return LT_TERM_KEY_F14 | mod;
		case VK_F15:	return LT_TERM_KEY_F15 | mod;
		case VK_F16:	return LT_TERM_KEY_F16 | mod;
		case VK_F17:	return LT_TERM_KEY_F17 | mod;
		case VK_F18:	return LT_TERM_KEY_F18 | mod;
		case VK_F19:	return LT_TERM_KEY_F19 | mod;
		case VK_F20:	return LT_TERM_KEY_F20 | mod;
		}

		if (c < 32 && c) {
			switch (c) {
			case 0x1F:
				return '/' | mod | LT_TERM_MOD_CTRL;
			default: c += 64;
			}
		}
		return c | mod;
	}

	case WINDOW_BUFFER_SIZE_EVENT:
		lt_update_term_dimensions();
		return LT_TERM_KEY_RESIZE;

	case MENU_EVENT:
	case MOUSE_EVENT:
	case FOCUS_EVENT:
	default:
		return 0;
	}
}

isz lt_term_write_direct(const char* str, usz len) {
	WriteConsole(lt_stdout->hnd, str, len, NULL, NULL);
	return 0;
}

#endif

