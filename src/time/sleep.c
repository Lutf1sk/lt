#include <lt/lt.h>
#include <lt/time.h>

#if defined(LT_UNIX)
#	include <unistd.h>

#	define SLEEPS(x) sleep(x)
#	define SLEEPMS(x) usleep(LT_MSEC_TO_USEC(x))
#	define SLEEPUS(x) usleep(x)

#elif defined(LT_WINDOWS)
#	define WIN32_LEAN_AND_MEAN
#	include <windows.h>

#	define SLEEPS(x) Sleep(LT_SEC_TO_MSEC(x))
#	define SLEEPMS(x) Sleep(x)
#	define SLEEPUS(x) Sleep(LT_USEC_TO_MSEC(x))
#endif

void lt_sleep_usec(u64 usec) {
	SLEEPUS(usec);
}

void lt_sleep_msec(u64 msec) {
	SLEEPMS(msec);
}

void lt_sleep_sec(u64 sec) {
	SLEEPS(sec);
}