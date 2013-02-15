
/********************************************************************
 * DESCRIPTION : I do not know what these function do exactly.
 *               I just copy it from several source.
 *               I hate hardware programming, 
 *               but I have studied how hardware work for 2 years
 *               to make my own OS. It is so difficult to me. ;-)
 *               The most important thing is I cannot write any
 *               description this file. :-(
 * FILE NAME : IRQ.h
 *******************************************************************/
#ifndef __IRQ_HANDLER_H__

#define __IRQ_HANDLER_H__

#include <types.h>
#include <idt.h>


// maximum IRQ
#define MAX_IRQ 32


// Vector number is IDT index
// IDT[0~0x1F] are reserved for processor's exceptions.
// Then the base of vector number of IRQ
// If keyboard's irq is #1, vector number of keyboard irq is 0x21
// IO APIC vector must begin at 0x20 so that this value become 0x20
#define VECTOR_BASE 0x20

typedef void (*irq_handler_t)(size_t);



//
// IRQ number is device number (eg. keyboard irq = #1)
// But vector number is the index of IDT
// and then vector number is (#irq + 0x20).
//
#define IRQ_TO_VECTOR(num) (num+VECTOR_BASE)

#define DECLARE_IRQ(num)	\
	extern void _irq_##num(void);	\
	set_intr_gate(IRQ_TO_VECTOR(num), _irq_##num);


void irq_init(void);
void ignore_isr_handler(size_t);

extern irq_handler_t irq_handler[MAX_IRQ];

void handle_irq(size_t);
void end_irq(size_t);

int register_irq(size_t, irq_handler_t);

#endif
