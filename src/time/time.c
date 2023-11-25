#include <lt/time.h>
#include <lt/lt.h>

#if defined(LT_UNIX)
#	include <sys/time.h>
#	include <time.h>

#	define TIMESTAMP struct timespec
#	define TIME(x) clock_gettime(CLOCK_MONOTONIC, &x)

#elif defined(LT_WINDOWS)
#	define WIN32_LEAN_AND_MEAN
#	include <windows.h>
#	include <stdint.h> // portable: uint64_t   MSVC: __int64

#	define TIMESTAMP struct timestamp
#	define TIME(x) win32_gettime(&x);

struct timestamp {
	long tv_sec, tv_nsec;
};

int win32_gettime(TIMESTAMP *spec) {
	_Alignas(16) __int64 wintime;
	GetSystemTimeAsFileTime((FILETIME*)&wintime);
	wintime -= 116444736000000000LL; //1jan1601 to 1jan1970
	spec->tv_sec = wintime / 10000000LL;
	spec->tv_nsec = wintime % 10000000LL * 100;
	return 0;
}

#endif

u64 lt_hfreq_time_sec(void) {
	TIMESTAMP ts;
	TIME(ts);
	return ts.tv_sec;
}

u64 lt_hfreq_time_msec(void) {
	TIMESTAMP ts;
	TIME(ts);
	return (u64)ts.tv_sec * 1000 + (u64)ts.tv_nsec / 1000000;
}

u64 lt_hfreq_time_usec(void) {
	TIMESTAMP ts;
	TIME(ts);
	return (u64)ts.tv_sec * 1000000 + (u64)ts.tv_nsec / 1000;
}

u64 lt_hfreq_time_nsec(void) {
	TIMESTAMP ts;
	TIME(ts);
	return (u64)ts.tv_sec * 1000000000 + (u64)ts.tv_nsec;
}
