/********************************************************************
 * DESCRIPTION : functions and data for initializing Interrupt Descriptor Table
 * FILE NAME : idt.h
 *******************************************************************/

#ifndef __IDT_H__
#define __IDT_H__

//#include <irq.h>
#include <types.h>



/*
 * exception type
 */
#define DESTYPE_INT	0x8e00	/* interrupt descriptor type : 01110, DPL=0 */
#define DESTYPE_TRAP	0x8f00	/* trap descriptor type : 01111, DPL=0 */

#define DESTYPE_SYS 0xef00 // system call type, trap-type, DPL=3

#define IDT_SIZE 256			/* length of table */

/* Interrupt Descriptor Table */
struct idt_desc {
	u16 offset0_15;
	u16 selector;
	u16 type;
	u16 offset16_31;
	u32 offset32_63;
	u32 reserved;

} __attribute__ ((packed));

struct idt_desc_ptr {
	u16 size;	// 16bit table limit
	u64 address;	// 64bit linear address
} __attribute__ ((packed));



struct idt_desc *idt_table;

void idt_init(void);
void set_intr_gate(size_t, void *);
void set_system_gate(size_t num, void *handler);
void set_trap_gate(size_t num, void *handler);




#endif
