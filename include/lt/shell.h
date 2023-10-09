#ifndef LT_SHELL_H
#define LT_SHELL_H 1

#include <lt/err.h>

lt_err_t lt_shell_exec(lstr_t command, int* out_code);

#endif