#include <lt2/bits.h>
#include <lt2/str.h>
#include <lt2/log.h>

#include <time.h>

static ls severity_names[] = {
	[LOG_EMERG]  = ls("EMERG!\x1b[0;1m"),
	[LOG_ALERT]  = ls("ALERT!\x1b[0;1m"),
	[LOG_CRIT]   = ls(" CRIT!\x1b[0;1m"),
	[LOG_ERR]    = ls("   ERR\x1b[0m"),
	[LOG_WARN]   = ls("  WARN\x1b[0m"),
	[LOG_NOTICE] = ls("NOTICE\x1b[0m"),
	[LOG_INFO]   = ls("  INFO\x1b[0m"),
	[LOG_DEBUG]  = ls(" DEBUG\x1b[0m"),
};

static ls severity_prefixes[] = {
	[LOG_EMERG]  = ls("\x1b[41;97;1m"),
	[LOG_ALERT]  = ls("\x1b[101;30;1m"),
	[LOG_CRIT]   = ls("\x1b[41;30;1m"),
	[LOG_ERR]    = ls("\x1b[31;1m"),
	[LOG_WARN]   = ls("\x1b[93;1m"),
	[LOG_NOTICE] = ls("\x1b[94;1m"),
	[LOG_INFO]   = ls("\x1b[97;1m"),
	[LOG_DEBUG]  = ls("\x1b[37;1m"),
};


log_sink log_map_ringbuf(ls path, u8 mode) {
	(void)severity_names;
	(void)severity_prefixes;
	return (log_sink) {};
}

log_sink log_alloc_ringbuf(usz max_entry_count, usz max_strtab_size) {
	return (log_sink) {};
}

i32 vlogf(log_sink* sink, u8 info, const char* fmt, va_list args) {
	return 0;
}

i32 llogf(log_sink* sink, u8 info, const char* fmt, ...) {
	return 0;
}

