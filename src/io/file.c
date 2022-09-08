#include <lt/io.h>
#include <lt/lt.h>
#include <lt/mem.h>

#include "file_def.h"

#if defined(LT_UNIX)
#	include <fcntl.h>
#	include <sys/stat.h>
#	include <unistd.h>

static
int lt_mode_to_posix(lt_file_mode_t access) {
	switch (access) {
	case LT_FILE_R: return O_RDONLY;
	case LT_FILE_W: return O_WRONLY | O_TRUNC | O_CREAT;
	case LT_FILE_RW: return O_RDWR;
	default:
		return 0;
	}
}

static
int lt_perms_to_posix(lt_file_perms_t perms) {
	switch (perms) {
	case LT_FILE_PERMIT_X: return S_IXUSR;
	default:
		return 0;
	}
}

#elif defined(LT_WINDOWS)
#	define WIN32_LEAN_AND_MEAN 1
#	include <windows.h>

#endif

lt_file_t* lt_file_open(char* path, lt_file_mode_t mode, lt_file_perms_t perms, lt_alloc_t* alloc) {
#if defined(LT_UNIX)
	mode_t posix_mode = lt_perms_to_posix(perms);
	if (mode == LT_FILE_W)
		posix_mode |= S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH;

	int fd = open(path, lt_mode_to_posix(mode), posix_mode);
	if (fd == -1)
		return NULL;

	struct stat stat_res;
	if (fstat(fd, &stat_res) == -1) {
		close(fd);
		return NULL;
	}

	if (S_ISDIR(stat_res.st_mode)) {
		close(fd);
		return NULL;
	}

	lt_file_t* file = lt_malloc(alloc, sizeof(lt_file_t));
	file->fd = fd;
	file->size = stat_res.st_size;
	file->block_size = stat_res.st_blksize;
	return file;
#elif defined(LT_WINDOWS)
	u64 size = 0;
	HANDLE h = INVALID_HANDLE_VALUE;
	if (mode == LT_FILE_W)
		h = CreateFile(path, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	else if (mode == LT_FILE_R) {
		h = CreateFile(path, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
		LARGE_INTEGER li = {0};
		GetFileSizeEx(h, &li);
		size = li.QuadPart;
	}
	else if (mode == LT_FILE_RW)
		lt_werr(CLSTR("LT_FILE_RW has not been implemented for windows\n")); // !!

	if (h == INVALID_HANDLE_VALUE)
		return NULL;

	lt_file_t* file = lt_malloc(alloc, sizeof(lt_file_t));
	file->hnd = h;
	file->size = size;
	return file;
#endif
}

b8 lt_file_read_entire(char* path, lstr_t* out, lt_alloc_t* alloc) {
	lt_file_t* file = lt_file_open(path, LT_FILE_R, 0, alloc);
	if (!file)
		return 0;
	usz size = lt_file_size(file);
	char* data = lt_malloc(alloc, size);
	isz res = lt_file_read(file, data, size);
	lt_file_close(file, alloc);
	if (res == -1) {
		lt_mfree(alloc, data);
		return 0;
	}
	*out = LSTR(data, res);
	return 1;
}

void lt_file_close(lt_file_t* file, lt_alloc_t* alloc) {
#if defined(LT_UNIX)
	close(file->fd);
#elif defined(LT_WINDOWS)
	CloseHandle(file->hnd);
#endif
	lt_mfree(alloc, file);
}

isz lt_file_read(lt_file_t* file, void* data, usz size) {
#if defined(LT_UNIX)

	isz read_bytes = read(file->fd, data, size);
	return read_bytes;

#elif defined(LT_WINDOWS)

	DWORD read_bytes = 0;
	BOOL err = ReadFile(file->hnd, data, size, &read_bytes, NULL);
	if (err == FALSE)
		return -1;
	return read_bytes;

#endif
}

isz lt_file_write(lt_file_t* file, void* data, usz size) {
#if defined(LT_UNIX)
	isz write_bytes = write(file->fd, data, size);
	return write_bytes;

#elif defined(LT_WINDOWS)
	DWORD write_bytes = 0;
	BOOL err = WriteFile(file->hnd, data, size, &write_bytes, NULL);
	if (err == FALSE)
		return -1;
	return write_bytes;
#endif
}

usz lt_file_size(lt_file_t* file) {
	return file->size;
}

isz lt_file_vprintf(lt_file_t* file, char* fmt, va_list argl) {
	return lt_io_vprintf((lt_io_callback_t)lt_file_write, file, fmt, argl);
}

isz lt_file_printf(lt_file_t* file, char* fmt, ...) {
	va_list argl;
	va_start(argl, fmt);
	isz bytes = lt_io_vprintf((lt_io_callback_t)lt_file_write, file, fmt, argl);
	va_end(argl);
	return bytes;
}

