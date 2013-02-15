#ifndef __ATOMIC_H__
#define __ATOMIC_H__


#include "types.h"


//
// If SMP machine, atomic operation needs bus-locking instruction.
//
#define LOCK_PREFIX "lock; "



/* The 64-bit atomic type */

#define ATOMIC_INIT(i)	{ (i) }

/**
 * atomic_read - read atomic variable
 * @v: pointer of type atomic_t
 *
 * Atomically reads the value of @v.
 * Doesn't imply a read memory barrier.
 */
#define atomic_read(v)		((v)->counter)

/**
 * atomic_set - set atomic variable
 * @v: pointer to type atomic_t
 * @i: required value
 *
 * Atomically sets the value of @v to @i.
 */
#define atomic_set(v, i)		(((v)->counter) = (i))

/**
 * atomic_add - add integer to atomic variable
 * @i: integer value to add
 * @v: pointer to type atomic_t
 *
 * Atomically adds @i to @v.
 */
static inline void atomic_add(long i, atomic_t *v)
{
	asm volatile(LOCK_PREFIX "addq %1,%0"
		     : "=m" (v->counter)
		     : "er" (i), "m" (v->counter));
}

/**
 * atomic_sub - subtract the atomic variable
 * @i: integer value to subtract
 * @v: pointer to type atomic_t
 *
 * Atomically subtracts @i from @v.
 */
static inline void atomic_sub(long i, atomic_t *v)
{
	asm volatile(LOCK_PREFIX "subq %1,%0"
		     : "=m" (v->counter)
		     : "er" (i), "m" (v->counter));
}

/**
 * atomic_sub_and_test - subtract value from variable and test result
 * @i: integer value to subtract
 * @v: pointer to type atomic_t
 *
 * Atomically subtracts @i from @v and returns
 * true if the result is zero, or false for all
 * other cases.
 */
static inline int atomic_sub_and_test(long i, atomic_t *v)
{
	unsigned char c;

	asm volatile(LOCK_PREFIX "subq %2,%0; sete %1"
		     : "=m" (v->counter), "=qm" (c)
		     : "er" (i), "m" (v->counter) : "memory");
	return c;
}

/**
 * atomic_inc - increment atomic variable
 * @v: pointer to type atomic_t
 *
 * Atomically increments @v by 1.
 */
static inline void atomic_inc(atomic_t *v)
{
	asm volatile(LOCK_PREFIX "incq %0"
		     : "=m" (v->counter)
		     : "m" (v->counter));
}

/**
 * atomic_dec - decrement atomic variable
 * @v: pointer to type atomic_t
 *
 * Atomically decrements @v by 1.
 */
static inline void atomic_dec(atomic_t *v)
{
	asm volatile(LOCK_PREFIX "decq %0"
		     : "=m" (v->counter)
		     : "m" (v->counter));
}


/**
 * atomic_dec_and_test - decrement and test
 * @v: pointer to type atomic_t
 *
 * Atomically decrements @v by 1 and
 * returns true if the result is 0, or false for all other
 * cases.
 */
static inline int atomic_dec_and_test(atomic_t *v)
{
	unsigned char c;

	asm volatile(LOCK_PREFIX "decq %0; sete %1"
		     : "=m" (v->counter), "=qm" (c)
		     : "m" (v->counter) : "memory");
	return c != 0;
}

/**
 * atomic_inc_and_test - increment and test
 * @v: pointer to type atomic_t
 *
 * Atomically increments @v by 1
 * and returns true if the result is zero, or false for all
 * other cases.
 */
static inline int atomic_inc_and_test(atomic_t *v)
{
	unsigned char c;

	asm volatile(LOCK_PREFIX "incq %0; sete %1"
		     : "=m" (v->counter), "=qm" (c)
		     : "m" (v->counter) : "memory");
	return c != 0;
}


/**
 * atomic_add_return - add and return
 * @i: integer value to add
 * @v: pointer to type atomic_t
 *
 * Atomically adds @i to @v and returns @i + @v
 */
static inline long atomic_add_return(long i, atomic_t *v)
{
	long __i = i;
	asm volatile(LOCK_PREFIX "xaddq %0, %1;"
		     : "+r" (i), "+m" (v->counter)
		     : : "memory");
	return i + __i;
}

/** 
 * substract and return result
 * 
 * @param i 
 * @param v 
 * 
 * @return 
 */
static inline long atomic_sub_return(long i, atomic_t *v)
{
	return atomic_add_return(-i, v);
}

/** 
 * compare and swap
 * 
 * @param ptr data to swap
 * @param old comparation value
 * @param new new value
 * 
 * @return old value
 */
static inline unsigned long atomic_cmpxchg(volatile void *ptr,
                                           unsigned long old,
                                           unsigned long new)
{
	unsigned long prev;

    asm volatile(LOCK_PREFIX "cmpxchgq %1,%2"
			     : "=a"(prev)
			     : "r"(new), "m"(*(volatile long *)ptr), "0"(old)
			     : "memory");
    return prev;

}


#endif
