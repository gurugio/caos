
#ifndef __SMP_H__
#define __SMP_H__


#include <apic.h>



//
// macros
//

#define smp_mb() asm volatile("mfence":::"memory")
#define smp_rmb() asm volatile("lfence":::"memory")
#define smp_wmb() asm volatile("sfence":::"memory")


#define preempt_count() (preempt_count)




//
// interrupt-disable is necessary for preempt-disable??
//

#define preempt_disable() \
	do { \
		preempt_count() += 1;\
		smp_wmb();\
	} while (0)

#define preempt_enable() \
	do { \
		preempt_count() -= 1;\
		smp_wmb();\
	} while (0)




//
// The address that AP start executin at
//
#define AP_INIT_ADDRESS 0x9000


//
// get CPU's ID
//
static inline size_t cpu_id(void)
{
	return apic_read(APIC_ID);
}



extern ssize_t preempt_count;

void smp_init(void);
void wake_ap(void);


#endif


