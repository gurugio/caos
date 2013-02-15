
//
// copy of linux/include/asm-x86_64/bitops.h
//
#ifndef __BITOPS_H__
#define __BITOPS_H__

#include <types.h>



//
// Every bit-operation must consider SMP and use LOCK primitive.
// Bit-operations for UP are removed. - 2009.4.22
//


//
// If SMP machine, atomic operation needs bus-locking instruction.
//
#define LOCK_PREFIX \
	"lock; "




#define ADDR(x) "+m" (*(volatile long *) (x))

/**
 * set_bit - Atomically set a bit in memory
 * @nr: the bit to set
 * @addr: the address to start counting from
 *
 * This function is atomic and may not be reordered.  See __set_bit()
 * if you do not require the atomic guarantees.
 * Note that @nr may be almost arbitrarily large; this function is not
 * restricted to acting on a single-word quantity.
 */
static __inline__ void set_bit(size_t nr, volatile void * addr)
{
	__asm__ __volatile__(
		LOCK_PREFIX
		"btsq %1,%0"
		:ADDR(addr)
		:"dIr" (nr) : "memory");
}


/**
 * clear_bit - Clears a bit in memory
 * @nr: Bit to clear
 * @addr: Address to start counting from
 *
 * clear_bit() is atomic and may not be reordered.  However, it does
 * not contain a memory barrier, so if it is used for locking purposes,
 * you should call smp_mb__before_clear_bit() and/or smp_mb__after_clear_bit()
 * in order to ensure changes are visible on other processors.
 */
static __inline__ void clear_bit(ssize_t nr, volatile void * addr)
{
	__asm__ __volatile__(
		LOCK_PREFIX
		"btrq %1,%0"
		:ADDR(addr)
		:"dIr" (nr));
}



/**
 * change_bit - Toggle a bit in memory
 * @nr: Bit to change
 * @addr: Address to start counting from
 *
 * change_bit() is atomic and may not be reordered.
 * Note that @nr may be almost arbitrarily large; this function is not
 * restricted to acting on a single-word quantity.
 */
static __inline__ void change_bit(ssize_t nr, volatile void * addr)
{
	__asm__ __volatile__(
		LOCK_PREFIX
		"btcq %1,%0"
		:ADDR(addr)
		:"dIr" (nr));
}

/**
 * test_and_set_bit - Set a bit and return its old value
 * @nr: Bit to set
 * @addr: Address to count from
 *
 * This operation is atomic and cannot be reordered.  
 * It also implies a memory barrier.
 */
static __inline__ ssize_t test_and_set_bit(ssize_t nr, volatile void * addr)
{
	ssize_t oldbit;

	__asm__ __volatile__( 
		LOCK_PREFIX
		"btsq %2,%1\n\t"
		"sbbl %0,%0\n\t"
		:"=r" (oldbit),ADDR(addr)
		:"dIr" (nr) : "memory");
	return oldbit;
}



/**
 * test_and_clear_bit - Clear a bit and return its old value
 * @nr: Bit to clear
 * @addr: Address to count from
 *
 * This operation is atomic and cannot be reordered.  
 * It also implies a memory barrier.
 */
static __inline__ ssize_t test_and_clear_bit(ssize_t nr, volatile void * addr)
{
	ssize_t oldbit;

	__asm__ __volatile__( 
		LOCK_PREFIX
		"btrq %2,%1\n\t"
		"sbbl %0,%0\n\t"
		:"=r" (oldbit),ADDR(addr)
		:"dIr" (nr) : "memory");
	return oldbit;
}



/**
 * test_and_change_bit - Change a bit and return its old value
 * @nr: Bit to change
 * @addr: Address to count from
 *
 * This operation is atomic and cannot be reordered.  
 * It also implies a memory barrier.
 */
static __inline__ ssize_t test_and_change_bit(ssize_t nr, volatile void * addr)
{
	ssize_t oldbit;

	__asm__ __volatile__(
		LOCK_PREFIX
		"btcq %2,%1\n\t"
		"sbbl %0,%0\n\t"
		:"=r" (oldbit),ADDR(addr)
		:"dIr" (nr) : "memory");
	return oldbit;
}

/**
 * test_bit - Determine whether a bit is set
 * @nr: bit number to test
 * @addr: Address to start counting from
 */

static __inline__ ssize_t test_bit(ssize_t nr, const volatile void * addr)
{
	
	size_t oldbit;

	__asm__ __volatile__(
		LOCK_PREFIX
		"btq %2,%1\n\t"
		"sbbq %0,%0\n\t"
		:"=r" (oldbit)
		:"m" (*(volatile long *)addr), "dlr" (nr));
	return oldbit;
}



/**
 * ffz - find first zero in word.
 * @word: The word to search
 *
 * Undefined if no zero exists, so code should check against ~0UL first.
 */
static __inline__ size_t ffz(size_t word)
{
	if (word == ((unsigned long)0x0) - 1)
		return (unsigned long)0x0 - 1;

	__asm__ __volatile__(
		"bsfq %1,%0"
		:"=r" (word)
		:"r" (~word));
	return word;
}

/**
 * __ffs - find first bit in word.
 * @word: The word to search
 *
 * Undefined if no bit exists, so code should check against 0 first.
 */
static __inline__ size_t __ffs(size_t word)
{
	if (word == 0)
		return 0;

	__asm__ __volatile__(
		"bsfq %1,%0"
		:"=r" (word)
		:"rm" (word));
	return word;
}


/*
 * __fls: find last bit set.
 * @word: The word to search
 *
 * Undefined if no zero exists, so code should check against ~0UL first.
 */
static __inline__ size_t __fls(size_t word)
{
	__asm__ __volatile__(
		"bsrq %1,%0"
		:"=r" (word)
		:"rm" (word));
	return word;
}

/**
 * fls64 - find last bit set in 64 bit word
 * @x: the word to search
 *
 * This is defined the same way as fls.
 */
static __inline__ size_t fls64(size_t x)
{
	if (x == 0)
		return 0;
	return __fls(x) + 1;
}



#endif
