#include <lt/thread.h>
#include <lt/lt.h>

static LT_INLINE
u8 atomic_test_and_set(volatile u8* val) {
	u8 cf;
	__asm__ volatile ("lock bts DWORD PTR [%1], 0\n" : "=@ccc"(cf) : "r"(val));
	return cf;
}

void lt_spinlock_lock(lt_spinlock_t slock[static 1]) {
	while (atomic_test_and_set(&slock->locked))
		;
}

void lt_spinlock_release(lt_spinlock_t slock[static 1]) {
	slock->locked = 0;
}

