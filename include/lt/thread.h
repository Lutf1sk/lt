#ifndef LT_THREAD_H
#define LT_THREAD_H 1

#include <lt/primitives.h>
#include <lt/fwd.h>

// thread.c
typedef struct lt_thread lt_thread_t;

typedef void(*lt_thread_proc_t)(void*);

lt_thread_t* lt_thread_create(lt_arena_t* arena, lt_thread_proc_t proc, void* args);
b8 lt_thread_join(lt_thread_t* thread);

// spinlock.c
typedef
struct lt_spinlock {
	volatile b8 locked;
} lt_spinlock_t;

void lt_spinlock_lock(lt_spinlock_t* slock);
void lt_spinlock_release(lt_spinlock_t* slock);

// mutex.c
typedef struct lt_mutex lt_mutex_t;

lt_mutex_t* lt_mutex_create(lt_arena_t* arena);
void lt_mutex_destroy(lt_mutex_t* mut);

void lt_mutex_lock(lt_mutex_t* mut);
void lt_mutex_release(lt_mutex_t* mut);

#endif
