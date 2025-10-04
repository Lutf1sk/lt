#include <lt2/thread.h>

#include <pthread.h>
#include <sched.h>
#include <signal.h>

static
void* thread_start_wrapper(void* args) {
	thread_handle* thread = args;
	thread->proc(thread->args);
	return NULL;
}

b8 thread_spawn(thread_handle thread[static 1], thread_fn proc, void* args, err* err) {
	thread->proc = proc;
	thread->args = args;
	int res = pthread_create((pthread_t*)&thread->pthread, NULL, thread_start_wrapper, thread);
	if (res) {
		throw_errno_val(err, res);
		return 0;
	}
	return 1;
}

b8 thread_join(const thread_handle thread[static 1], err* err) {
	int res = pthread_join((pthread_t)thread->pthread, NULL);
	if (res) {
		throw_errno_val(err, res);
		return 0;
	}
	return 1;
}

b8 thread_terminate(const thread_handle thread[static 1], err* err) {
	int res = pthread_kill((pthread_t)thread->pthread, SIGTERM);
	if (res) {
		throw_errno_val(err, res);
		return 0;
	}
	return 1;
}

b8 thread_kill(const thread_handle thread[static 1], err* err) {
	int res = pthread_kill((pthread_t)thread->pthread, SIGKILL);
	if (res) {
		throw_errno_val(err, res);
		return 0;
	}
	return 1;
}

b8 thread_stop(const thread_handle thread[static 1], err* err) {
	int res = pthread_kill((pthread_t)thread->pthread, SIGSTOP);
	if (res) {
		throw_errno_val(err, res);
		return 0;
	}
	return 1;
}

b8 thread_continue(const thread_handle thread[static 1], err* err) {
	int res = pthread_kill((pthread_t)thread->pthread, SIGCONT);
	if (res) {
		throw_errno_val(err, res);
		return 0;
	}
	return 1;
}

b8 thread_cancel(const thread_handle thread[static 1], err* err) {
	int res = pthread_cancel((pthread_t)thread->pthread);
	if (res) {
		throw_errno_val(err, res);
		return 0;
	}
	return 1;
}

b8 thread_yield(err* err) {
	if (sched_yield() < 0) {
		throw_errno(err);
		return 0;
	}
	return 1;
}

