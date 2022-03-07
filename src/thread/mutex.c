#include <lt/thread.h>
#include <lt/mem.h>
#include <lt/lt.h>

#if defined(LT_UNIX)
#	include <pthread.h>

typedef
struct lt_mutex {
	pthread_mutex_t pmut;
} lt_mutex_t;

lt_mutex_t* lt_mutex_create(lt_arena_t* arena) {
	lt_mutex_t* m = lt_arena_reserve(arena, sizeof(lt_mutex_t));
	m->pmut = (pthread_mutex_t)PTHREAD_MUTEX_INITIALIZER;
	pthread_mutex_init(&m->pmut, NULL);
	return m;
}

void lt_mutex_destroy(lt_mutex_t* m) {
	pthread_mutex_destroy(&m->pmut);
}

void lt_mutex_lock(lt_mutex_t* m) {
	pthread_mutex_lock(&m->pmut);
}

void lt_mutex_release(lt_mutex_t* m) {
	pthread_mutex_unlock(&m->pmut);
}


#elif defined(LT_WINDOWS)
#	define WIN32_LEAN_AND_MEAN 1
#	include <windows.h>

typedef
struct lt_mutex {
	HANDLE wmut;
} lt_mutex_t;

lt_mutex_t* lt_mutex_create(lt_arena_t* arena) {
	lt_mutex_t* m = lt_arena_reserve(arena, sizeof(lt_mutex_t));
	m->wmut = CreateMutex(NULL, FALSE, NULL);
	if (!m->wmut)
		return NULL;
	return m;
}

void lt_mutex_destroy(lt_mutex_t* m) {
	CloseHandle(m->wmut);
}

void lt_mutex_lock(lt_mutex_t* m) {
	WaitForSingleObject(m->wmut, INFINITE);
}

void lt_mutex_release(lt_mutex_t* m) {
	ReleaseMutex(m->wmut);
}

#endif

