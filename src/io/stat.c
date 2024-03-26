#include <lt/io.h>
#include <lt/mem.h>

#include <sys/stat.h>

static
lt_dirent_type_t convert_filetype(int mode) {
	switch (mode & S_IFMT) {
	case S_IFDIR: return LT_DIRENT_DIR;
	case S_IFLNK: return LT_DIRENT_SYMLINK;
	case S_IFREG: return LT_DIRENT_FILE;

	case S_IFBLK: // block device
	case S_IFCHR: // character device
	case S_IFIFO: // FIFO/pipe
	case S_IFSOCK: // socket
	default: return LT_DIRENT_UNKNOWN;
	}
}

lt_err_t lt_statp(lstr_t path, lt_stat_t* out_stat) {
	if (path.len > LT_PATH_MAX)
		return LT_ERR_PATH_TOO_LONG;

	char cpath[LT_PATH_MAX + 1];
	memcpy(cpath, path.str, path.len);
	cpath[path.len] = 0;

	struct stat st;
	if (stat(cpath, &st) < 0) {
		return lt_errno();
	}

	out_stat->type = convert_filetype(st.st_mode);
	out_stat->size = st.st_size;
	return LT_SUCCESS;
}

lt_err_t lt_lstatp(lstr_t path, lt_stat_t* out_stat) {
	if (path.len > LT_PATH_MAX)
		return LT_ERR_PATH_TOO_LONG;

	char cpath[LT_PATH_MAX + 1];
	memcpy(cpath, path.str, path.len);
	cpath[path.len] = 0;

	struct stat st;
	if (lstat(cpath, &st) < 0) {
		return lt_errno();
	}

	out_stat->type = convert_filetype(st.st_mode);
	out_stat->size = st.st_size;
	return LT_SUCCESS;
}
