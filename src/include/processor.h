#ifndef __PROCESSOR_H
#define __PROCESSOR_H

#include <gdt.h>
#include <types.h>

#include <printf.h>



//
// Maximum 2 processor supported
//
#define NR_CPUS 2



// CPUID
#define X86_VENDOR_UNKNOWN 0xff
#define X86_VENDOR_INTEL 0
#define X86_VENDOR_AMD 1


// x86_64 cache align
#define X86_64_CACHE_ALIGN 16


/*
 * EFLAGS bits
 */
#define X86_EFLAGS_CF	0x00000001 /* Carry Flag */
#define X86_EFLAGS_PF	0x00000004 /* Parity Flag */
#define X86_EFLAGS_AF	0x00000010 /* Auxillary carry Flag */
#define X86_EFLAGS_ZF	0x00000040 /* Zero Flag */
#define X86_EFLAGS_SF	0x00000080 /* Sign Flag */
#define X86_EFLAGS_TF	0x00000100 /* Trap Flag */
#define X86_EFLAGS_IF	0x00000200 /* Interrupt Flag */
#define X86_EFLAGS_DF	0x00000400 /* Direction Flag */
#define X86_EFLAGS_OF	0x00000800 /* Overflow Flag */
#define X86_EFLAGS_IOPL	0x00003000 /* IOPL mask */
#define X86_EFLAGS_NT	0x00004000 /* Nested Task */
#define X86_EFLAGS_RF	0x00010000 /* Resume Flag */
#define X86_EFLAGS_VM	0x00020000 /* Virtual Mode */
#define X86_EFLAGS_AC	0x00040000 /* Alignment Check */
#define X86_EFLAGS_VIF	0x00080000 /* Virtual Interrupt Flag */
#define X86_EFLAGS_VIP	0x00100000 /* Virtual Interrupt Pending */
#define X86_EFLAGS_ID	0x00200000 /* CPUID detection flag */

/*
 * Basic CPU control in CR0
 */
#define X86_CR0_PE	0x00000001 /* Protection Enable */
#define X86_CR0_MP	0x00000002 /* Monitor Coprocessor */
#define X86_CR0_EM	0x00000004 /* Emulation */
#define X86_CR0_TS	0x00000008 /* Task Switched */
#define X86_CR0_ET	0x00000010 /* Extension Type */
#define X86_CR0_NE	0x00000020 /* Numeric Error */
#define X86_CR0_WP	0x00010000 /* Write Protect */
#define X86_CR0_AM	0x00040000 /* Alignment Mask */
#define X86_CR0_NW	0x20000000 /* Not Write-through */
#define X86_CR0_CD	0x40000000 /* Cache Disable */
#define X86_CR0_PG	0x80000000 /* Paging */

/*
 * Paging options in CR3
 */
#define X86_CR3_PWT	0x00000008 /* Page Write Through */
#define X86_CR3_PCD	0x00000010 /* Page Cache Disable */

/*
 * Intel CPU features in CR4
 */
#define X86_CR4_VME	0x00000001 /* enable vm86 extensions */
#define X86_CR4_PVI	0x00000002 /* virtual interrupts flag enable */
#define X86_CR4_TSD	0x00000004 /* disable time stamp at ipl 3 */
#define X86_CR4_DE	0x00000008 /* enable debugging extensions */
#define X86_CR4_PSE	0x00000010 /* enable page size extensions */
#define X86_CR4_PAE	0x00000020 /* enable physical address extensions */
#define X86_CR4_MCE	0x00000040 /* Machine check enable */
#define X86_CR4_PGE	0x00000080 /* enable global pages */
#define X86_CR4_PCE	0x00000100 /* enable performance counters at ipl 3 */
#define X86_CR4_OSFXSR	0x00000200 /* enable fast FPU save and restore */
#define X86_CR4_OSXMMEXCPT 0x00000400 /* enable unmasked SSE exceptions */
#define X86_CR4_VMXE	0x00002000 /* enable VMX virtualization */




/* thread structure... */
typedef struct _thread_struct
{
	unsigned int edi;
	unsigned int esi;
	unsigned int ebp;
	unsigned int esp0;
	unsigned int ebx;
	unsigned int edx;
	unsigned int ecx;
	unsigned int eax;
	unsigned short ds;
	unsigned short ds_res;
	unsigned short es;
	unsigned short es_res;
	unsigned short fs;
	unsigned short fs_res;
	unsigned short gs;
	unsigned short gs_res;
	unsigned int	eip;
	unsigned short cs;
	unsigned short cs_res;
	unsigned int eflags;
	unsigned int esp;
	unsigned short ss;
	unsigned short ss_res;
} thread_struct;



typedef struct _tss_struct {
	u32 reserved1;
	u64 rsp0;	
	u64 rsp1;
	u64 rsp2;
	u64 reserved2;
	u64 ist[7];
	u32 reserved3;
	u32 reserved4;
	u16 reserved5;
	u16 io_bitmap_base;
	/*
	 * The extra 1 is there because the CPU will access an
	 * additional byte beyond the end of the IO permission
	 * bitmap. The extra byte must be all 1 bits, and must
	 * be within the limit. Thus we have:
	 *
	 * 128 bytes, the bitmap itself, for ports 0..0x3ff
	 * 8 bytes, for an extra "long" of ~0UL
	 */
	unsigned long io_bitmap[IO_BITMAP_LONGS + 1];
} __attribute__((packed)) tss_struct;




struct cpuinfo_x86 {
	//u8 x86;				// CPU family
	u8 x86_vendor;		// CPU vendor
	//u8 x86_model;
	int cpuid_level;	// maximum supported CPUID level
	char x86_vendor_id[16];
	//char x86_model_id[64];
	int x86_cache_size;	// in KB
	int x86_cache_alignment;
	int x86_tlbsize;  // number of 4K pages in DTLB/ITLB combined
	u8 x86_virt_bits, x86_phys_bits;
	u8 x86_max_cores; // cpuid returend max cores value
	

	u8 apicid;

	u8 booted_cores; // number of cores as seen by OS
	u8 phys_proc_id; // physical processor id
	u8 cpu_core_id;  // core id

};



#define HALT while(1);

static inline void halt(void)
{
	while (1);
}


//
// CR0~CR4,CR8 handling functions, copied from linux-2.6.23
//

static inline u64 read_cr0(void)
{ 
	u64 cr0;
	asm volatile("movq %%cr0,%0" : "=r" (cr0));
	return cr0;
}

static inline void write_cr0(u64 val) 
{ 
	asm volatile("movq %0,%%cr0" :: "r" (val));
}

static inline u64 read_cr2(void)
{
	u64 cr2;
	asm("movq %%cr2,%0" : "=r" (cr2));
	return cr2;
}

static inline void write_cr2(u64 val)
{
	asm volatile("movq %0,%%cr2" :: "r" (val));
}

static inline u64 read_cr3(void)
{ 
	u64 cr3;
	asm("movq %%cr3,%0" : "=r" (cr3));
	return cr3;
}

static inline void write_cr3(u64 val)
{
	asm volatile("movq %0,%%cr3" :: "r" (val) : "memory");
}

static inline u64 read_cr4(void)
{ 
	u64 cr4;
	asm("movq %%cr4,%0" : "=r" (cr4));
	return cr4;
}

static inline void write_cr4(u64 val)
{ 
	asm volatile("movq %0,%%cr4" :: "r" (val) : "memory");
}

static inline u64 read_cr8(void)
{
	u64 cr8;
	asm("movq %%cr8,%0" : "=r" (cr8));
	return cr8;
}

static inline void write_cr8(u64 val)
{
	asm volatile("movq %0,%%cr8" :: "r" (val) : "memory");
}



static inline void __flush_tlb(void)
{
	// writing CR3 does flushing TLB
	write_cr3(read_cr3());
}

static inline void __flush_tlb_all(void)
{
	u64 cr4 = read_cr4();

	// disable Page-Global-Enable means 
	// all TLB must be flushed including global pages
	write_cr4(cr4 & ~X86_CR4_PGE);

	// restore CR4
	write_cr4(cr4);
}

static inline void flush_cache(void)
{
	asm volatile("wbinvd":::"memory");
}

#define cli() __asm__ __volatile__("cli":::"memory")
#define sti() __asm__ __volatile__("sti":::"memory")





#endif /* __PROCESSOR_H */
