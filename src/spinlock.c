
#include <spinlock.h>
#include <bitops.h>	// set_bit
#include <smp.h>	// smp_mb()


#define DEBUG
#undef DEBUG

//
// If spin-lock is free, it locks spin-lock and returns 1.
// If spin-lock is locked, it returns 0.
//
size_t raw_spin_trylock(spinlock_t *lock)
{
	size_t tmp = 0;


	asm volatile (
		"movb $0, %b0\n\t"
		"lock;xchgb %b0, %1\n\t"
		: "=&a" (tmp), "+m" (lock->slock)
		:
		: "memory");

		
	return tmp;
}





void spin_lock(spinlock_t *lock)
{

	preempt_disable();

#ifdef DEBUG
	caos_printf("PREEMPT=%d\n", preempt_count());
#endif

	while (!raw_spin_trylock(lock)) {
		preempt_enable();
		while (spin_is_locked(lock))
			cpu_relax();
		preempt_disable();
	}


}


void spin_unlock(spinlock_t *lock)
{

	set_bit(0, &lock->slock);
	smp_mb();

	preempt_enable();
#ifdef DEBUG
	caos_printf("PREEMPT=%d\n", preempt_count());
#endif
}





