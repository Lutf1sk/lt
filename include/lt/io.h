#ifndef LT_IO_H
#define LT_IO_H 1

#include <lt/lt.h>
#include <lt/fwd.h>

#include <stdarg.h>

typedef struct lt_file lt_file_t;

// std.c
extern lt_file_t* lt_stdout;
extern lt_file_t* lt_stderr;
extern lt_file_t* lt_stdin;

// fmt.c
isz lt_io_dummy_callb(void* usr, void* data, usz len);

isz lt_io_vprintf(lt_io_callback_t callb, void* usr, char* fmt, va_list argl);
isz lt_io_printf(lt_io_callback_t callb, void* usr, char* fmt, ...);

// file.c
typedef
enum lt_file_mode {
	LT_FILE_R,
	LT_FILE_W,
	LT_FILE_RW,
} lt_file_mode_t;

typedef
enum lt_file_perms {
	LT_FILE_PERMIT_X,
} lt_file_perms_t;

lt_file_t* lt_file_open(char* path, lt_file_mode_t access, lt_file_perms_t perms, lt_alloc_t* alloc);
void lt_file_close(lt_file_t* file, lt_alloc_t* alloc);

b8 lt_file_read_entire(char* path, lstr_t* out, lt_alloc_t* alloc);

isz lt_file_read(lt_file_t* file, void* data, usz size);
isz lt_file_write(lt_file_t* file, void* data, usz size);

usz lt_file_size(lt_file_t* file);

isz lt_vfprintf(lt_file_t* file, char* fmt, va_list args);
isz lt_fprintf(lt_file_t* file, char* fmt, ...);

// cli.c
isz lt_printf(char* fmt, ...);

// str.c
isz lt_str_io_callb(char** str, void* data, usz len);

isz lt_vsprintf(char* str, char* fmt, va_list args);
isz lt_sprintf(char* str, char* fmt, ...);

// alloc.c
typedef struct lt_io_alloc_ctx lt_io_alloc_ctx_t;
isz lt_alloc_io_callb(lt_io_alloc_ctx_t* cx, void* data, usz len);

isz lt_vaprintf(lstr_t* out, lt_alloc_t* alc, char* fmt, va_list args);
isz lt_aprintf(lstr_t* out, lt_alloc_t* alc, char* fmt, ...);

#endif
