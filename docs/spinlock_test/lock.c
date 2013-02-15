#include <stdio.h>

#include <unistd.h>
#include <pthread.h>
#include <stdlib.h>



//
// type definitions
//


typedef signed int ssize_t;

typedef struct {
	volatile ssize_t slock;
} spinlock_t;

#define SPIN_LOCK_UNLOCKED 1
#define SPIN_LOCK_LOCKED 0



volatile ssize_t preempt_count = 0;

//
// macros
//

#define smp_mb() asm volatile("mfence":::"memory")
#define smp_rmb() asm volatile("lfence":::"memory")
#define smp_wmb() asm volatile("sfence":::"memory")

#define cpu_relax() asm volatile("rep;nop":::"memory")



#define spin_lock_init(lock) \
	do { lock->slock = SPIN_LOCK_UNLOCKED; } while (0)


/**
 * test_bit - Determine whether a bit is set
 * @nr: bit number to test
 * @addr: Address to start counting from
 */

static __inline__ size_t test_bit(size_t nr, const volatile void * addr)
{
	
	size_t oldbit;


	__asm__ __volatile__(
			"btl %2,%1\n\t"
			"sbbl %0,%0\n\t"
			:"=r" (oldbit)
			:"m" (*(volatile long *)addr), "dlr" (nr));
	return oldbit;
}


static __inline__ void set_bit(size_t nr, volatile void * addr)
{
	__asm__ __volatile__(
		"btsl %1,%0"
		:"=m"(*(volatile long *)addr)
		:"dIr" (nr) : "memory");
}



static __inline__ void atomic_inc(int *v)
{

	asm volatile (
			"incl %0"
			:"=m"(*v)
			:"m"(*v));
}


static __inline__ void atomic_dec(int *v)
{

	asm volatile (
			"decl %0"
			:"=m"(*v)
			:"m"(*v));
}

#define preempt_disable() \
	do { \
		preempt_count += 1;\
		smp_mb();\
	} while (0)

#define preempt_enable() \
	do { \
		preempt_count -= 1;\
		smp_mb();\
	} while (0)



#define spin_is_locked(lock) test_bit(0, &lock->slock)

//
// If spin-lock is free, it locks spin-lock and returns 1.
// If spin-lock is locked, it returns 0.
//
size_t raw_spin_trylock(spinlock_t *lock)
{
	size_t tmp = 0;

	//printf("tmp=%d lock=%d\n", tmp, lock->slock);

	asm volatile (
		"movb $0, %b0\n\t"
		"lock;xchgb %b0, %1\n\t"
		: "=&a" (tmp), "+m" (lock->slock)
		:
		: "memory");

	//printf("tmp=%d lock=%d\n", tmp, lock->slock);
		
	return tmp;
}





void spin_lock(spinlock_t *lock)
{

	preempt_disable();

	while (!raw_spin_trylock(lock)) {
		preempt_enable();
		while (spin_is_locked(lock)) {
			cpu_relax();
			printf("RELAX ");
		}
		preempt_disable();
	}


}


void spin_unlock(spinlock_t *lock)
{
	set_bit(0, &lock->slock);
	preempt_enable();
}



spinlock_t l = {SPIN_LOCK_UNLOCKED};



void thread_func(void *tid)
{
	srand((unsigned int)time(NULL));


	while (1) {
		spin_lock(&l);


		//
		// CRITICAL SECTION
		//
		printf("LOCK by   <%d>\n", *(int *)tid);

		usleep(100 * (rand() % 10));

		if (test_bit(0, &l.slock)) {
			printf("ERROR\n");
			return;
		}

		printf("======>PREEMPT=%d\n", preempt_count);

		printf("UNLOCK by <%d>\n", *(int *)tid);
		fflush(stdout);
		//
		// CRITICAL SECTION
		//


		spin_unlock(&l);
		
		
		//
		// NORMAL SECTION
		//
		usleep(1000 * (rand() % 10));

	}



}



void preempt_monitor(void)
{
	int old = 0;

	while (1) {
		if (preempt_count != old) {
			printf("[MONITOR]PREEMPT=%d\n", preempt_count);
			old = preempt_count;
		}
		//if (preempt_count == 0) printf("SCHEDULE");
	}
		
}


int main(void)
{

	int a=0,b=1,c=2;
	pthread_t thr[4];
	int thr_id[4];

	thr_id[0] = pthread_create(&thr[0], NULL, thread_func, &a);
	thr_id[1] = pthread_create(&thr[1], NULL, thread_func, &b);
	thr_id[2] = pthread_create(&thr[2], NULL, thread_func, &c);
	thr_id[3] = pthread_create(&thr[3], NULL, preempt_monitor, NULL);


	pthread_join(thr[0], NULL);
	pthread_join(thr[1], NULL);
	pthread_join(thr[2], NULL);
	pthread_join(thr[3], NULL);


	return 0;
}



