#pragma once

#include <lt2/common.h>

// ansi escapes

#define CSI "\x1b["

#define RESET   CSI"0m"
#define BOLD    CSI"1m"
#define FAINT   CSI"2m"
#define ITALIC  CSI"3m"
#define UNDERLN CSI"4m"
#define REGULAR CSI"22m"

#define FG_BLACK    CSI"30m"
#define FG_RED      CSI"31m"
#define FG_GREEN    CSI"32m"
#define FG_YELLOW   CSI"33m"
#define FG_BLUE     CSI"34m"
#define FG_MAGENTA  CSI"35m"
#define FG_CYAN     CSI"36m"
#define FG_WHITE    CSI"37m"
#define BG_BLACK    CSI"40m"
#define BG_RED      CSI"41m"
#define BG_GREEN    CSI"42m"
#define BG_YELLOW   CSI"43m"
#define BG_BLUE     CSI"44m"
#define BG_MAGENTA  CSI"45m"
#define BG_CYAN     CSI"46m"
#define BG_WHITE    CSI"47m"
#define FG_BBLACK   CSI"90m"
#define FG_BRED     CSI"91m"
#define FG_BGREEN   CSI"92m"
#define FG_BYELLOW  CSI"93m"
#define FG_BBLUE    CSI"94m"
#define FG_BMAGENTA CSI"95m"
#define FG_BCYAN    CSI"96m"
#define FG_BWHITE   CSI"97m"
#define BG_BBLACK   CSI"100m"
#define BG_BRED     CSI"101m"
#define BG_BGREEN   CSI"102m"
#define BG_BYELLOW  CSI"103m"
#define BG_BBLUE    CSI"104m"
#define BG_BMAGENTA CSI"105m"
#define BG_BCYAN    CSI"106m"
#define BG_BWHITE   CSI"107m"

#define CLS_TO_END     CSI"0J"
#define CLS_TO_BEG     CSI"1J"
#define CLS            CSI"2J"
#define CLS_SCROLLBACK CSI"3J"

#define CLL_TO_END CSI"0K"
#define CLL_TO_BEG CSI"1K"
#define CLL        CSI"2K"

#define CSAVE      CSI"s"
#define CRESTORE   CSI"u"
#define CRESET     CSI"H"
#define CSET(y, x) CSI#y";"#x"H"
#define CUP1       CSI"A"
#define CUP(n)     CSI#n"A"
#define CDOWN1     CSI"B"
#define CDOWN(n)   CSI#n"B"
#define CRIGHT1    CSI"C"
#define CRIGHT(n)  CSI#n"C"
#define CLEFT1     CSI"D"
#define CLEFT(n)   CSI#n"D"
#define CNEXTLN1   CSI"E"
#define CNEXTLN(n) CSI#n"E"
#define CPREVLN1   CSI"F"
#define CPREVLN(n) CSI#n"F"
#define CRESETY    CSI"G"
#define CSETY(n)   CSI#n"G"

// extensions
#define EXT_CURSOR_ON         CSI"?25h"
#define EXT_CURSOR_OFF        CSI"?25l"
#define EXT_ALTBUF_ON         CSI"?1049h"
#define EXT_ALTBUF_OFF        CSI"?1049l"
#define EXT_REPORT_FOCUS_ON   CSI"?1004h"
#define EXT_REPORT_FOCUS_OFF  CSI"?1004l"
#define EXT_BRACKET_PASTE_ON  CSI"?2004h"
#define EXT_BRACKET_PASTE_OFF CSI"?2004l"

#define ARG_NONE 0
#define ARG_STR  1
#define ARG_BOOL 2
#define ARG_INT  3

typedef struct cli_param {
	u8 arg_type;
	u8 short_key;
	ls long_key;
	ls description;
} cli_param;

typedef struct cli_options {
	void (*callback)(struct cli_options* cli, isz key, ls val);
	cli_param* params;
	usz param_count;
	void* userdata;
} cli_options;

b8 parse_cli_args(int argc, char** argv, cli_options cli[static 1], err* err);
void print_cli_help(cli_options cli[static 1]);

typedef struct cli_process {
	file_handle out;
	file_handle in;
	file_handle err;
	u64 pid;
} cli_process_t;

cli_process_t cli_run(ls cmd, err* error);
void cli_close(cli_process_t* p, err* error);

