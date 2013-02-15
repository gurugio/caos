

#include <io.h>
#include <pic8259.h>
#include <irq_handler.h>

#include <printf.h>


// 2nd chnnel of 8259a which is connected to 1st 8259a
// should be unmasked initially.
u16 irq_mask = 0xfffb;

#define	irq_mask0_7	(unsigned char)(irq_mask&0xff)
#define	irq_mask8_15	(unsigned char)((irq_mask>>8)&0xff)



//
// activate PIC8259A
// IRQ base vector is 0x20 and 16 IRQs are supported.
//
void pic8259_init(void)
{

	//--------- 8259A initialization starts --------------
	
	/* COMMAND: initialize hardware interrupt base vector... */
	outb( 0x20, 0x11 );	// you are master mode
	outb( 0xa0, 0x11 );	// you are slave mode
	
	// VALUE: base vector number
	outb( 0x21, VECTOR_BASE );	// vector 0x20~0x27
	outb( 0xa1, VECTOR_BASE );	// vector 0x28~0x2F
	
	// COMMAND: cascade mode
	outb( 0x21, 0x04 );	// master's IRQ#2 is connected to slave
	outb( 0xa1, 0x02 );	// slave is connected to master's IRQ#2
	
	// COMMAND: working mode
	outb( 0x21, 0x01 );	// 8086 mode
	outb( 0xa1, 0x01 );	// 8086 mode
	
	//--------- 8259A initialization is over --------------
	
	// mask all IRQs now
	outb( 0x21, irq_mask0_7 ); // mask 0~1,3~7 IRQs
	outb( 0xa1, irq_mask8_15 ); // mask 8~15 IRQs


}


/*
 * enable specified IRQ by store mask bit.
 * @irq: irq number
 */
void enable_pic8259(size_t irq)
{
	irq_mask	= irq_mask & ~(1<<irq);

	if( irq >= 8 )
		outb( 0xa1, irq_mask8_15 );
	else
		outb( 0x21, irq_mask0_7 );
}

/*
 * disable specified irq by set mask bit
 * @irq: irq number
 */
void disable_pic8259(size_t irq)
{
	irq_mask	= irq_mask | (1<<irq);

	if( irq >= 8 )
		outb( 0xa1, irq_mask8_15 );
	else
		outb( 0x21, irq_mask0_7 );
}


//
// acknowledge PIC8259 to accept following IRQs 
//
void ack_pic8259(void)
{
	outb(0x20, 0x20);	// EOI to master
	outb(0xa0, 0x20);	// EOI to slave
}




