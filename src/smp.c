#include <apic.h>
#include <smp.h>
#include <mp_spec.h>
#include <atomic.h>
#include <percpu.h>


ssize_t preempt_count;



//
//  clock estimation will be implemented later..
//  This delay does just delay-loop
//
static void delay(size_t count)
{
	volatile s64 a = -1;

	for (; count > 0; count--) {
		for (; a > 0; a--)
			continue;
	}


}




//
// send Startup IPIs to AP
// refer to INTEL PROCESSOR MANUAL vol.2 & MP specification
//
void wake_ap(void)
{

	
	//
	// Interrupt Command Register
	// send interprocessor interrupts(IPIs) to other processors.
	//
	// [63:56]: destination
	// [19:18]: destination shorthand
	// [15]   : trigger mode, 0:edge, 1:level
	// [14]   : level, 0:de-assert, 1:assert
	// [12]   : delivery status (RO), 0:idle, 1:send pending
	// [11]   : destination mode, 0:physical, 1:logical
	// [10:8] : delivery mode, 101:INIT, 110:STARTUP
	// [7:0]  : vector, address of a 4KByte page in the real-address
	

	// destination shorthand = 11:all excluding self 
	// (send to all processors except sending-processor
	// trigger mode = 0:edge
	// level = 1:assert
	// destination mode = 0:physical
	// delivery mode = 101:INIT
	apic_write(APIC_ICR, APIC_DEST_ALLBUT|APIC_INT_ASSERT|APIC_DM_INIT);


	// 10ms delay
	delay(1000000);
	delay(1000000);

	// delivery mode = 111:STARTUP
	// Because base address of AP initialization code is 0x9000 (setup.asm)
	// vector is 0x9
	apic_write(APIC_ICR, APIC_DEST_ALLBUT|APIC_INT_ASSERT|APIC_DM_STARTUP|(AP_INIT_ADDRESS>>12));

	// 100us delay
	delay(1000000);


	// delivery mode = 111:STARTUP
	apic_write(APIC_ICR, APIC_DEST_ALLBUT|APIC_INT_ASSERT|APIC_DM_STARTUP|(AP_INIT_ADDRESS>>12));

	// 100us delay
	delay(1000000);


	// signal sent?
	if ((apic_read(APIC_ICR) & APIC_ICR_BUSY) != 0) {
		caos_printf("AP-WAKENING FAIL!\n");
		halt();
	}


	return;
}


void smp_init(void)
{

	preempt_count() = 0;

	caos_printf("preempt-count=> %d\n", preempt_count());
	
	
	// per-cpu data setup
	setup_per_cpu_areas();

}


