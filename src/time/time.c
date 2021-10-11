#include <lt/time.h>
#include <lt/lt.h>

#if defined(LT_UNIX)
#	include <sys/time.h>
#	include <time.h>
#elif defined(LT_WINDOWS)
#	define WIN32_LEAN_AND_MEAN
#	include <Windows.h>
#	include <stdint.h> // portable: uint64_t   MSVC: __int64

typedef struct timeval {
	long tv_sec;
	long tv_usec;
} timeval;

static
int gettimeofday(struct timeval* tp, struct timezone* tzp) {
	// Note: some broken versions only have 8 trailing zero's, the correct epoch has 9 trailing zero's
	// This magic number is the number of 100 nanosecond intervals since January 1, 1601 (UTC)
	// until 00:00:00 January 1, 1970
	static const uint64_t EPOCH = ((uint64_t) 116444736000000000ULL);

	SYSTEMTIME  system_time;
	FILETIME    file_time;
	uint64_t    time;

	GetSystemTime( &system_time );
	SystemTimeToFileTime( &system_time, &file_time );
	time =  ((uint64_t)file_time.dwLowDateTime )      ;
	time += ((uint64_t)file_time.dwHighDateTime) << 32;

	tp->tv_sec  = (long) ((time - EPOCH) / 10000000L);
	tp->tv_usec = (long) (system_time.wMilliseconds * 1000);
	return 0;
}
#endif

u64 lt_hfreq_time_sec(void) {
	struct timespec ts;
	clock_gettime(CLOCK_MONOTONIC, &ts);
	return ts.tv_sec;
}

u64 lt_hfreq_time_msec(void) {
	struct timespec ts;
	clock_gettime(CLOCK_MONOTONIC, &ts);
	return (u64)ts.tv_sec * 1000 + (u64)ts.tv_nsec / 1000000;
}

u64 lt_hfreq_time_usec(void) {
	struct timespec ts;
	clock_gettime(CLOCK_MONOTONIC, &ts);
	return (u64)ts.tv_sec * 1000000 + (u64)ts.tv_nsec / 1000;
}

