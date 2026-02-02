#pragma once

#include <lt2/common.h>

#define STDIN_FILENO  0
#define STDOUT_FILENO 1
#define STDERR_FILENO 2

typedef struct wasi_iovec {
	u8* buf;
	usz size;
} wasi_iovec_t;

typedef struct wasi_ciovec {
	const u8* buf;
	usz size;
} wasi_ciovec_t;

WASM_IMPORT("wasi_snapshot_preview1", "fd_write")
int __wasi_fd_write(int fd, const wasi_ciovec_t* iovs, usz count, __SIZE_TYPE__* out_written);

INLINE
isz write(int fd, const void* data, usz size) {
	wasi_ciovec_t iov = { .buf = data, .size = size };
	usz written;
	__wasi_fd_write(fd, &iov, 1, &written);
	return written;
}


#define WASI_CLOCKID_REALTIME           0
#define WASI_CLOCKID_MONOTONIC          1
#define WASI_CLOCKID_PROCESS_CPUTIME_ID 2
#define WASI_CLOCKID_THREAD_CPUTIME_ID  3

WASM_IMPORT("wasi_snapshot_preview1", "clock_time_get")
int __wasi_clock_time_get(u32 clock, u64 precision, u64* time);

INLINE
u64 time(u64* p) {
	u64 ns;
	__wasi_clock_time_get(WASI_CLOCKID_REALTIME, 1000000000, &ns);
	u64 s = ns / 1000000000;
	if (p)
		*p = s;
	return s;
}


NORETURN
WASM_IMPORT("wasi_snapshot_preview1", "proc_exit")
void __wasi_proc_exit(u32 code);


#define WASI_EVENTTYPE_CLOCK 0

typedef struct wasi_subscription {
	u64 usr;
	u16 type;

	struct {
		u32 id;
		u64 timeout;
		u64 precision;
		u16 flags;
	} clock;
} wasi_subscription_t;

typedef struct wasi_event {
	u64 usr;
	i32 error;
	u16 type;
	struct {
		u64 timestamp;
	} clock;
} wasi_event_t;

WASM_IMPORT("wasi_snapshot_preview1", "poll_oneoff")
int __wasi_poll_oneoff(const wasi_subscription_t* sub, wasi_event_t* out_ev, usz sub_count, usz* out_ev_count);

