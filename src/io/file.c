#include <lt/io.h>
#include <lt/lt.h>
#include <lt/mem.h>
#include <lt/str.h>
#include <lt/text.h>

#include <stdio.h>

#if defined(LT_UNIX)
#	include <fcntl.h>
#	include <sys/stat.h>
#	include <unistd.h>
#	include <errno.h>

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

lt_file_t* lt_fopenp(lstr_t path_, lt_file_mode_t mode, lt_file_perms_t perms, lt_alloc_t alloc[static 1]) {
	if (path_.len > LT_PATH_MAX)
		return NULL; // !! LT_ERR_PATH_TOO_LONG
	char path[LT_PATH_MAX + 1];
	memcpy(path, path_.str, path_.len);
	path[path_.len] = 0;

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
		lt_werrf("LT_FILE_RW has not been implemented for windows\n"); // !!

	if (h == INVALID_HANDLE_VALUE)
		return NULL;

	lt_file_t* file = lt_malloc(alloc, sizeof(lt_file_t));
	file->hnd = h;
	file->size = size;
	return file;

#else
	LT_NOT_IMPLEMENTED();
#endif
}

void lt_fclose(const lt_file_t file[static 1], lt_alloc_t alloc[static 1]) {
#if defined(LT_UNIX)
	close(file->fd);

#elif defined(LT_WINDOWS)
	CloseHandle(file->hnd);

#else
	LT_NOT_IMPLEMENTED();
#endif

	lt_mfree(alloc, file);
}

b8 lt_fexistp(lstr_t path) {
	if (path.len > LT_PATH_MAX)
		return LT_ERR_PATH_TOO_LONG;
	char cpath[LT_PATH_MAX + 1];
	memcpy(cpath, path.str, path.len);
	cpath[path.len] = 0;

#if defined(LT_UNIX)
	struct stat stat_res;
	if (stat(cpath, &stat_res) == 0)
		return 1;
	return errno == EACCES; // if the error was permission denied, the file might still exist, just in an inaccesible state or location
#else
	LT_NOT_IMPLEMENTED();
#endif
}

lt_err_t lt_fremovep(lstr_t path) {
	if (path.len > LT_PATH_MAX)
		return LT_ERR_PATH_TOO_LONG;
	char cpath[LT_PATH_MAX + 1];
	memcpy(cpath, path.str, path.len);
	cpath[path.len] = 0;

	if (remove(cpath)) {
		return lt_errno();
	}
	return LT_SUCCESS;
}

lt_err_t lt_fmovep(lstr_t from, lstr_t to) {
	if (from.len > LT_PATH_MAX || to.len > LT_PATH_MAX)
		return LT_ERR_PATH_TOO_LONG;

	char cfrom[LT_PATH_MAX + 1];
	memcpy(cfrom, from.str, from.len);
	cfrom[from.len] = 0;

	char cto[LT_PATH_MAX + 1];
	memcpy(cto, to.str, to.len);
	cto[to.len] = 0;

	if (rename(cfrom, cto) < 0)
		return LT_ERR_UNKNOWN; // !!
	return LT_SUCCESS;
}

lt_err_t lt_fcopyp(lstr_t from, lstr_t to, void* buf, usz bufsz, lt_alloc_t alloc[static 1]) {
	lt_err_t err;

	lt_file_t* inf = lt_fopenp(from, LT_FILE_R, 0, alloc);
	if (inf == NULL)
		return LT_ERR_UNKNOWN;

	lt_file_t* outf = lt_fopenp(to, LT_FILE_W, 0, alloc);
	if (outf == NULL) {
		err = LT_ERR_UNKNOWN;
		goto err0;
	}

	isz res;
	while ((res = lt_fread(inf, buf, bufsz))) {
		if (res < 0) {
			err = -res;
			goto err1;
		}
		res = lt_fwrite(outf, buf, res);
		if (res < 0) {
			err = -res;
			goto err1;
		}
	}

	err = LT_SUCCESS;

err1:	lt_fclose(inf, alloc);
err0:	lt_fclose(outf, alloc);
		return err;
}

lt_err_t lt_flinkp(lstr_t from, lstr_t to) {
	if (from.len > LT_PATH_MAX || to.len > LT_PATH_MAX)
		return LT_ERR_PATH_TOO_LONG;

	char cfrom[LT_PATH_MAX + 1];
	memcpy(cfrom, from.str, from.len);
	cfrom[from.len] = 0;

	char cto[LT_PATH_MAX + 1];
	memcpy(cto, to.str, to.len);
	cto[to.len] = 0;

#if defined(LT_UNIX)
	if (link(cto, cfrom) < 0)
		return LT_ERR_UNKNOWN; // !!
#else
	LT_NOT_IMPLEMENTED();
#endif
	return LT_SUCCESS;
}

lt_err_t lt_fsymlinkp(lstr_t from, lstr_t to) {
	if (from.len > LT_PATH_MAX || to.len > LT_PATH_MAX)
		return LT_ERR_PATH_TOO_LONG;

	char cfrom[LT_PATH_MAX + 1];
	memcpy(cfrom, from.str, from.len);
	cfrom[from.len] = 0;

	char cto[LT_PATH_MAX + 1];
	memcpy(cto, to.str, to.len);
	cto[to.len] = 0;

#if defined(LT_UNIX)
	if (symlink(cto, cfrom) < 0)
		return LT_ERR_UNKNOWN; // !!
#else
	LT_NOT_IMPLEMENTED();
#endif
	return LT_SUCCESS;
}

lt_err_t lt_freadallp(lstr_t path, lstr_t out[static 1], lt_alloc_t alloc[static 1]) {
	lt_file_t* file = lt_fopenp(path, LT_FILE_R, 0, alloc);
	if (!file)
		return LT_ERR_UNKNOWN; // !!

	usz size = lt_fsize(file);
	char* data = lt_malloc(alloc, size);
	if (!data) {
		lt_fclose(file, alloc);
		return LT_ERR_OUT_OF_MEMORY;
	}

	isz res = lt_fread(file, data, size);
	lt_fclose(file, alloc);
	if (res < 0) {
		lt_mfree(alloc, data);
		return -res;
	}
	*out = LSTR(data, res);
	return LT_SUCCESS;
}

isz lt_fread(lt_file_t file[static 1], void* data, usz size) {
#if defined(LT_UNIX)
	isz read_bytes = read(file->fd, data, size);
	if (read_bytes < 0)
		return -LT_ERR_UNKNOWN; // !!
	return read_bytes;

#elif defined(LT_WINDOWS)
	DWORD read_bytes = 0;
	BOOL err = ReadFile(file->hnd, data, size, &read_bytes, NULL);
	if (err == FALSE)
		return -LT_ERR_UNKNOWN; // !!
	return read_bytes;

#else
	LT_NOT_IMPLEMENTED();
#endif
}

#include <lt/strstream.h>

lt_err_t lt_freadallp_utf8(lstr_t path, lstr_t out[static 1], lt_alloc_t alloc[static 1]) {
	lt_err_t err;

	lstr_t file_data;
	if ((err = lt_freadallp(path, &file_data, alloc)))
		return err;

	if (lt_lsprefix(file_data, LT_UTF16LE_BOM)) {
		lt_strstream_t ss;
		if ((err = lt_strstream_create(&ss, alloc))) {
			lt_mfree(alloc, file_data.str);
			return err;
		}

		file_data.len &= ~1; // make sure that the length is 16-aligned

		u16* it = (u16*)file_data.str, *end = (u16*)(file_data.str + file_data.len);
		while (it < end) {
			u16 wlo = *it++;
			if ((wlo & 0xFC00) == 0xD800 && it < end) {
				u16 whi = *it++;
				u32 c = (wlo & 0x03FF) | ((whi & 0x03FF) << 10);
				lt_strstream_writec(&ss, c);
			}
			else
				lt_strstream_writec(&ss, wlo);
		}

		lt_mfree(alloc, file_data.str);
		file_data = ss.str;
	}

	else if (lt_lsprefix(file_data, LT_UTF16BE_BOM)) {
		lt_strstream_t ss;
		if ((err = lt_strstream_create(&ss, alloc))) {
			lt_mfree(alloc, file_data.str);
			return err;
		}

		file_data.len &= ~1; // make sure that the length is 16-aligned

		// !! i can't be bothered to find a file encoded in big-endian utf-16, no clue if this
		//    actually does what it's supposed to.

		u16* it = (u16*)file_data.str, *end = (u16*)(file_data.str + file_data.len);
		while (it < end) {
			u16 wlo = *it++;
			if ((wlo & 0x00FC) == 0x00D8 && it < end) {
				u16 whi = *it++;
				u32 c = ((wlo & 0xFF03) << 10) | (whi & 0xFF03);
				lt_strstream_writec(&ss, c);
			}
			else
				lt_strstream_writec(&ss, wlo);
		}

		lt_mfree(alloc, file_data.str);
		file_data = ss.str;
	}

	*out = file_data;
	return LT_SUCCESS;
}

isz lt_fwrite(lt_file_t file[static 1], const void* data, usz size) {
#if defined(LT_UNIX)
	isz write_bytes = write(file->fd, data, size);
	if (write_bytes < 0)
		return -LT_ERR_UNKNOWN; // !!
	return write_bytes;

#elif defined(LT_WINDOWS)
	DWORD write_bytes = 0;
	BOOL err = WriteFile(file->hnd, data, size, &write_bytes, NULL);
	if (err == FALSE)
		return -LT_ERR_UNKNOWN; // !!
	return write_bytes;

#else
	LT_NOT_IMPLEMENTED();
#endif
}

usz lt_fsize(lt_file_t file[static 1]) {
	return file->size;
}

isz lt_vfprintf(lt_file_t file[static 1], const char* fmt, va_list argl) {
	return lt_io_vprintf((lt_write_fn_t)lt_fwrite, file, fmt, argl);
}

isz lt_fprintf(lt_file_t file[static 1], const char* fmt, ...) {
	va_list argl;
	va_start(argl, fmt);
	isz res = lt_io_vprintf((lt_write_fn_t)lt_fwrite, file, fmt, argl);
	va_end(argl);
	return res;
}

