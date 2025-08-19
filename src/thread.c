#include <lt2/thread.h>

#include <pthread.h>
#include <sched.h>

#define _XOPEN_SOURCE 500
#include <signal.h>

static
void* thread_start_wrapper(void* args) {
	thread_handle* thread = args;
	thread->proc(thread->args);
	return NULL;
}

b8 thread_spawn(thread_handle thread[static 1], thread_fn proc, void* args, err* error) {
	thread->proc = proc;
	thread->args = args;
	if (pthread_create((pthread_t*)&thread->pthread, NULL, thread_start_wrapper, thread)) {
		throw(error, ERR_ANY, "pthread_create() failed");
		return 0;
	}
	return 1;
}

b8 thread_join(const thread_handle thread[static 1], err* error) {
	if (pthread_join(thread->pthread, NULL) != 0) {
		throw(error, ERR_ANY, "pthread_join() failed");
		return 0;
	}
	return 1;
}

b8 thread_terminate(const thread_handle thread[static 1], err* error) {
	if (pthread_kill(thread->pthread, SIGTERM)) {
		throw(error, ERR_ANY, "pthread_kill() failed");
		return 0;
	}
	return 1;
}

b8 thread_kill(const thread_handle thread[static 1], err* error) {
	if (pthread_kill(thread->pthread, SIGKILL)) {
		throw(error, ERR_ANY, "pthread_kill() failed");
		return 0;
	}
	return 1;
}

b8 thread_stop(const thread_handle thread[static 1], err* error) {
	if (pthread_kill(thread->pthread, SIGSTOP)) {
		throw(error, ERR_ANY, "pthread_kill() failed");
		return 0;
	}
	return 1;
}

b8 thread_continue(const thread_handle thread[static 1], err* error) {
	if (pthread_kill(thread->pthread, SIGCONT)) {
		throw(error, ERR_ANY, "pthread_kill() failed");
		return 0;
	}
	return 1;
}

b8 thread_cancel(const thread_handle thread[static 1], err* error) {
	if (pthread_cancel(thread->pthread)) {
		throw(error, ERR_ANY, "pthread_cancel() failed");
		return 0;
	}
	return 1;
}

b8 thread_yield(err* error) {
	if (sched_yield()) {
		throw(error, ERR_ANY, "sched_yield() failed");
		return 0;
	}
	return 1;
}

