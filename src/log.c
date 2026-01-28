#include <lt2/log.h>
#include <lt2/str.h>

#include <time.h>
#include <unistd.h>

log_sink* default_log_sink = &(log_sink) {
	.type = LOGSINK_FILE,
	.file = {
		.fd = STDERR_FILENO
	}
};

static ls colored_prefixes[] = {
	[LOG_EMERG]  = ls("\x1b[41;97;1m"),
	[LOG_ALERT]  = ls("\x1b[101;30;1m"),
	[LOG_CRIT]   = ls("\x1b[41;30;1m"),
	[LOG_ERR]    = ls("\x1b[31;1m"),
	[LOG_WARN]   = ls("\x1b[93;1m"),
	[LOG_NOTICE] = ls("\x1b[94;1m"),
	[LOG_INFO]   = ls("\x1b[97;1m"),
	[LOG_DEBUG]  = ls("\x1b[37;1m"),
};

static ls colored_severities[] = {
	[LOG_EMERG]  = ls("EMERG!\x1b[0;1m"),
	[LOG_ALERT]  = ls("ALERT!\x1b[0;1m"),
	[LOG_CRIT]   = ls(" CRIT!\x1b[0;1m"),
	[LOG_ERR]    = ls("   ERR\x1b[0m"),
	[LOG_WARN]   = ls("  WARN\x1b[0m"),
	[LOG_NOTICE] = ls("NOTICE\x1b[0m"),
	[LOG_INFO]   = ls("  INFO\x1b[0m"),
	[LOG_DEBUG]  = ls(" DEBUG\x1b[0m"),
};

static ls severities[] = {
	[LOG_EMERG]  = ls("EMERG!"),
	[LOG_ALERT]  = ls("ALERT!"),
	[LOG_CRIT]   = ls(" CRIT!"),
	[LOG_ERR]    = ls("   ERR"),
	[LOG_WARN]   = ls("  WARN"),
	[LOG_NOTICE] = ls("NOTICE"),
	[LOG_INFO]   = ls("  INFO"),
	[LOG_DEBUG]  = ls(" DEBUG"),
};

static ls colored_suffix = ls("\x1b[0m\n");

static
isz write_out(void* usr, const void* data, usz size) {
	return write(*(file_handle*)usr, data, size);
}

i32 vlogf(log_sink* sink, u8 info, const char* fmt, va_list args) {
	if (!sink)
		sink = default_log_sink;

	u8  severity  = info & LOG_SEVERITY_MASK;
	u64 unix_time = time(NULL);

	char msg_buf[LOG_MAX_SIZE];
	ls msg = vlsprintf(lls(msg_buf, sizeof(msg_buf)), fmt, args);

	do {
		if (sink->type == LOGSINK_FILE) {
			if (sink->file.color)
				lprintf_fn(write_out, &sink->file.fd, "{ls}{dt64} {ls} {ls}{ls}", colored_prefixes[severity], unix_time, colored_severities[severity], msg, colored_suffix);
			else
				lprintf_fn(write_out, &sink->file.fd, "{dt64} {ls} {ls}\n", unix_time, severities[severity], msg);
		}

		else if (sink->type == LOGSINK_NET_SYSLOG) {
			throw(err_fail, ERR_NOT_IMPLEMENTED, "syslog sink not implemented");
		}

		sink = sink->next;
	} while (sink);

	return 0;
}

i32 llogf(log_sink* sink, u8 info, const char* fmt, ...) {
	va_list args;
	va_start(args, fmt);
	i32 res = vlogf(sink, info, fmt, args);
	va_end(args);
	return res;
}

