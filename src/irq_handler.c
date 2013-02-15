


#include <printf.h>
#include <types.h>
#include <apic.h>
#include <irq_handler.h>
#include <irq_entry.h>	// _ignore_isr
#include <idt.h>	// irq_handler[]
#include <pic8259.h> // ack_pic8259
#include <irq_vector.h>
#include <io_apic.h>

#define DEBUG
#undef DEBUG


irq_handler_t irq_handler[MAX_IRQ];




//
// set IDT descriptors (0x20~0x2F) to irq_handler table (0x0~0xF).
// Actual handlers are registered into irq_handler table, not IDT.
//
void irq_init(void)
{
	ssize_t i;

	for (i = 0; i<MAX_IRQ; i++) {
		irq_handler[i] = ignore_isr_handler;
	}


    // DECLARE_IRQ is macro, so number is passed as string.
    // So decimal number is present, not hexa-decimal.

    // initializa table for several IRQs
    // (including CPU-local IRQs and external IRQs)
	DECLARE_IRQ(0);
	DECLARE_IRQ(1);
	DECLARE_IRQ(2);
	DECLARE_IRQ(3);
	DECLARE_IRQ(4);
	DECLARE_IRQ(5);
	DECLARE_IRQ(6);
	DECLARE_IRQ(7);
	DECLARE_IRQ(8);
	DECLARE_IRQ(9);
    DECLARE_IRQ(10);
	DECLARE_IRQ(11);
	DECLARE_IRQ(12);
	DECLARE_IRQ(13);
	DECLARE_IRQ(14);
	DECLARE_IRQ(15);
	DECLARE_IRQ(16);
	DECLARE_IRQ(17);
	DECLARE_IRQ(18);
	DECLARE_IRQ(19);
	DECLARE_IRQ(20);
	DECLARE_IRQ(21);

}


//
// dummy handler for unidentified IRQ
// @num: IRQ number
//
void ignore_isr_handler(size_t num)
{
	caos_printf("Unknown interrupt occur, <irq=%d>\n", num);

}



//
// Register irq handler into irq_handler table.
// @irq: irq number
// @handler: handler function
//
int register_irq(size_t irq, irq_handler_t handler)
{
    
	// external device's irq
	/* if (irq < CPU_IRQ_BASE) */
	/* 	enable_pic8259(irq); */

	irq_handler[irq] = handler;
    
#ifdef DEBUG
	caos_printf("Register irq#%d Handler=%x\n", irq, irq_handler[irq]);
#endif

    return io_apic_enable_irq(irq);
}



//
// ack PIC and enable next IRQ
//
void end_irq(size_t irq)
{

	/* if (irq < CPU_IRQ_BASE) */
	/* 	ack_pic8259(); */
	/* else */

    irq = irq; // ignore irq
    ack_APIC();
}



//
// Every irq-entry function calls handle_irq 
// and then handle_irq call irq handlers.
// IRQs can be request from peripherals and even processors,
// so that republish function like this is needed to treat irq-resources.
// @irq: irq number
//
void handle_irq(size_t irq)
{
	u32 task_pri;


	//
	// prevent lower priority IRQ
	//
	// The task priority register indicates 
	// current processor's priority,
	// so that this has interrupt masked 
	// if new irq has lower priority than current irq
	//
	task_pri = apic_read(APIC_TASKPRI);
	apic_write(APIC_TASKPRI, IRQ_TO_VECTOR(irq));


#ifdef DEBUG
    // ignore IRQ#16 that is BSP local-timer IRQ
    // because it makes screen dirty 
    if (irq != 16)	caos_printf("IRQ[0x%x] occured\n", irq);
#endif

	// call irq's own handler
	if (irq_handler[irq] != NULL)
		irq_handler[irq](irq);


#ifdef DEBUG
    // ignore IRQ#16 that is BSP local-timer IRQ
    // because it makes screen dirty 
	if (irq != 16) caos_printf("end IRQ[0x%x]\n", irq);
#endif

	// external device's irq, ack PIC8259
	end_irq(irq);

	// restore masking prioriry
	apic_write(APIC_TASKPRI, task_pri);

}


