#include <lt2/common.h>

#ifdef ON_UNIX
#	include <fcntl.h>
#	include <sys/mman.h>
#	include <sys/stat.h>

int posix_prot_tab[8] = {
	[0]   = PROT_NONE,
	[R]   = PROT_READ,
	[W]   = PROT_WRITE,
	[X]   = PROT_EXEC,
	[RW]  = PROT_READ  | PROT_WRITE,
	[RX]  = PROT_READ  | PROT_EXEC,
	[WX]  = PROT_WRITE | PROT_EXEC,
	[RWX] = PROT_READ  | PROT_WRITE | PROT_EXEC,
};

int posix_file_mode_tab[8] = {
	[0]   = 0,
	[R]   = O_RDONLY,
	[W]   = O_WRONLY,
	[X]   = 0,
	[RW]  = O_RDWR,
	[RX]  = O_RDONLY,
	[WX]  = O_WRONLY,
	[RWX] = O_RDWR,
};

int posix_file_prot_tab[8] = {
	[0]   = 0,
	[R]   = S_IRUSR | S_IRGRP | S_IROTH,
	[W]   = S_IWUSR,
	[X]   = S_IXUSR | S_IXGRP | S_IXOTH,
	[RW]  = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH,
	[RX]  = S_IRUSR | S_IXUSR | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH,
	[WX]  = S_IWUSR | S_IXUSR | S_IXGRP | S_IXOTH,
	[RWX] = S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH,
};

u8 posix_file_type(int mode) {
	switch (mode & S_IFMT) {
	case S_IFDIR: return FS_DIR;
	case S_IFLNK: return FS_LINK;
	case S_IFREG: return FS_FILE;

	case S_IFBLK: // block device
	case S_IFCHR: // character device
		return FS_ANY;

	case S_IFIFO:  return FS_ANY; // pipe
	case S_IFSOCK: return FS_ANY; // socket
	default: return FS_ANY;
	}
}

#endif
