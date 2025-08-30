#pragma once

#include <lt2/common.h>

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

