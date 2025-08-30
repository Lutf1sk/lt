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

typedef struct log_header {
	u8  version;
	u8  headings;
	u16 header_size;

	u32 strtab_size;
	u32 strtab_top;

	u32 max_entries;
	u32 enttab_first;
	u32 entry_count;
} log_header;

typedef struct log_entry {
	u8  info;
	u8  fields;
	u8  pad[2];
	u32 size;
	u32 offset;
	u64 time;
} log_entry;

extern ls log_mapping;
extern log_header* log_file_header;

extern u8* log_strtab;
extern usz log_strtab_mask;

extern log_entry* log_enttab;
extern usz log_enttab_mask;

#define MAX_LOG_FIELDS 32

extern thread_local usz log_field_count;
extern thread_local u64 log_fields[MAX_LOG_FIELDS];

b8 log_open(ls path, u8 mode, err* err);
void log_close(err* err);

void print_log_entry(i32 index);

INLINE
usz log_push_field(u64 val) {
	if (log_field_count < MAX_LOG_FIELDS)
		log_fields[log_field_count++] = val;
	return log_field_count - 1;
}

INLINE
void log_pop_field(void) {
	if (log_field_count)
		--log_field_count;
}

INLINE
void log_clear_fields(void) {
	log_field_count = 0;
}

i32 vlogf(u8 info, const char* fmt, va_list args);
i32 llogf(u8 info, const char* fmt, ...);

