#include <lt/io.h>
#include <lt/lt.h>

#if defined(LT_UNIX)

typedef
struct lt_file {
	int fd;
	u64 size;
	usz block_size;
	usz it;
	u8* buf;
} lt_file_t;

#elif defined(LT_WINDOWS)
#	define WIN32_LEAN_AND_MEAN 1
#	include <windows.h>

typedef
struct lt_file {
	HANDLE hnd;
	u64 size;
} lt_file_t;

#endif

