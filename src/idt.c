



#include <idt.h>
#include <gdt.h>	// __KERNEL_CS64
#include <types.h>

#include <string.h> // caos_memset()
#include <pgtable.h> // PAGE_SIZE
#include <page.h>	// alloc_bootmem()

#include <except_entry.h>
#include <irq_entry.h>	// _ignore_isr

#include <printf.h>







//
// load IDTR with linear address and size of IDT
// @newidt: linear address of IDT
// @limit: size of IDT
//
static void set_idt(void *newidt, u16 limit)
{
	struct idt_desc_ptr curidtr;

	curidtr.size = limit;
	curidtr.address = (u64)newidt;

	__asm__ __volatile__ (
			"lidtq %0\n"
			: : "m" (curidtr)
			);

}



//
// IDT table build
//
void idt_init(void)
{
	size_t i;

	idt_table = (struct idt_desc *)alloc_bootmem(1);



	// clear IDT area
	caos_memset(idt_table, 0, PAGE_SIZE);


	// default interrupt handler
	for (i = 0; i < IDT_SIZE; i++) {
		set_trap_gate(i, _ignore_isr);
	}


	// set GDTR
	set_idt(idt_table, PAGE_SIZE);

	caos_printf("IDT table at %x\n", (u64)idt_table);

	
	// processor exception handlers
	set_trap_gate(0, _divide_error);
	set_trap_gate(1, _debug);
	set_intr_gate(2, _nmi);
	set_system_gate(3, _int3);
	set_system_gate(4, _overflow);
	set_system_gate(5, _bounds);
	set_trap_gate(6, _invalid_op);
	set_trap_gate(7, _device_not_available);
	set_trap_gate(8, _doublefault_fn);
	set_trap_gate(9, _coprocessor_segment_overrun);
	set_trap_gate(10, _invalid_TSS);
	set_trap_gate(11, _segment_not_present);
	set_trap_gate(12, _stack_segment);
	set_trap_gate(13, _general_protection);
	set_intr_gate(14, _page_fault);
	set_trap_gate(16, _coprocessor_error);
	set_trap_gate(17, _alignment_check);
	set_trap_gate(18, _machine_check);
	set_trap_gate(19, _simd_coprocessor_error);


}



//
// add interrupt-gate into IDT
// @num: exception number
// @handler: linear address of entry point of exception handler
//
void set_intr_gate(size_t num, void *handler)
{
		if (num < IDT_SIZE) {
			idt_table[num].offset0_15 = (u16)((u64)handler & 0xffff);
			idt_table[num].selector = __KERNEL_CS64;
			idt_table[num].type = DESTYPE_INT;
			idt_table[num].offset16_31 = (u16)(((u64)handler & 0xFFFF0000) >> 16);
			idt_table[num].offset32_63 = (u32)(((u64)handler & 0xFFFFFFFF00000000) >> 32);
			idt_table[num].reserved = (u32)0;
		}

}


// system-call descriptor
void set_system_gate(size_t num, void *handler)
{	
	if (num < IDT_SIZE) {
		idt_table[num].offset0_15 = (u16)((u64)handler & 0xffff);
		idt_table[num].selector = __KERNEL_CS64;
		idt_table[num].type = DESTYPE_SYS;
		idt_table[num].offset16_31 = (u16)(((u64)handler & 0xFFFF0000) >> 16);
		idt_table[num].offset32_63 = (u32)(((u64)handler & 0xFFFFFFFF00000000) >> 32);
		idt_table[num].reserved = (u32)0;
	}


}

void set_trap_gate(size_t num, void *handler)
{		
	if (num < IDT_SIZE) {
		idt_table[num].offset0_15 = (u16)((u64)handler & 0xffff);
		idt_table[num].selector = __KERNEL_CS64;
		idt_table[num].type = DESTYPE_TRAP;
		idt_table[num].offset16_31 = (u16)(((u64)handler & 0xFFFF0000) >> 16);
		idt_table[num].offset32_63 = (u32)(((u64)handler & 0xFFFFFFFF00000000) >> 32);
		idt_table[num].reserved = (u32)0;
	}


}




