#pragma once

#include <lt2/common.h>

#define LOG_EMERG  0
#define LOG_ALERT  1
#define LOG_CRIT   2
#define LOG_ERR    3
#define LOG_WARN   4
#define LOG_NOTICE 5
#define LOG_INFO   6
#define LOG_DEBUG  7
#define LOG_SEVERITY_MASK 0x07

#define LOGSINK_FILE       0
#define LOGSINK_NET_SYSLOG 1

#define LOG_MAX_SIZE 4096

typedef struct log_sink {
	u8 type;
	union {
		struct {
			ls  host;
			u16 port;
			file_handle sockfd;
		} network;

		struct {
			file_handle fd;
			u8 color;
		} file;
	};
	log_sink* next;
	log_sink* fallback;
} log_sink;

extern log_sink* default_log_sink;

i32 vlogf(log_sink* sink, u8 info, const char* fmt, va_list args);
i32 llogf(log_sink* sink, u8 info, const char* fmt, ...);

