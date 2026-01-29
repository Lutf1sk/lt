#include <lt2/time.h>

#if defined(ON_UNIX)
#	ifndef _POSIX_C_SOURCE
#		define _POSIX_C_SOURCE 199309L
#	endif
#	include <sys/time.h>
#	include <time.h>
#elif defined(ON_WINDOWS)
#	define WIN32_LEAN_AND_MEAN
#	include <windows.h>
#endif

u64 time_ns(void) {
#ifdef ON_UNIX
	struct timespec ts;
	clock_gettime(CLOCK_MONOTONIC, &ts);
	return S_TO_NS((u64)ts.tv_sec) + (u64)ts.tv_nsec;
#elifdef ON_WINDOWS
	static LARGE_INTEGER freq = {0};
	if UNLIKELY (!freq.QuadPart)
		QueryPerformanceFrequency(&freq);
	LARGE_INTEGER ticks;
	QueryPerformanceCounter(&ticks);
	return ticks.QuadPart * 1000000000 / freq.QuadPart; // this can probably overflow for large (but reasonable) values
#endif
}

u64 time_us(void) {
	return NS_TO_US(time_ns());
}

u64 time_ms(void) {
	return NS_TO_MS(time_ns());
}

u64 time_s(void) {
	return NS_TO_S(time_ns());
}

void sleep_ns(u64 nsec) {
#ifdef ON_UNIX
	struct timespec t;
	t.tv_sec  = nsec / 1000000000;
	t.tv_nsec = nsec % 1000000000;
	clock_nanosleep(CLOCK_MONOTONIC, 0, &t, NULL);
#elifdef ON_WINDOWS
	Sleep(LT_NSEC_TO_MSEC(nsec));
#endif
}

void sleep_us(u64 usec) {
	sleep_ns(US_TO_NS(usec));
}

void sleep_ms(u64 msec) {
	sleep_ns(MS_TO_NS(msec));
}

void sleep_s(u64 sec) {
	sleep_ns(S_TO_NS(sec));
}

