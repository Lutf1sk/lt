#include <lt/io.h>
#include <lt/lt.h>

#if defined(LT_UNIX)
#	include <execinfo.h>
#	include <stdlib.h>
#elif defined(LT_WINDOWS)

#endif

static
void lt_backtrace_print(char* str) {
	lt_printf("~\t%s\n", str);
}

void lt_backtrace(void(*callback)(char*)) {
	if (!callback) {
		lt_printls(CLSTR("BACKTRACE\n"));
		callback = lt_backtrace_print;
	}
#if defined(LT_UNIX)
#	define BACKTRACE_MAX_DEPTH 128
	void* symbols[BACKTRACE_MAX_DEPTH];
	int symbol_count = backtrace(symbols, BACKTRACE_MAX_DEPTH);

	char** symbol_names = backtrace_symbols(symbols, symbol_count);
	if (!symbol_names) {
		lt_printls(CLSTR("Backtrace failed\n"));
		return;
	}

	for (int i = 1; i < symbol_count; ++i)
		callback(symbol_names[i]);
	free(symbol_names);

#elif defined(LT_WINDOWS)
	lt_werr(CLSTR("Backtracing is not implemented for windows yet\n"));
#endif
}

