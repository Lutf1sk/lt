#ifndef LT_IO_H
#define LT_IO_H 1

#include <lt/err.h>
#include <lt/fwd.h>

#include <stdarg.h>

typedef struct lt_file lt_file_t;
typedef struct lt_dir lt_dir_t;

typedef
enum lt_dirent_type {
	LT_DIRENT_FILE,
	LT_DIRENT_DIR,
	LT_DIRENT_SYMLINK,
	LT_DIRENT_DEVICE,
	LT_DIRENT_PIPE,
	LT_DIRENT_SOCKET,
	LT_DIRENT_UNKNOWN,
} lt_dirent_type_t;

typedef
struct lt_dirent {
	lt_dirent_type_t type;
	lstr_t name;
} lt_dirent_t;

// std.c
extern lt_file_t* lt_stdout;
extern lt_file_t* lt_stderr;
extern lt_file_t* lt_stdin;

// fmt.c
isz lt_io_dummy_callb(void* usr, void* data, usz len);

isz lt_io_vprintf(lt_io_callback_t callb, void* usr, char* fmt, va_list argl);
isz lt_io_printf(lt_io_callback_t callb, void* usr, char* fmt, ...);

// file.c
#define LT_PATH_MAX 512

typedef
enum lt_file_mode {
	LT_FILE_R = 1,
	LT_FILE_W = 2,
	LT_FILE_RW = 3,
} lt_file_mode_t;

typedef
enum lt_file_perms {
	LT_FILE_PERMIT_X = 0x01,
	LT_FILE_PERMIT_S = 0x01,
	LT_FILE_PERMIT_R = 0x02,
	LT_FILE_PERMIT_W = 0x04,
} lt_file_perms_t;

lt_file_t* lt_fopenp(lstr_t path, lt_file_mode_t access, lt_file_perms_t perms, lt_alloc_t* alloc);
void lt_fclose(lt_file_t* file, lt_alloc_t* alloc);

b8 lt_fexistp(lstr_t path);

lt_err_t lt_fremovep(lstr_t path);
lt_err_t lt_fmovep(lstr_t path, lstr_t new_path);

lt_err_t lt_fcopyp(lstr_t from, lstr_t to, void* buf, usz bufsz, lt_alloc_t* alloc);

lt_err_t lt_flinkp(lstr_t link, lstr_t target);
lt_err_t lt_fsymlinkp(lstr_t link, lstr_t target);

lt_err_t lt_freadallp(lstr_t path, lstr_t* out, lt_alloc_t* alloc);
lt_err_t lt_freadallp_utf8(lstr_t path, lstr_t* out, lt_alloc_t* alloc);

isz lt_fread(lt_file_t* file, void* data, usz size);
isz lt_fwrite(lt_file_t* file, void* data, usz size);

usz lt_fsize(lt_file_t* file);

isz lt_vfprintf(lt_file_t* file, char* fmt, va_list args);
isz lt_fprintf(lt_file_t* file, char* fmt, ...);

// dir.c
lt_dir_t* lt_dopenp(lstr_t path, lt_alloc_t* alloc);
void lt_dclose(lt_dir_t* dir, lt_alloc_t* alloc);

lt_dirent_t* lt_dread(lt_dir_t* dir);

lt_err_t lt_dcopyp(lstr_t from, lstr_t to, void* buf, usz bufsz, lt_alloc_t* alloc);

lt_err_t lt_mkdir(lstr_t path);
lt_err_t lt_mkpath(lstr_t path);

#define lt_foreach_dirent(it_name, dir) \
		for (lt_dirent_t* it_name; (it_name = lt_dread(dir)); )

// stat.c
typedef
struct lt_stat {
	usz size;
	lt_dirent_type_t type;
	lt_file_perms_t permit;
} lt_stat_t;

lt_err_t lt_statp(lstr_t path, lt_stat_t* out_stat);
lt_err_t lt_lstatp(lstr_t path, lt_stat_t* out_stat);

// cli.c
isz lt_printf(char* fmt, ...);

// str.c
isz lt_str_io_callb(char** str, void* data, usz len);

isz lt_vsprintf(char* str, char* fmt, va_list args);
isz lt_sprintf(char* str, char* fmt, ...);

// alloc.c
typedef struct lt_io_alloc_ctx lt_io_alloc_ctx_t;
isz lt_alloc_io_callb(lt_io_alloc_ctx_t* cx, void* data, usz len);

isz lt_vaprintf(lstr_t* out, lt_alloc_t* alloc, char* fmt, va_list args);
isz lt_aprintf(lstr_t* out, lt_alloc_t* alloc, char* fmt, ...);

#endif
