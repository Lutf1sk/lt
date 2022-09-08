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

isz lt_file_printuq_hex(lt_file_t* file, usz n);
isz lt_file_printuq(lt_file_t* file, u64 n);
isz lt_file_printiq(lt_file_t* file, i64 n);
isz lt_file_printfq(lt_file_t* file, f64 n);

isz lt_file_printc(lt_file_t* file, char c);
isz lt_file_printls(lt_file_t* file, lstr_t str);
isz lt_file_printf(lt_file_t* file, char* fmt, ...);

// fmt.c
isz lt_file_vprintf(lt_file_t* file, char* fmt, va_list args);
isz lt_str_vprintf(char* str, char* fmt, va_list args);

// cli.c
isz lt_printc(char c);
isz lt_printls(lstr_t str);
isz lt_printf(char* fmt, ...);

// str.c
isz lt_str_printuq_hex(char* str, usz n);
isz lt_str_printuq(char* str, u64 n);
isz lt_str_printiq(char* str, i64 n);
isz lt_str_printfq(char* str, f64 n);

isz lt_str_printf(char* str, char* fmt, ...);

#endif
