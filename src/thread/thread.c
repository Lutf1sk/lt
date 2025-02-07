#include <lt/lt.h>
#include <lt/thread.h>
#include <lt/mem.h>

#if defined(LT_UNIX)
#	include <pthread.h>
#	include <signal.h>
#	include <sched.h>

typedef
struct lt_thread {
	pthread_t pthread;
	lt_thread_fn_t proc;
	void* args;
} lt_thread_t;

static
void* lt_thread_start_wrapper(void* args) {
	lt_thread_t* thread = args;
	thread->proc(thread->args);
	return NULL;
}

lt_thread_t* lt_thread_create(lt_thread_fn_t proc, void* args, lt_alloc_t alloc[static 1]) {
	lt_thread_t* thread = lt_malloc(alloc, sizeof(lt_thread_t));
	if (!thread)
		return NULL;
	thread->proc = proc;
	thread->args = args;
	if (pthread_create(&thread->pthread, NULL, lt_thread_start_wrapper, thread)) {
		lt_mfree(alloc, thread);
		return NULL;
	}
	return thread;
}

b8 lt_thread_join(const lt_thread_t* thread, lt_alloc_t alloc[static 1]) {
	if (pthread_join(thread->pthread, NULL) != 0)
		return 0;
	lt_mfree(alloc, thread);
	return 1;
}

lt_err_t lt_thread_terminate(const lt_thread_t* thread) {
	if (pthread_kill(thread->pthread, SIGTERM))
		return LT_ERR_UNKNOWN;
	return LT_SUCCESS;
}

lt_err_t lt_thread_kill(const lt_thread_t* thread) {
	if (pthread_kill(thread->pthread, SIGKILL))
		return LT_ERR_UNKNOWN;
	return LT_SUCCESS;
}

lt_err_t lt_thread_stop(const lt_thread_t* thread) {
	if (pthread_kill(thread->pthread, SIGSTOP))
		return LT_ERR_UNKNOWN;
	return LT_SUCCESS;
}

lt_err_t lt_thread_continue(const lt_thread_t* thread) {
	if (pthread_kill(thread->pthread, SIGCONT))
		return LT_ERR_UNKNOWN;
	return LT_SUCCESS;
}

lt_err_t lt_thread_cancel(const lt_thread_t* thread) {
	if (pthread_cancel(thread->pthread))
		return LT_ERR_UNKNOWN;
	return LT_SUCCESS;
}

lt_err_t lt_thread_yield(void) {
	if (sched_yield())
		return LT_ERR_UNKNOWN;
	return LT_SUCCESS;
}

#elif defined(LT_WINDOWS)
#	define WIN32_LEAN_AND_MEAN 1
#	include <windows.h>

typedef
struct lt_thread {
	HANDLE hnd;
	lt_thread_fn_t proc;
	void* args;
} lt_thread_t;

static
DWORD WINAPI lt_thread_start_wrapper(void* args) {
	lt_thread_t* thread = args;
	thread->proc(thread->args);
	return 0;
}

lt_thread_t* lt_thread_create(lt_thread_fn_t proc, void* args, lt_alloc_t alloc[static 1]) {
	lt_thread_t* thread = lt_malloc(alloc, sizeof(lt_thread_t));
	if (!thread) {
		return NULL;
	}
	thread->proc = proc;
	thread->args = args;
	thread->hnd = CreateThread(NULL, 0, lt_thread_start_wrapper, thread, 0, NULL);
	if (!thread->hnd) {
		lt_mfree(alloc, thread);
		return NULL;
	}
	return thread;
}

b8 lt_thread_join(const lt_thread_t* thread, lt_alloc_t alloc[static 1]) {
	if (WaitForSingleObject(thread->hnd, INFINITE) != WAIT_OBJECT_0)
		return 0;
	CloseHandle(thread->hnd);
	lt_mfree(alloc, thread);
	return 1;
}

#endif
