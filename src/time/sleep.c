#include <lt/lt.h>
#include <lt/time.h>

#if defined(LT_UNIX)
#	include <sys/time.h>
#	include <time.h>

#	define SLEEP(x) clock_nanosleep(CLOCK_MONOTONIC, 0, &(x), NULL)
#	define TIMESPEC struct timespec

#elif defined(LT_WINDOWS)
#	define WIN32_LEAN_AND_MEAN
#	include <windows.h>

#	define SLEEP(x) Sleep(LT_SEC_TO_MSEC((x).tv_sec) + LT_NSEC_TO_MSEC((x).tv_nsec))
#	define TIMESPEC struct { u64 tv_sec, tv_nsec; }
#endif

void lt_sleep_nsec(u64 nsec) {
	TIMESPEC t;
	t.tv_sec = 0;
	t.tv_nsec = nsec;
	SLEEP(t);
}

void lt_sleep_usec(u64 usec) {
	TIMESPEC t;
	t.tv_sec = 0;
	t.tv_nsec = LT_USEC_TO_NSEC(usec);
	SLEEP(t);
}

void lt_sleep_msec(u64 msec) {
	TIMESPEC t;
	t.tv_sec = 0;
	t.tv_nsec = LT_MSEC_TO_NSEC(msec);
	SLEEP(t);
}

void lt_sleep_sec(u64 sec) {
	TIMESPEC t;
	t.tv_sec = sec;
	t.tv_nsec = 0;
	SLEEP(t);
}