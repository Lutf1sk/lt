#include <lt/lt.h>
#include <lt/term.h>
#include <lt/utf8.h>

#if defined(LT_UNIX)
#	include <termios.h>
#	include <unistd.h>
#	include <poll.h>
#	include <signal.h>
#	include <sys/ioctl.h>
#	include <ctype.h>

static lt_term_flags_t term_flags;
static struct termios old_term;

u32 lt_term_width = 0, lt_term_height = 0;
i32 lt_term_mouse_x = 0, lt_term_mouse_y = 0;

static volatile u8 resized = 0;

static
lt_err_t lt_update_term_dimensions(void) {
	struct winsize wsz;
	if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &wsz) < 0)
		return LT_ERR_UNKNOWN; // !!

	lt_term_width = wsz.ws_col;
	lt_term_height = wsz.ws_row;
	return LT_SUCCESS;
}

static
void lt_handle_winch(int sig) {
	resized = 1;
}

lt_err_t lt_term_init(lt_term_flags_t flags) {
	term_flags = flags;

	struct termios term;
	if (tcgetattr(STDOUT_FILENO, &term) < 0)
		goto err0;
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
		goto err1;
	if (tcflush(STDOUT_FILENO, TCIOFLUSH) < 0)
		goto err1;

	if ((flags & LT_TERM_MOUSE) && write(STDOUT_FILENO, "\x1B[?1003h", 8) < 0)
		goto err1;
	if ((flags & LT_TERM_BPASTE) && write(STDOUT_FILENO, "\x1b[?2004h", 8) < 0)
		goto err1;
	if ((flags & LT_TERM_ALTBUF) && write(STDOUT_FILENO, "\x1b[?1049h", 8) < 0)
		goto err1;

	struct sigaction sact = {0};
	sact.sa_handler = lt_handle_winch;
	if (sigaction(SIGWINCH, &sact, NULL) < 0)
		goto err1;

	return lt_update_term_dimensions();

err1:	lt_term_restore();
err0:	return LT_ERR_UNKNOWN; // !!
}

void lt_term_restore(void) {
	tcsetattr(STDOUT_FILENO, TCSANOW, &old_term);
	tcflush(STDOUT_FILENO, TCIOFLUSH);

	if (term_flags & LT_TERM_MOUSE)
		write(STDOUT_FILENO, "\x1B[?1003l", 8);
	if (term_flags & LT_TERM_BPASTE)
		write(STDOUT_FILENO, "\x1b[?2004l", 8);
	if (term_flags & LT_TERM_ALTBUF)
		write(STDOUT_FILENO, "\x1b[?1049l", 8);
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
			lt_utf8_decode(&c, recv_buf);
			return c | mod;
		}
	}
	return c | mod;
}

void lt_term_write_direct(char* str, usz len) {
	write(STDOUT_FILENO, str, len);
	fsync(STDOUT_FILENO);
}

#endif

