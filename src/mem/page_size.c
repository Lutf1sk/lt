#include <lt/mem.h>
#include <lt/lt.h>

#if defined(LT_UNIX)
#	include <unistd.h>
#elif defined(LT_WINDOWS)
#	define WIN32_LEAN_AND_MEAN 1
#	include <sysinfoapi.h>
#endif

static usz page_size = 0;

static
usz lt_page_size_cached(void) {
	return page_size;
}

static
usz lt_page_size_initial(void) {
#if defined(LT_UNIX)

	page_size = sysconf(_SC_PAGE_SIZE);

#elif defined(LT_WINDOWS)

	SYSTEM_INFO sys_info;
	GetNativeSystemInfo(&sys_info);
	page_size = sys_info.dwPageSize;

#endif

	lt_get_pagesize = lt_page_size_cached;
	return page_size;
}

usz (*lt_get_pagesize)(void) = lt_page_size_initial;


