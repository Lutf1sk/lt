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

#define LOGSINK_FILE    0
#define LOGSINK_NETWORK 1
#define LOGSINK_RINGBUF 2

typedef struct log_sink {
	u8 type;
	union {
		struct {
			logfile_header* header;
		} disk_ringbuf;

		struct {
			ls  host;
			u16 port;
			i32 sockfd;
		} network;

		struct {
			i32 fd;
			u8  color;
		} file;
	};
	log_sink* next;
	log_sink* fallback;
} log_sink;

// --- ringbuf

typedef struct PACKED(logfile_header) {
	u8 version;
	u8 headings;
	u8 pad[2];

	u32 strtab_start;
	u32 strtab_max;
	u32 strtab_size;

	u32 enttab_start;
	u32 enttab_max;
	u32 enttab_count;
} logfile_header;

typedef struct PACKED(logfile_entry) {
	u8  info;
	u8  pad[1];
	u16 fields;
	u32 size;
	u64 offset;
	u64 time;
} logfile_entry;

log_sink log_map_ringbuf(ls path, u8 mode);
log_sink log_alloc_ringbuf(usz max_entry_count, usz max_strtab_size);

// ---

#define MAX_LOG_FIELDS 32
extern thread_local usz log_field_count;
extern thread_local u64 log_fields[MAX_LOG_FIELDS];

INLINE
usz log_push(u64 val) {
	if (log_field_count < MAX_LOG_FIELDS)
		log_fields[log_field_count++] = val;
	return log_field_count - 1;
}

INLINE
void log_pop(void) {
	if (log_field_count)
		--log_field_count;
}

INLINE
void log_clear(void) {
	log_field_count = 0;
}

i32 vlogf(log_sink* sink, u8 info, const char* fmt, va_list args);
i32 llogf(log_sink* sink, u8 info, const char* fmt, ...);

