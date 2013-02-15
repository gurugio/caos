#ifndef __SPINLOCK_H__
#define __SPINLOCK_H__



#include <types.h>



typedef struct {
	ssize_t slock;
} spinlock_t;

#define SPIN_LOCK_UNLOCKED 1
#define SPIN_LOCK_LOCKED 0



#define cpu_relax() asm volatile("rep;nop":::"memory")



#define spin_lock_init(lock) \
	do { lock->slock = SPIN_LOCK_UNLOCKED; } while (0)


#define spin_is_locked(lock) test_bit(0, &lock->slock)

#endif
