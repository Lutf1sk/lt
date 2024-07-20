#ifndef LT_IO_H
#define LT_IO_H 1

#include <lt/err.h>
#include <lt/fwd.h>

#include <stdarg.h>

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
isz lt_io_dummy_write(void* usr, const void* data, usz len);
isz lt_io_dummy_read(void* usr, void* data, usz len);

isz lt_io_vprintf(lt_write_fn_t callb, void* usr, const char* fmt, va_list argl);
isz lt_io_printf(lt_write_fn_t callb, void* usr, const char* fmt, ...);

// file.c
#if defined(LT_UNIX)
typedef
struct lt_file {
	int fd;
	u64 size;
	usz block_size;
} lt_file_t;

#elifdef LT_WINDOWS
typedef
struct lt_file {
	void* hnd;
	u64 size;
} lt_file_t;
#endif

#define LT_PATH_MAX 512
#define LT_FILENAME_MAX 256

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

lt_file_t* lt_fopenp(lstr_t path, lt_file_mode_t access, lt_file_perms_t perms, lt_alloc_t alloc[static 1]);
void lt_fclose(const lt_file_t file[static 1], lt_alloc_t alloc[static 1]);

b8 lt_fexistp(lstr_t path);

lt_err_t lt_fremovep(lstr_t path);
lt_err_t lt_fmovep(lstr_t path, lstr_t new_path);

lt_err_t lt_fcopyp(lstr_t from, lstr_t to, void* buf, usz bufsz, lt_alloc_t alloc[static 1]);

lt_err_t lt_flinkp(lstr_t link, lstr_t target);
lt_err_t lt_fsymlinkp(lstr_t link, lstr_t target);

lt_err_t lt_freadallp(lstr_t path, lstr_t out[static 1], lt_alloc_t alloc[static 1]);
lt_err_t lt_freadallp_utf8(lstr_t path, lstr_t out[static 1], lt_alloc_t alloc[static 1]);

isz lt_fread(lt_file_t file[static 1], void* data, usz size);
isz lt_fwrite(lt_file_t file[static 1], const void* data, usz size);

usz lt_fsize(lt_file_t file[static 1]);

isz lt_vfprintf(lt_file_t file[static 1], const char* fmt, va_list args);
isz lt_fprintf(lt_file_t file[static 1], const char* fmt, ...);

// dir.c

#ifdef LT_LINUX
typedef
struct lt_dir {
	void* dp;
	lt_dirent_t ent;
} lt_dir_t;

#elifdef LT_WINDOWS
#	define WIN32_LEAN_AND_MEAN 1
#	include <windows.h>
typedef
struct lt_dir {
	b8 eof;
	WIN32_FIND_DATA ffd;
	void* fhnd;
	lt_dirent_t ent;
} lt_dir_t;
#endif

lt_dir_t* lt_dopenp(lstr_t path, lt_alloc_t alloc[static 1]);
void lt_dclose(const lt_dir_t dir[static 1], lt_alloc_t alloc[static 1]);

lt_dirent_t* lt_dread(lt_dir_t dir[static 1]);

#define LT_DCOPY_MERGE 1
#define LT_DCOPY_OVERWRITE_FILES 2

lt_err_t lt_dcopyp(lstr_t from, lstr_t to, void* buf, usz bufsz, u32 flags, lt_alloc_t alloc[static 1]);

lt_err_t lt_dremovep(lstr_t path, lt_alloc_t alloc[static 1]);

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

#ifdef LT_UNIX
lt_dirent_type_t lt_enttype_from_unix(int mode);
#endif

lt_err_t lt_statp(lstr_t path, lt_stat_t* out_stat);
lt_err_t lt_lstatp(lstr_t path, lt_stat_t* out_stat);

// cli.c
isz lt_printf(const char* fmt, ...);

// str.c
isz lt_write_str(char* str[static 1], const void* data, usz len);

isz lt_vsprintf(char* str, const char* fmt, va_list args);
isz lt_sprintf(char* str, const char* fmt, ...);

// alloc.c
typedef struct lt_io_alloc_ctx lt_io_alloc_ctx_t;
isz lt_write_alloc(lt_io_alloc_ctx_t* cx, const void* data, usz len);

isz lt_vaprintf(lstr_t out[static 1], lt_alloc_t alloc[static 1], const char* fmt, va_list args);
isz lt_aprintf(lstr_t out[static 1], lt_alloc_t alloc[static 1], const char* fmt, ...);

#endif
