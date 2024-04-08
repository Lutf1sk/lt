#include <lt/io.h>
#include <lt/mem.h>

#ifdef LT_LINUX

#include <sys/stat.h>

lt_dirent_type_t lt_enttype_from_unix(int mode) {
	switch (mode & S_IFMT) {
	case S_IFDIR: return LT_DIRENT_DIR;
	case S_IFLNK: return LT_DIRENT_SYMLINK;
	case S_IFREG: return LT_DIRENT_FILE;

	case S_IFBLK: // block device
	case S_IFCHR: // character device
		return LT_DIRENT_DEVICE;

	case S_IFIFO: return LT_DIRENT_PIPE;
	case S_IFSOCK: return LT_DIRENT_SOCKET;
	default: return LT_DIRENT_UNKNOWN;
	}
}

static
lt_file_perms_t convert_permissions(int mode) {
	lt_file_perms_t perms = 0;

	// !! this is really oversimplified and needs more logic to really provide any useful data
	if ((mode & S_IRUSR) || (mode & S_IRGRP) || (mode & S_IROTH)) {
		perms |= LT_FILE_PERMIT_R;
	}
	if ((mode & S_IWUSR) || (mode & S_IWGRP) || (mode & S_IWOTH)) {
		perms |= LT_FILE_PERMIT_W;
	}
	if ((mode & S_IXUSR) || (mode & S_IXGRP) || (mode & S_IXOTH)) {
		perms |= LT_FILE_PERMIT_X;
	}
	return perms;
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

	out_stat->type = lt_enttype_from_unix(st.st_mode);
	out_stat->permit = convert_permissions(st.st_mode);
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

	out_stat->type = lt_enttype_from_unix(st.st_mode);
	out_stat->permit = convert_permissions(st.st_mode);
	out_stat->size = st.st_size;
	return LT_SUCCESS;
}

#elifdef LT_WINDOWS


#endif