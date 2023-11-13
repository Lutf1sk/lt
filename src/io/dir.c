#include <lt/io.h>
#include <lt/str.h>
#include <lt/mem.h>

#include <dirent.h>
#include <sys/stat.h>

typedef
struct lt_dir {
	DIR* dp;
	lt_dirent_t ent;
} lt_dir_t;

lt_dir_t* lt_dopenp(lstr_t path, lt_alloc_t* alloc) {
	if (path.len >= LT_PATH_MAX)
		return NULL; // !! LT_ERR_PATH_TOO_LONG
	char cpath[LT_PATH_MAX];
	memcpy(cpath, path.str, path.len);
	cpath[path.len] = 0;

	lt_dir_t* dir = lt_malloc(alloc, sizeof(lt_dir_t));
	if (dir == NULL)
		return NULL;

	dir->dp = opendir(cpath);
	if (dir->dp == NULL) {
		lt_mfree(alloc, dir);
		return NULL;
	}
	return dir;
}

void lt_dclose(lt_dir_t* dir, lt_alloc_t* alloc) {
	closedir(dir->dp);
	lt_mfree(alloc, dir);
}

lt_dirent_t* lt_dread(lt_dir_t* dir) {
	struct dirent* dirent = readdir(dir->dp);
	if (dirent == NULL)
		return NULL;

	switch (dirent->d_type) {
	case DT_REG: dir->ent.type = LT_DIRENT_FILE; break;
	case DT_DIR: dir->ent.type = LT_DIRENT_DIR; break;
	case DT_LNK: dir->ent.type = LT_DIRENT_SYMLINK; break;
	default: dir->ent.type = LT_DIRENT_UNKNOWN; break;
	}

	dir->ent.name = lt_lsfroms(dirent->d_name);
	return &dir->ent;
}

lt_err_t lt_dcopyp(lstr_t from, lstr_t to, void* buf, usz bufsz, lt_alloc_t* alloc) {
	lt_err_t err, ret = LT_SUCCESS;

	if ((err = lt_mkdir(to)) && err != LT_ERR_EXISTS)
		return err;

	lt_dir_t* dir = lt_dopenp(from, alloc);
	if (!dir)
		return LT_ERR_UNKNOWN;

	lt_dirent_t* dirent;
	while ((dirent = lt_dread(dir))) {
		if (lt_lseq(dirent->name, CLSTR(".")) || lt_lseq(dirent->name, CLSTR("..")))
			continue;

		lstr_t from_ent = lt_lsbuild(alloc, "%S/%S", from, dirent->name);
		lstr_t to_ent = lt_lsbuild(alloc, "%S/%S", to, dirent->name);

		switch (dirent->type) {
		case LT_DIRENT_DIR:
			if ((err = lt_dcopyp(from_ent, to_ent, buf, bufsz, alloc)))
				ret = err;
			break;

		case LT_DIRENT_FILE:
			if ((err = lt_fcopyp(from_ent, to_ent, buf, bufsz, alloc)))
				ret = err;
			break;

		default:
			break;
		}

		lt_mfree(alloc, to_ent.str);
		lt_mfree(alloc, from_ent.str);
	}

err0:	lt_dclose(dir, alloc);
		return ret;
}

lt_err_t lt_mkdir(lstr_t path) {
	if (path.len >= LT_PATH_MAX)
		return LT_ERR_PATH_TOO_LONG;
	char cpath[LT_PATH_MAX];
	memcpy(cpath, path.str, path.len);
	cpath[path.len] = 0;

	int res = mkdir(cpath, 0755);
	if (res < 0)
		return lt_errno();
	return LT_SUCCESS;
}

lt_err_t lt_mkpath(lstr_t path) {
	if (path.len >= LT_PATH_MAX)
		return LT_ERR_PATH_TOO_LONG;

	lt_err_t err;

	if ((err = lt_mkdir(path)) == LT_SUCCESS)
		return LT_SUCCESS;
	if (err == LT_ERR_EXISTS)
		return LT_SUCCESS;

	if (err != LT_ERR_NOT_FOUND)
		return err;

	if ((err = lt_mkpath(lt_lsdirname(path))))
		return err;

	if ((err = lt_mkdir(path)) == LT_SUCCESS)
		return LT_SUCCESS;
	if (err == LT_ERR_EXISTS)
		return LT_SUCCESS;
	return err;
}
