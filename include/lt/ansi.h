#ifndef LT_ANSI_H
#define LT_ANSI_H 1

#define LT_CSI "\x1b["

// Misc. text styles
#define LT_RESET		LT_CSI"0m"
#define LT_BOLD			LT_CSI"1m"
#define LT_FAINT		LT_CSI"2m"
#define LT_ITALIC		LT_CSI"3m"
#define LT_UNDERLINE	LT_CSI"4m"
#define LT_NORMAL		LT_CSI"22m"

// Foreground colors
#define LT_FG_BLACK		LT_CSI"30m"
#define LT_FG_RED		LT_CSI"31m"
#define LT_FG_GREEN		LT_CSI"32m"
#define LT_FG_YELLOW	LT_CSI"33m"
#define LT_FG_BLUE		LT_CSI"34m"
#define LT_FG_MAGENTA	LT_CSI"35m"
#define LT_FG_CYAN		LT_CSI"36m"
#define LT_FG_WHITE		LT_CSI"37m"

// Background colors
#define LT_BG_BLACK		LT_CSI"40m"
#define LT_BG_RED		LT_CSI"41m"
#define LT_BG_GREEN		LT_CSI"42m"
#define LT_BG_YELLOW	LT_CSI"43m"
#define LT_BG_BLUE		LT_CSI"44m"
#define LT_BG_MAGENTA	LT_CSI"45m"
#define LT_BG_CYAN		LT_CSI"46m"
#define LT_BG_WHITE		LT_CSI"47m"

// Bright foreground colors
#define LT_FG_BBLACK	LT_CSI"90m"
#define LT_FG_BRED		LT_CSI"91m"
#define LT_FG_BGREEN	LT_CSI"92m"
#define LT_FG_BYELLOW	LT_CSI"93m"
#define LT_FG_BBLUE		LT_CSI"94m"
#define LT_FG_BMAGENTA	LT_CSI"95m"
#define LT_FG_BCYAN		LT_CSI"96m"
#define LT_FG_BWHITE	LT_CSI"97m"

// Bright foreground colors
#define LT_BG_BBLACK	LT_CSI"100m"
#define LT_BG_BRED		LT_CSI"101m"
#define LT_BG_BGREEN	LT_CSI"102m"
#define LT_BG_BYELLOW	LT_CSI"103m"
#define LT_BG_BBLUE		LT_CSI"104m"
#define LT_BG_BMAGENTA	LT_CSI"105m"
#define LT_BG_BCYAN		LT_CSI"106m"
#define LT_BG_BWHITE	LT_CSI"107m"

// Clear screen
#define LT_CLS_TO_END				LT_CSI"0J"
#define LT_CLS_TO_BEG				LT_CSI"1J"
#define LT_CLS						LT_CSI"2J"
#define LT_CLS_DELETE_SCROLLBACK	LT_CSI"3J"

// Clear line
#define LT_CLL_TO_END	LT_CSI"0K"
#define LT_CLL_TO_BEG	LT_CSI"1K"
#define LT_CLL			LT_CSI"2K"

// Cursor movement
#define LT_CSAVE		LT_CSI"s"
#define LT_CRESTORE		LT_CSI"u"
#define LT_CRESET		LT_CSI"H"
#define LT_CSET(y, x)	LT_CSI#y";"#x"H"
#define LT_CUP1			LT_CSI"A"
#define LT_CUP(y)		LT_CSI#y"A"
#define LT_CDOWN1		LT_CSI"B"
#define LT_CDOWN(y)		LT_CSI#y"B"
#define LT_CRIGHT1		LT_CSI"C"
#define LT_CRIGHT(x)	LT_CSI#x"C"
#define LT_CLEFT1		LT_CSI"D"
#define LT_CLEFT(x) 	LT_CSI#x"D"
#define LT_CNEXTL1		LT_CSI"E"
#define LT_CNEXTL(y)	LT_CSI#y"E"
#define LT_CPREVL1		LT_CSI"F"
#define LT_CPREVL(y)	LT_CSI#y"F"
#define LT_CRESETY		LT_CSI"G"
#define LT_CSETY(y)		LT_CSI#y"G"

// Extensions
#define LT_CURSOR_ENABLE			LT_CSI"?25h"
#define LT_CURSOR_DISABLE			LT_CSI"?25l"
#define LT_ALTBUF_ENABLE			LT_CSI"?1049h"
#define LT_ALTBUF_DISABLE			LT_CSI"?1049l"
#define LT_REPORT_FOCUS_ENABLE		LT_CSI"?1004h"
#define LT_REPORT_FOCUS_DISABLE		LT_CSI"?1004l"
#define LT_BRACKET_PASTE_ENABLE		LT_CSI"?2004h"
#define LT_BRACKET_PASTE_DISABLE	LT_CSI"?2004l"

#ifdef LT_ANSI_SHORTEN_NAMES
#	define CSI LT_CSI

	// Misc. text styles
#	define RESET		LT_RESET
#	define BOLD			LT_BOLD
#	define FAINT		LT_FAINT
#	define ITALIC		LT_ITALIC
#	define UNDERLINE	LT_UNDERLINE
#	define NORMAL		LT_NORMAL

	// Foreground colors
#	define FG_BLACK		LT_FG_BLACK
#	define FG_RED		LT_FG_RED
#	define FG_GREEN		LT_FG_GREEN
#	define FG_YELLOW	LT_FG_YELLOW
#	define FG_BLUE		LT_FG_BLUE
#	define FG_MAGENTA	LT_FG_MAGENTA
#	define FG_CYAN		LT_FG_CYAN
#	define FG_WHITE		LT_FG_WHITE

	// Background colors
#	define BG_BLACK		LT_BG_BLACK
#	define BG_RED		LT_BG_RED
#	define BG_GREEN		LT_BG_GREEN
#	define BG_YELLOW	LT_BG_YELLOW
#	define BG_BLUE		LT_BG_BLUE
#	define BG_MAGENTA	LT_BG_MAGENTA
#	define BG_CYAN		LT_BG_CYAN
#	define BG_WHITE		LT_BG_WHITE

	// Bright foreground colors
#	define FG_BBLACK	LT_FG_BBLACK
#	define FG_BRED		LT_FG_BRED
#	define FG_BGREEN	LT_FG_BGREEN
#	define FG_BYELLOW	LT_FG_BYELLOW
#	define FG_BBLUE		LT_FG_BBLUE
#	define FG_BMAGENTA	LT_FG_BMAGENTA
#	define FG_BCYAN		LT_FG_BCYAN
#	define FG_BWHITE	LT_FG_BWHITE

	// Bright foreground colors
#	define BG_BBLACK	LT_BG_BBLACK
#	define BG_BRED		LT_BG_BRED
#	define BG_BGREEN	LT_BG_BGREEN
#	define BG_BYELLOW	LT_BG_BYELLOW
#	define BG_BBLUE		LT_BG_BBLUE
#	define BG_BMAGENTA	LT_BG_BMAGENTA
#	define BG_BCYAN		LT_BG_BCYAN
#	define BG_BWHITE	LT_BG_BWHITE

	// Clear screen
#	define CLS_TO_END				LT_CLS_TO_END
#	define CLS_TO_BEG				LT_CLS_TO_BEG
#	define CLS						LT_CLS
#	define CLS_DELETE_SCROLLBACK	LT_CLS_DELETE_SCROLLBACK

	// Clear line
#	define CLL_TO_END	LT_CLL_TO_END
#	define CLL_TO_BEG	LT_CLL_TO_BEG
#	define CLL			LT_CLL

	// Cursor movement
#	define CSAVE		LT_CSAVE
#	define CRESTORE		LT_CRESTORE
#	define CRESET		LT_CRESET
#	define CSET(y, x)	LT_CSET(y, x)
#	define CUP1			LT_CUP1
#	define CUP(y)		LT_CUP(y)
#	define CDOWN1		LT_CDOWN1
#	define CDOWN(y)		LT_CDOWN(y)
#	define CRIGHT1		LT_CRIGHT1
#	define CRIGHT(x)	LT_CRIGHT(x)
#	define CLEFT1		LT_CLEFT1
#	define CLEFT(x) 	LT_CLEFT(x)
#	define CNEXTL1		LT_CNEXTL1
#	define CNEXTL(y)	LT_CNEXTL(y)
#	define CPREVL1		LT_CPREVL1
#	define CPREVL(y)	LT_CPREVL(y)
#	define CRESETY		LT_CRESETY
#	define CSETY(y)		LT_CSETY(y)

	// Extensions
#	define CURSOR_ENABLE			LT_CURSOR_ENABLE
#	define CURSOR_DISABLE			LT_CURSOR_DISABLE
#	define ALTBUF_ENABLE			LT_ALTBUF_ENABLE
#	define ALTBUF_DISABLE			LT_ALTBUF_DISABLE
#	define REPORT_FOCUS_ENABLE		LT_REPORT_FOCUS_ENABLE
#	define REPORT_FOCUS_DISABLE		LT_REPORT_FOCUS_DISABLE
#	define BRACKET_PASTE_ENABLE		LT_BRACKET_PASTE_ENABLE
#	define BRACKET_PASTE_DISABLE	LT_BRACKET_PASTE_DISABLE
#endif

#endif
