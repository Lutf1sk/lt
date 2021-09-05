#include <lt/io.h>

#include "file_def.h"

#if defined(LT_UNIX)

static lt_file_t lt_stdin_ = { 0, 0, 512, 0, NULL };
static lt_file_t lt_stdout_ = { 1, 0, 512, 0, NULL };
static lt_file_t lt_stderr_ = { 2, 0, 512, 0, NULL };

lt_file_t* lt_stdin = &lt_stdin_;
lt_file_t* lt_stdout = &lt_stdout_;
lt_file_t* lt_stderr = &lt_stderr_;

#elif defined(LT_WINDOWS)

#include <stdio.h>

static lt_file_t lt_stdin_ = { NULL, 0 };
static lt_file_t lt_stdout_ = { NULL, 0 };
static lt_file_t lt_stderr_ = { NULL, 0 };

static
void __attribute__((constructor)) lt_win32_init_std_handles(void) {
	lt_stdin_.hnd = GetStdHandle(STD_INPUT_HANDLE);
	lt_stdout_.hnd = GetStdHandle(STD_OUTPUT_HANDLE);
	lt_stderr_.hnd = GetStdHandle(STD_ERROR_HANDLE);
}

lt_file_t* lt_stdin = &lt_stdin_;
lt_file_t* lt_stdout = &lt_stdout_;
lt_file_t* lt_stderr = &lt_stderr_;

#endif
