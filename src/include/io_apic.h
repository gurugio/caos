#ifndef __IO_APIC_H__
#define __IO_APIC_H__


#include <io.h>
#include <processor.h>


/*
 * REFINE!! refer to linux-2.6.27/include/asm-x86/io_apic.h 
 *
 */


#define IO_APIC_BASE 0xFFFF8000C0000000

//#define MAX_IO_APICS 128

#define IO_APIC_ID 0x0
#define IO_APIC_VERSION 0x1
#define			GET_IO_APIC_VERSION(x)		((x)&0xF)
#define			GET_IO_APIC_ENTRY(x)	(((x)>>16)&0xFF)	// max entry number
#define IO_APIC_ARB 0x2

#define IO_APIC_REDTBL_BASE 0x10
#define IO_APIC_REDTBL0 0x10
#define IO_APIC_REDTBL1 0x12
#define IO_APIC_REDTBL2 0x14
#define IO_APIC_REDTBL3 0x16
#define IO_APIC_REDTBL4 0x18
#define IO_APIC_REDTBL5 0x1A
#define IO_APIC_REDTBL6 0x1C
#define IO_APIC_REDTBL7 0x1E
#define IO_APIC_REDTBL8 0x20
#define IO_APIC_REDTBL9 0x22
#define IO_APIC_REDTBL10 0x24
#define IO_APIC_REDTBL11 0x26
#define IO_APIC_REDTBL12 0x28
#define IO_APIC_REDTBL13 0x2A
#define IO_APIC_REDTBL14 0x2C
#define IO_APIC_REDTBL15 0x2E
#define IO_APIC_REDTBL16 0x30
#define IO_APIC_REDTBL17 0x32
#define IO_APIC_REDTBL18 0x34
#define IO_APIC_REDTBL19 0x36
#define IO_APIC_REDTBL20 0x38
#define IO_APIC_REDTBL21 0x3A
#define IO_APIC_REDTBL22 0x3C
#define IO_APIC_REDTBL23 0x3E
#define 		SET_IO_APIC_DEST(x) (((x)&0xFF)<<56)
#define 		IO_APIC_IRQ_MASK (1<<16)
#define 		IO_APIC_TRIG_MODE (1<<15)
#define 		IO_APIC_IRR (1<<14)
#define 		IO_APIC_INTPOL (1<<13)
#define 		IO_APIC_DELIVS (1<<12)
#define 		IO_APIC_DESTMOD (1<<11)
#define 		IO_APIC_DELMOD_FIX ((0)<<8)
#define 		IO_APIC_DELMOD_LOW ((1)<<8)
#define 		IO_APIC_DELMOD_SMI ((2)<<8)
#define 		IO_APIC_DELMOD_NMI ((4)<<8)
#define 		IO_APIC_DELMOD_INIT ((5)<<8)
#define 		IO_APIC_DELMOD_ExtINT ((7)<<8)



//
// copy of linux-2.6.20/arch/x86_64/kernel/io_apic.c
//
struct io_apic {
	// IO_APIC_BASE address is mapped to IO Register Select register (index)
	// IO_APIC_BASE+0x10 is mapped to IO Window register (data)
	u32 index;
	u32 unused[3];
	u32 data;
};



int io_apic_enable_irq(size_t irq);
void io_apic_init(void);




#endif
