#include <lt/lt.h>
#include <lt/thread.h>
#include <lt/mem.h>

#if defined(LT_UNIX)
#	include <pthread.h>

typedef
struct lt_thread {
	pthread_t pthread;
	lt_thread_proc_t proc;
	void* args;
} lt_thread_t;

static
void* lt_thread_start_wrapper(void* args) {
	lt_thread_t* thread = args;
	thread->proc(thread->args);
	return NULL;
}

lt_thread_t* lt_thread_create(lt_arena_t* arena, lt_thread_proc_t proc, void* args) {
	lt_thread_t* thread = lt_arena_reserve(arena, sizeof(lt_thread_t));
	thread->proc = proc;
	thread->args = args;
	return pthread_create(&thread->pthread, NULL, lt_thread_start_wrapper, thread) == 0 ? thread : NULL;
}

b8 lt_thread_join(lt_thread_t* thread) {
	return pthread_join(thread->pthread, NULL) == 0;
}

#elif defined(LT_WINDOWS)
#	define WIN32_LEAN_AND_MEAN 1
#	include <windows.h>

typedef
struct lt_thread {
	HANDLE hnd;
	lt_thread_proc_t proc;
	void* args;
} lt_thread_t;

static
DWORD WINAPI lt_thread_start_wrapper(void* args) {
	lt_thread_t* thread = args;
	thread->proc(thread->args);
	return 0;
}

lt_thread_t* lt_thread_create(lt_arena_t* arena, lt_thread_proc_t proc, void* args) {
	lt_thread_t* thread = lt_arena_reserve(arena, sizeof(lt_thread_t));
	thread->proc = proc;
	thread->args = args;
	thread->hnd = CreateThread(NULL, 0, lt_thread_start_wrapper, thread, 0, NULL);
	return thread;
}

b8 lt_thread_join(lt_thread_t* thread) {
	if (WaitForSingleObject(thread->hnd, INFINITE) != WAIT_OBJECT_0)
		return 0;
	CloseHandle(thread->hnd);
	return 1;
}

#endif
