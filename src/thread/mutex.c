#include <lt/thread.h>
#include <lt/mem.h>
#include <lt/lt.h>

#if defined(LT_UNIX)
#	include <pthread.h>

typedef
struct lt_mutex {
	pthread_mutex_t pmut;
} lt_mutex_t;

lt_mutex_t* lt_mutex_create(lt_alloc_t alloc[static 1]) {
	pthread_mutex_t pmut = (pthread_mutex_t)PTHREAD_MUTEX_INITIALIZER;
	if (pthread_mutex_init(&pmut, NULL)) {
		return NULL;
	}
	lt_mutex_t* m = lt_malloc(alloc, sizeof(lt_mutex_t));
	if (!m) {
		pthread_mutex_destroy(&pmut);
		return NULL;
	}
	m->pmut = pmut;
	return m;
}

void lt_mutex_destroy(lt_mutex_t* m, lt_alloc_t alloc[static 1]) {
	pthread_mutex_destroy(&m->pmut);
	lt_mfree(alloc, m);
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

lt_mutex_t* lt_mutex_create(lt_alloc_t alloc[static 1]) {
	HANDLE wmut = CreateMutex(NULL, FALSE, NULL);
	if (!wmut) {
		return NULL;
	}
	lt_mutex_t* m = lt_malloc(alloc, sizeof(lt_mutex_t));
	if (!m) {
		CloseHandle(wmut);
		return NULL;
	}
	m->wmut = wmut;
	return m;
}

void lt_mutex_destroy(lt_mutex_t* m, lt_alloc_t alloc[static 1]) {
	CloseHandle(m->wmut);
	lt_mfree(alloc, m);
}

void lt_mutex_lock(lt_mutex_t* m) {
	WaitForSingleObject(m->wmut, INFINITE);
}

void lt_mutex_release(lt_mutex_t* m) {
	ReleaseMutex(m->wmut);
}

#endif

