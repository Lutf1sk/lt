#include <lt2/time.h>

#if defined(ON_UNIX)
#	ifndef _POSIX_C_SOURCE
#		define _POSIX_C_SOURCE 199309L
#	endif
#	include <sys/time.h>
#	include <time.h>

#	define TIMESTAMP struct timespec
#	define TIME(x) clock_gettime(CLOCK_MONOTONIC, &x)

#	define SLEEP(x) clock_nanosleep(CLOCK_MONOTONIC, 0, &(x), NULL)
#	define TIMESPEC struct timespec

#elif defined(ON_WINDOWS)
#	define WIN32_LEAN_AND_MEAN
#	include <windows.h>
#	include <stdint.h> // portable: uint64_t   MSVC: __int64

#	define TIMESTAMP struct timestamp
#	define TIME(x) win32_gettime(&x);

#	define SLEEP(x) Sleep(LT_SEC_TO_MSEC((x).tv_sec) + LT_NSEC_TO_MSEC((x).tv_nsec))
#	define TIMESPEC struct { u64 tv_sec, tv_nsec; }

struct timestamp {
	u64 tv_sec, tv_nsec;
};

static
int win32_gettime(TIMESTAMP *spec) {
	_Alignas(16) __int64 wintime;
	GetSystemTimeAsFileTime((FILETIME*)&wintime);
	wintime -= 116444736000000000LL; //1jan1601 to 1jan1970
	spec->tv_sec = wintime / 10000000LL;
	spec->tv_nsec = wintime % 10000000LL * 100;
	return 0;
}

#endif

u64 time_s(void) {
	TIMESTAMP ts;
	TIME(ts);
	return ts.tv_sec + NS_TO_S((u64)ts.tv_nsec);
}

u64 time_ms(void) {
	TIMESTAMP ts;
	TIME(ts);
	return S_TO_MS((u64)ts.tv_sec) + NS_TO_MS((u64)ts.tv_nsec);
}

u64 time_us(void) {
	TIMESTAMP ts;
	TIME(ts);
	return S_TO_US((u64)ts.tv_sec) + NS_TO_US((u64)ts.tv_nsec);
}

u64 time_ns(void) {
	TIMESTAMP ts;
	TIME(ts);
	return S_TO_NS((u64)ts.tv_sec) + (u64)ts.tv_nsec;
}

void sleep_ns(u64 nsec) {
	TIMESPEC t;
	t.tv_sec  = nsec / 1000000000;
	t.tv_nsec = nsec % 1000000000;
	SLEEP(t);
}

void sleep_us(u64 usec) {
	sleep_ns(US_TO_NS(usec));
}

void sleep_ms(u64 msec) {
	sleep_ns(MS_TO_NS(msec));
}

void sleep_s(u64 sec) {
	TIMESPEC t;
	t.tv_sec = sec;
	t.tv_nsec = 0;
	SLEEP(t);
}

