#pragma once

#include <lt2/common.h>

typedef void(*thread_fn)(void*);

typedef struct thread_handle {
	void* pthread;
	thread_fn proc;
	void* args;
} thread_handle;

b8 thread_spawn(thread_handle thread[static 1], thread_fn fn, void* args, err* error);
b8 thread_join(const thread_handle thread[static 1], err* error);

b8 thread_terminate(const thread_handle thread[static 1], err* error);
b8 thread_kill(const thread_handle thread[static 1], err* error);
b8 thread_stop(const thread_handle thread[static 1], err* error);
b8 thread_continue(const thread_handle thread[static 1], err* error);

b8 thread_cancel(const thread_handle thread[static 1], err* error);

b8 thread_yield(err* error);


