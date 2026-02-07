#include <lt2/time.h>

#ifdef ON_UNIX
#	ifndef _POSIX_C_SOURCE
#		define _POSIX_C_SOURCE 199309L
#	endif
#	include <sys/time.h>
#	include <time.h>
#elif ON_WINDOWS
#	define WIN32_LEAN_AND_MEAN
#	include <windows.h>
#elifdef ON_WASI
#	include <lt2/wasi.h>
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
	u64 whole = ((u64)ticks.QuadPart / (u64)freq.QuadPart) * 1000000000ULL;
	u64 part  = ((u64)ticks.QuadPart % (u64)freq.QuadPart) * 1000000000ULL / (u64)freq.QuadPart;
	return whole + part;
#elifdef ON_WASI
	u64 ns;
	__wasi_clock_time_get(WASI_CLOCKID_MONOTONIC, 1, &ns);
	return ns;
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
	Sleep(NS_TO_MS(nsec));
#elifdef ON_WASI
	wasi_subscription_t sub = {
		.usr  = 1,
		.type = WASI_EVENTTYPE_CLOCK,
		.clock = {
			.id        = WASI_CLOCKID_MONOTONIC,
			.timeout   = nsec,
			.precision = 1,
			.flags     = 0
		}
	};
	wasi_event_t ev;
	usz ev_count;
	__wasi_poll_oneoff(&sub, &ev, 1, &ev_count);
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

