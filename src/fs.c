#include <lt2/common.h>

#ifdef ON_UNIX
#	include <lt2/posix.h>

#	include <unistd.h>
#	include <fcntl.h>
#	include <dirent.h>
#	include <limits.h>
#	include <sys/mman.h>
#	include <sys/stat.h>
#	include <sys/inotify.h>

#	define INOTIFY_BUFSZ (16 * (sizeof(struct inotify_event) * NAME_MAX + 1))

thread_local char path_buf[PATH_BUF_SIZE];

b8 convert_path(ls path, err* err) {
	if (path.size >= PATH_BUF_SIZE) {
		throw(err, ERR_PATH_TOO_LONG, "path length exceeds maximum of " AS_CSTR(PATH_BUF_SIZE) " characters");
		return 1;
	}
	memcpy(path_buf, path.ptr, path.size);
	path_buf[path.size] = 0;
	return 0;
}

ls fmapall(ls path, u8 mode, err* err) {
	file_handle file = lfopen(path, mode, err);
	if (file < 0)
		return ls("");

	struct stat st;
	fstat(file, &st);

	int posix_flags = MAP_SHARED;
	int posix_prot  = posix_prot_tab[mode];
	void* block = mmap(NULL, st.st_size, posix_prot, posix_flags, file, 0);
	close(file);
	if (block == MAP_FAILED) {
		throw_errno(err);
		return ls("");
	}

	return lls(block, st.st_size);
}

void funmap(ls mapping, err* err) {
	if (munmap(mapping.ptr, mapping.size) < 0)
		throw_errno(err);
}

file_handle fcreate(ls path, u8 prot, err* err) {
	if (convert_path(path, err))
		return -1;
	int posix_prot = posix_file_prot_tab[prot & 0b111];
	int fd = open(path_buf, O_WRONLY | O_CREAT, posix_prot);
	if (fd < 0) {
		throw_errno(err);
		return -1;
	}
	return fd;
}

file_handle lfopen(ls path, u8 mode, err* err) {
	if (convert_path(path, err))
		return -1;
	int posix_mode = posix_file_mode_tab[mode & 0b111];
	int fd = open(path_buf, posix_mode, 0);
	if (fd < 0)
		throw_errno(err);
	return fd;
}

void lfclose(file_handle file, err* err) {
	if (close(file) < 0)
		throw_errno(err);
}

b8 fsetsize(file_handle fd, usz size, err* err) {
	if (ftruncate(fd, size) < 0) {
		throw_errno(err);
		return 0;
	}
	return 1;
}

usz lfwrite(file_handle file, const void* data, usz size, err* err) {
	isz res = write(file, data, size);
	if (res < 0) {
		throw_errno(err);
		return 0;
	}
	return res;
}

usz lfread(file_handle file, void* data, usz size, err* err) {
	isz res = read(file, data, size);
	if (res < 0) {
		throw_errno(err);
		return 0;
	}
	return res;
}

dir_handle ldopen(ls path, err* err) {
	if (convert_path(path, err))
		return NULL;

	DIR* d = opendir(path_buf);
	if (!d) {
		throw_errno(err);
		return NULL;
	}
	return (dir_handle)d;
}

void ldclose(dir_handle dir, err* err) {
	if (!dir)
		throw(err, ERR_BAD_HANDLE, "cannot close null dir_handle");
	else if (closedir((DIR*)dir) < 0)
		throw_errno(err);
}

b8 ldnext(dir_handle dir, dir_entry ent[static 1], err* err) {
	if (!dir) {
		throw(err, ERR_BAD_HANDLE, "cannot read null dir_handle");
		return 0;
	}
	struct dirent* de = readdir((DIR*)dir);
	if (!de)
		return 0;

	switch (de->d_type) {
	case DT_REG: ent->type = FS_FILE; break;
	case DT_DIR: ent->type = FS_DIR;  break;
	case DT_LNK: ent->type = FS_LINK; break;
	default:     ent->type = FS_ANY;  break;
	}

	usz name_size = strlen(de->d_name);
	if (name_size > FILENAME_BUF_SIZE) {
		throw(err, ERR_LIMIT_EXCEEDED, "name of directory entry exceeds maximum length");
		name_size = FILENAME_BUF_SIZE;
	}
	memcpy(ent->name, de->d_name, name_size);
	ent->name_size = name_size;
	return 1;
}

b8 lfstat(ls path, file_stat out_stat[static 1], err* err) {
	if (convert_path(path, err))
		return 0;

	struct stat st;
	if (stat(path_buf, &st) < 0) {
		throw_errno(err);
		return 0;
	}

	out_stat->type = posix_file_type(st.st_mode);
	out_stat->size = st.st_size;
	return 1;
}

u32 fwatch_once(ls path, u32 events, err* err) {
	if (convert_path(path, err))
		return WATCH_FAILED;

	int in_fd = inotify_init();
	if UNLIKELY (in_fd < 0) {
		throw_errno(err);
		return WATCH_FAILED;
	}

	int posix_events = 0;
	if (events & WATCH_MODIFIED)
		posix_events |= IN_MODIFY;
	if (events & WATCH_SAVED)
		posix_events |= IN_CLOSE_WRITE;
	if (events & WATCH_DELETED)
		posix_events |= IN_DELETE_SELF;
	if (events & WATCH_MOVED)
		posix_events |= IN_MOVE_SELF;

	int watch_fd = inotify_add_watch(in_fd, path_buf, posix_events);
	if UNLIKELY (watch_fd < 0) {
		close(in_fd);
		throw_errno(err);
		return WATCH_FAILED;
	}

	u8 buf[INOTIFY_BUFSZ];
	ssize_t len = read(in_fd, buf, sizeof(buf));
	if UNLIKELY (len < 0) {
		close(in_fd);
		throw_errno(err);
		return WATCH_FAILED;
	}

	struct inotify_event* it  = (void*)buf;
	struct inotify_event* end = (void*)(buf + len);

	u32 out_events = 0;
	while (it < end) {
		if (it->mask & IN_MODIFY)
			out_events |= WATCH_MODIFIED;
		else if (it->mask & IN_CLOSE_WRITE)
			out_events |= WATCH_SAVED;
		else if (it->mask & IN_DELETE_SELF)
			out_events |= WATCH_DELETED;
		else if (it->mask & IN_MOVE_SELF)
			out_events |= WATCH_MOVED;
		it = (void*)((u8*)(it + 1) + it->len);
	}

	close(in_fd);
	return out_events;
}

#endif // ON_UNIX

