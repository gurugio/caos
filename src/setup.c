

/*
 * The boot-time setup-routine, setup.asm, build boot-parameter for HW architecture.
 *
 * This file then handles the architecture-dependent parts of initialization.
 *
 */


#include <setup.h>
#include <types.h>

#include <processor.h> // struct cpuinfo_x86
#include <msr.h>	// cpuid()

#include <page.h>	// bootmem_init()
#include <memory.h>	// node_init()

#include <pgtable.h> // PAGE_OFFSET


#include <gdt.h>
#include <idt.h>

#include <printf.h>
#include <io.h>	// phys_to_virt
#include <mp_spec.h>

#include <apic.h> // apic_init
#include <irq_handler.h>	// irq_init
#include <pic8259.h>	// pic8259_init
#include <smp.h>	// wake_ap




#define DEBUG 1
#undef DEBUG




//
// memory status of #0 process, swapper
// The swapper process is the kernel itself.
//
struct mm_struct init_mm;


struct cpuinfo_x86 boot_cpu_data;

size_t phy_mem_size;
u64 boot_gdt;



//
// processor-dependent initializations for x86_64 
//
void setup_arch(void)
{
	u64 *boot_params = BOOT_PARAMS;




	// booting-parameter
	phy_mem_size = boot_params[PARAMS_OFFSET_PHYMEM_SIZE];
	boot_gdt = boot_params[PARAMS_OFFSET_BOOT_GDT];	



	// init_mm initialization
	
	// BIOS/ISA video area - this will be manages as resource in future
	init_mm.start_reserved = boot_params[PARAMS_OFFSET_START_VIDEO]; 
	init_mm.end_reserved = boot_params[PARAMS_OFFSET_END_VIDEO];

	// kernel-image map
	init_mm.start_code = boot_params[PARAMS_OFFSET_START_CODE];
	init_mm.end_code = boot_params[PARAMS_OFFSET_END_CODE];
	init_mm.start_data = boot_params[PARAMS_OFFSET_START_DATA];
	init_mm.end_data = boot_params[PARAMS_OFFSET_END_DATA];
	init_mm.start_bss = boot_params[PARAMS_OFFSET_START_BSS];
	init_mm.end_bss = boot_params[PARAMS_OFFSET_END_BSS];
	init_mm.start_brk = boot_params[PARAMS_OFFSET_END_KERNEL];	// end of kernel image
	
	// per-cpu data section
	init_mm.start_percpu = boot_params[PARAMS_OFFSET_START_PERCPU];
	init_mm.end_percpu = boot_params[PARAMS_OFFSET_END_PERCPU];

	// for BSP's initialization, such as building paging tables, mem_map...
	init_mm.start_stack = KERNEL_STACK_START;
	init_mm.start_heap = KERNEL_STACK_START;
	init_mm.end_heap = (u64)phys_to_virt(boot_params[PARAMS_OFFSET_BOOT_SIZE]*0x100000);

	

#ifdef DEBUG
	caos_printf("%x %x %x %x\n", init_mm.start_code, init_mm.start_data, init_mm.start_bss, init_mm.start_brk);
	caos_printf("%x %x\n", init_mm.start_heap, init_mm.end_heap);
#endif


	// struct resource code_resource, data_resource


	

	// early_identify_cpu() - boot CPU information at struct cpuinfo_x86 boot_cpu_data
	early_identify_cpu(&boot_cpu_data);



	// e820 handling comes here in future




	// booting memory allocator - reserved or occufied memory for kernel and initial data
	bootmem_init();


	// build GDT, IDT
	gdt_init(boot_gdt);		
	idt_init();

	// rebuild paging-tables, PGD/PUD/PMD/PTEs
	paging_init(phy_mem_size); // build paging-tables




	//
	// current page status if memory size is 128MB
	//
	// 0x9F ~ 0xFF: BIOS
	// 0x100 ~ 0x1FF: KERNEL
	// 0x200: GDT
	// 0x201: TSS
	// 0x202: IDT
	// 0x203: PGD
	// 0x204: PUD
	// 0x205: PMD
	// 0x206 ~ 0x245: PTE (128MB)
	// 0x246: PMD_reserv
	// 0x247 ~ 0x250: PTE_reserv
	// 0x251 ~ 0x491: mem_map (128MB)
	//


	
	// ACPI/SMP/E820 map handling - not implemented yet


	
	// SMP initialization
	mp_spec_init();
	

	// build mem_map - page descriptor table
	// mem_map_init must be placed at the end of booting memory users
	mem_map_init();

	//
	// Now, booting memory allocator is disabled
	//


}




//
// identify CPU vendor.
// Detail processor identification will be implemented 
// in identify_cpu() function in future.
//
void early_identify_cpu(struct cpuinfo_x86 *cpu)
{
	
	cpu->x86_cache_size = -1;
	cpu->x86_vendor = X86_VENDOR_UNKNOWN;	// unknown yet
	//cpu->x86_model = 0;
	cpu->x86_vendor_id[0] = '\0';
	//cpu->x86_model_id[0] = '\0';
	cpu->x86_cache_alignment = 64;	// hard-coding, bad!
	cpu->x86_max_cores = 4;		// more than 4-core is not considered.



	// cpuid instruction, refer to CPUID manual
	cpuid(0x00000000, (unsigned int *)&cpu->cpuid_level,
			(unsigned int *)&cpu->x86_vendor_id[0],
			(unsigned int *)&cpu->x86_vendor_id[8],
			(unsigned int *)&cpu->x86_vendor_id[4]);

	if (cpu->x86_vendor_id[9] == 'A' && \
			cpu->x86_vendor_id[10] == 'M' && \
			cpu->x86_vendor_id[11] == 'D')
		cpu->x86_vendor = X86_VENDOR_AMD;
	else if (cpu->x86_vendor_id[7] == 'I' && \
			cpu->x86_vendor_id[8] == 'n' && \
			cpu->x86_vendor_id[9] == 't' && \
			cpu->x86_vendor_id[10] == 'e' && \
			cpu->x86_vendor_id[11] == 'l')
		cpu->x86_vendor = X86_VENDOR_INTEL;

	caos_printf("CPU VENDOR=%s\n", cpu->x86_vendor_id);


}




