


#include <gdt.h>
#include <types.h>
#include <page.h> // alloc_bootmem()
#include <string.h> // memset

#include <printf.h>


#define DEBUG 
#undef DEBUG


struct gdt_desc *gdt_table;

struct tss_struct *tss;


//
// address in GDTR must be linear address
// @newgdt: linear address of new GDT 
// @limit: size of GDT
//
void set_gdt(void *newgdt, u16 limit)
{
	struct gdt_desc_ptr curgdtr;

	curgdtr.size = limit;
	curgdtr.address = (u64)newgdt;


	
	__asm__ __volatile__ (
			"lgdtq %0\n"
			: : "m" (curgdtr)
			);

	load_segments();

}



//
// initialize segment registers, ds,es,ss,fs,gs, with data segment index.
//
void load_segments(void)
{
	// memory barrier is needed
	__asm__ __volatile__ (
			"\tmovl %0, %%ds\n"
			"\tmovl %0, %%es\n"
			"\tmovl %0, %%ss\n"
			"\tmovl %0, %%fs\n"
			"\tmovl %0, %%gs\n"
			: : "a" (__KERNEL_DS64) : "memory"
			);
}



//
// allocated one page for TSS and set TR register.
// @num: TSS segment index in GDT
//
void set_tss(size_t num)
{

	u64 address = (u64)alloc_bootmem(1);
	u16 tss_selector = num << 3;

	set_gdt_desc(num, (u32)(address & 0xFFFFFFFF), 104, 0x89);

	gdt_table[num+1].l = (u32)((address>>32) & 0xFFFFFFFF);

	
	caos_printf("TSS address=%x\n", address);
	caos_memset((void *)address, 0, PAGE_SIZE);

	__asm__ __volatile__ (
			"\tltrw %0\n"
			: : "m" (tss_selector)
			);

	tss = (struct tss_struct *)address;


}



void gdt_init(u64 boot_gdt_addr)
{
	u64 *boot_gdt = (u64 *)boot_gdt_addr;
	int i = 1;

	gdt_table = (struct gdt_desc *)alloc_bootmem(1);


	//
	// copy booting-step GDT and re-use it.
	//
	while (boot_gdt[i] != 0) {
		caos_memcpy(&gdt_table[i], &boot_gdt[i], sizeof(struct gdt_desc));
		i++;
	}

#ifdef DEBUG
	caos_printf("%d desc copy\n", i);

	for (; i>0; i--) {
		caos_printf("%x\n", gdt_table[i]);
	}

#endif

	set_gdt(gdt_table, PAGE_SIZE);

	caos_printf("GDT table at %x\n", (u64)gdt_table);

	// 0: null desc
	// 1~2: 32bit desc
	// 3~4: 64bit desc
	// 5~6: TSS desc
	
	set_tss(5);


}



void set_gdt_desc(size_t num, u32 addr, u32 offset, flag_t type )
{

	gdt_table[num].l	= (u32)(offset & 0xffff)
				| (u32)((addr & 0xffff)<<16);
	
	gdt_table[num].h	= (u32)(addr & 0xff000000)
				| (u32)((addr>>16) & 0xff)
				| (u32)(offset & 0xf0000)
				| (u32)((type<<12) & 0xf00000)
				| (u32)((type<<8) & 0xff00);
}





