#ifndef LT_THREAD_H
#define LT_THREAD_H 1

#include <lt/primitives.h>
#include <lt/fwd.h>

// thread.c
typedef struct lt_thread lt_thread_t;

typedef void(*lt_thread_proc_t)(void*);

lt_thread_t* lt_thread_create(lt_thread_proc_t proc, void* args, lt_alloc_t* alloc);
b8 lt_thread_join(lt_thread_t* thread, lt_alloc_t* alloc);

// spinlock.c
typedef
struct lt_spinlock {
	volatile b8 locked;
} lt_spinlock_t;

#define LT_SPINLOCK_INIT { 0 }

void lt_spinlock_lock(lt_spinlock_t* slock);
void lt_spinlock_release(lt_spinlock_t* slock);

// mutex.c
typedef struct lt_mutex lt_mutex_t;

lt_mutex_t* lt_mutex_create(lt_alloc_t* alloc);
void lt_mutex_destroy(lt_mutex_t* m, lt_alloc_t* alloc);

void lt_mutex_lock(lt_mutex_t* m);
void lt_mutex_release(lt_mutex_t* m);

#endif
