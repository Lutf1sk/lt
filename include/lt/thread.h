#ifndef LT_THREAD_H
#define LT_THREAD_H 1

#include <lt/fwd.h>
#include <lt/err.h>

// thread.c
typedef struct lt_thread lt_thread_t;

typedef void(*lt_thread_fn_t)(void*);

lt_thread_t* lt_thread_create(lt_thread_fn_t fn, void* args, lt_alloc_t alloc[static 1]);
b8 lt_thread_join(const lt_thread_t* thread, lt_alloc_t alloc[static 1]);

lt_err_t lt_thread_terminate(const lt_thread_t* thread);
lt_err_t lt_thread_kill(const lt_thread_t* thread);
lt_err_t lt_thread_stop(const lt_thread_t* thread);
lt_err_t lt_thread_continue(const lt_thread_t* thread);

lt_err_t lt_thread_cancel(const lt_thread_t* thread);

lt_err_t lt_thread_yield(void);

// spinlock.c
typedef
struct lt_spinlock {
	volatile b8 locked;
} lt_spinlock_t;

#define LT_SPINLOCK_INIT { 0 }

void lt_spinlock_lock(lt_spinlock_t slock[static 1]);
void lt_spinlock_release(lt_spinlock_t slock[static 1]);

// mutex.c

lt_mutex_t* lt_mutex_create(lt_alloc_t alloc[static 1]);
void lt_mutex_destroy(lt_mutex_t* m, lt_alloc_t alloc[static 1]);

void lt_mutex_lock(lt_mutex_t* m);
void lt_mutex_release(lt_mutex_t* m);

#endif
