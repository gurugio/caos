
// 
// all functions are copy of linux/arch/x86_64/kernel/apic.c
// refer to include/asm-x86_64/apic.h, apicdef.h
//
//

#include <processor.h>
#include <apic.h>

#include <pgtable.h>
#include <irq_handler.h>

#include <screen.h> // screen_info
#include <irq_vector.h>
#include <smp.h>	// cpu_id()

#include <mp_spec.h> // boot_cpu_id, mp_lapic_addr

#include <io.h>

#define DEBUG
#undef DEBUG



// each cpu's Local APIC ID
u8 x86_cpu_to_apicid[NR_CPUS] = { [0 ... NR_CPUS-1] = 0xff };



//
// This processor's Local APIC is working correctly?
// This is copied from the Linux kernel, 2.6.25
//
ssize_t verify_local_APIC(void)
{
	u32 reg0, reg1;

	//
	// APIC version register
	// [23:16]: # of LVT(Local Vector Table) entries - 1
	// [7:0]  : Version, 1Xh-Local APIC version
	//
	reg0 = apic_read(APIC_LVR);

#ifdef DEBUG
	caos_printf("local apic version=%x\n", reg0);
#endif


	apic_write(APIC_LVR, reg0^APIC_LVR_MASK);
	reg1 = apic_read(APIC_LVR);

#ifdef DEBUG
	caos_printf("local apic version=%x\n", reg1);
#endif
	
	/*
	 * The two version reads above should print the same
	 * numbers.  If the second one is different, then we
	 * poke at a non-APIC.
	 */
	if (reg1 != reg0)
		return 0;

	/*
	 * Check if the version looks reasonably.
	 */
	reg1 = GET_APIC_VERSION(reg0);
	if (reg1 == 0x00 || reg1 == 0xff)
		return 0;
	reg1 = GET_APIC_MAXLVT(reg0);
	if (reg1 < 0x02 || reg1 == 0xff)
		return 0;

	/*
	 * The ID register is read/write in a real APIC.
	 */
	reg0 = apic_read(APIC_ID);
	apic_write(APIC_ID, reg0 ^ APIC_ID_MASK);

	reg1 = apic_read(APIC_ID);
#ifdef DEBUG
	caos_printf("Getting old ID: %x\n", reg0);
	caos_printf("Getting test ID: %x\n", reg1);
#endif
	apic_write(APIC_ID, reg0);
	if (reg1 != (reg0 ^ APIC_ID_MASK))
		return 0;


	return 1;
}




// 
// Interrupt Handler of BSP's Local APIC Timer
//
void bsp_local_timer(size_t num)
{

	static u8 tick=0;
	ssize_t i;

	// clear the first line of screen
	for (i=79; i>=0; i--)
		set_screen(i, ' ');


	// print message that indicates timer running
	set_screen(0, 'B');
	set_screen(1, 'S');
	set_screen(2, 'P');
	set_screen(3, tick++);
	

	set_screen(77, 'A');
	set_screen(78, 'P');


    num = num; /* remove compiler warning */
#ifdef DEBUG
	// ISR bit corresponding to the Timer INT has been setting
	caos_printf("ISR SETTING [%x]", apic_read(APIC_ISR+0x10));
#endif
}




//
// REFINE!!! APIC timer ISR must be refined!!!
//
// refer to arch/x86_64/kernel_apic.c , arch/x86_64/kernel/entry.S
// Entry point of BSP-timer IRQ -> apic_timer_interrupt in entry.S
// C language Handler           -> smp_apic_timer_interrupt in kernel_apic.c
//
void init_bsp_timer(void)
{
	u32 lvt_timer;


	caos_printf("Init local timer of CPU #%d ", cpu_id());

	//
	// Timer interrupt period is hard-coding value.
	// Dynamic analysis of processor's bus clock is need.
	//

	// Initial Count register
	apic_write(APIC_TMICT, 0xFFFFFF);
	// Divide Configuration register
	apic_write(APIC_TDCR, APIC_TDR_DIV_64);


	//
	// enable timer interrupt
	//
	lvt_timer = apic_read(APIC_LVTT);
	lvt_timer &= (~APIC_LVT_MASKED);		// unmask
	lvt_timer |= APIC_LVT_TIMER_PERIODIC;	// periodically repeat
	lvt_timer |= IRQ_TO_VECTOR(BSP_TIMER_IRQ);			// vector
	apic_write(APIC_LVTT, lvt_timer);

#ifdef DEBUG
	caos_printf("APIC_LVTT=%x\n", lvt_timer);
#endif
	
	if ((apic_read(APIC_LVTT) & APIC_LVT_MASKED) == 0)	{
		caos_printf("..success\n");
		// timer irq : local APIC timer do not need PIC-enabling
		// only register handler.
		register_irq(BSP_TIMER_IRQ, bsp_local_timer);
	} else {
		caos_printf("..fail\n");
	}
}




//
// setup BSP's Local APIC, refer linux-kernel 
// arch/x86_64/kernel/apic.c - init_bsp_APIC(), setup_local_APIC()
//
void init_bsp_APIC(void)
{

	u32 lapic_SVR;
	u32 lapic_id;


	//
	// check BSP's Local APIC exists
	//

	// print BSP's Local APIC ID (which is equal to CPU ID)
	lapic_id = apic_read(APIC_ID);

    if (lapic_id != boot_cpu_id || lapic_id != x86_cpu_to_apicid[boot_cpu_id]) {
        /* If AP should call this function, remove following codes and just exit function */
        caos_printf("This is not BSP! Please reboot system.\n");
        halt();
    }

    
	// check Local APIC version
	if (!verify_local_APIC()) {
		caos_printf("\nCannot verify BSP's Local APIC!!\n");
		halt();
	}


	// mask all interrupts ([16] = 1)
	apic_write(APIC_LVTT, 0x10000);		// local timer
	apic_write(APIC_LVTTHMR, 0x10000);	// Thermal monitor 
	apic_write(APIC_LVTPC, 0x10000);	// Performance monitor
	apic_write(APIC_LVT1, 0x10000);		// Local INT #1
	apic_write(APIC_LVT0, 0x10000);		// Local INT #0	
	apic_write(APIC_LVTERR, 0x10000);	// Error



	//
	// Vector 0~31 is invalid because they are reserved for 
	// processor exceptions and Intel's own purpose.
	//


	//
	// interrupt priority = vector / 16
	// 1 being the lowest priority and 15 is the highest
	//


	// task priority register
	// The task priority allows software to set a priority threshold 
	// for interrupt the processor
	// [7:4]: Task priority
	// [3:0]: Task priority sub-class
	apic_write(APIC_TASKPRI, 0); // BSP accepts all INTs & exceptions



	//
	// Delivery mode ExtINT: 
	// Causes the processor to respond to the interrupts
	// as if the interrupt originated in an externally connected
	// interrupt controller. The external controller is
	// expected to supply the vector information.
	// The APIC architecture supports only one ExtINT source in a system,
	// usually contained in the compatibility bridge.
	//


	//
	// I do not know how local interrupts work
	//

#if 0
    do
    {
        u32 reg;
        reg = apic_read(APIC_LVT0);
        reg &= ~APIC_LVT_MASKED;		// enable
        reg |= APIC_LVT_LEVEL_TRIGGER;	// only level trigger mode for ExtINT
        reg |= APIC_INPUT_POLARITY;		// active low
        reg |= APIC_DM_FIXED;;			// delivery mode ExtINT
        reg |= IRQ_TO_VECTOR(LINT0_IRQ);			// vector of LINT0
        apic_write(APIC_LVT0, reg);
    } while (0);
#endif


	// I do not know what spurious interrupt
	// spurious interrupt vector
	apic_write(APIC_SPIV, apic_read(APIC_SPIV) | IRQ_TO_VECTOR(SPURIOUS_IRQ));


#ifdef DEBUG
	caos_printf("SPIV=%x", apic_read(APIC_SPIV));
	caos_printf("LINT0=%x\n", apic_read(0x350));
	caos_printf("LINT1=%x", apic_read(0x360));
	caos_printf("error=%x\n", apic_read(0x370));
	caos_printf("perfor=%x", apic_read(0x340));
	caos_printf("therm=%x\n", apic_read(0x330));
#endif
	
	apic_write(APIC_EOI, 0);	// clear ISR reg


	//
	// activate BSP's local APIC
	//
	lapic_SVR = apic_read(APIC_SPIV);	// Spurious Interrupt Vector reg
	lapic_SVR |= APIC_SPIV_APIC_ENABLED; // set enable bit explicitly
	apic_write(APIC_SPIV, lapic_SVR);

	lapic_SVR = apic_read(APIC_SPIV);
	if (lapic_SVR & APIC_SPIV_APIC_ENABLED)
		caos_printf("enabled\n");

    apic_write(APIC_EOI, 0);	// clear ISR reg
    
	return;
}


