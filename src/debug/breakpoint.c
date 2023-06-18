#include <lt/lt.h>

#if defined(LT_UNIX)
#	include <signal.h>
#	define BREAKPOINT raise(SIGTRAP)
#elif defined(LT_WINDOWS)
#	define BREAKPOINT __debugbreak()
#endif

void lt_breakpoint() {
	BREAKPOINT;
}

