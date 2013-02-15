


/*
 *
 * 2008.11.27 - pgd_alloc, pgd_free, pud_alloc, pmd_alloc, pte_alloc_map... Page allocation functions will be added
 *
 */




#include <types.h>
#include <page.h>
#include <setup.h>	// init_mm
#include <pgtable.h> // paging tables

#include <printf.h>

#include <string.h>


#define DEBUG 
#undef DEBUG






//
// Page Global Dir for kernel
//
pgd_t *swapper_pg_dir;


//
// Page descriptor table
//
struct page *mem_map;



// amount of pages for booting-memory allocator
static size_t bootmem_page_index, bootmem_page_start, bootmem_page_max;


//
// setup booting-memory allocation by page size.
//
void bootmem_init(void)
{
	
	// free-memory area.
	// Physical memory 0x0~1MB is not used in booting sequence.
	// Kernel image is placed at 1MB ~ 1MB+(image-size)
	// Therefore setup.asm passed kernel-image size and 
	// kernel location is written in init_mm structure.
	
	// start_heap == kernel-stack starting == free-memory starting
	// Memory allocation is by page-frame size.
	bootmem_page_start = __pa(init_mm.start_heap) >> PAGE_SHIFT; // page number
	bootmem_page_index = bootmem_page_start;

	// bootloader builds paging-tables for 8MB
	// so that maximum free-page number is 0x7FF
	bootmem_page_max = (__pa(init_mm.end_heap) >> PAGE_SHIFT) - 1;

	caos_printf("bootmem page %x~%x\n", bootmem_page_index, bootmem_page_max);
}






//
// booting-page allocating, count is page number, not order.
// @requestied page count
//
void *alloc_bootmem(size_t count)
{

	if (bootmem_page_index+count >= bootmem_page_max) {
		caos_printf("FATAL ERROR: KERNEL HEAP EXHAUSTS");
		halt();
		return NULL;
	}

	bootmem_page_index += count;


	return __va((bootmem_page_index-count) << PAGE_SHIFT);

}


//================================================================
// mem_map_init must be placed at the end of booting memory users.
// After mem_map_init function is completed, 
// buddy system which is implemented with mem_map table, is activated
//================================================================

//
// Build mem_map table that represents physical page-frames
//
void mem_map_init(void)
{
	unsigned int page_count;

	int mem_map_size;
	int mem_map_page;
	unsigned int l;
	u64 virtual_addr;


	
	page_count = (phy_mem_size * 0x100000)/PAGE_SIZE;
	mem_map_size = page_count * sizeof(struct page) + 11;
	mem_map_page = PAGE_ALIGN(mem_map_size) / PAGE_SIZE;


#ifdef DEBUG
	caos_printf("page count = %d, mem_map_size = %dbyte, mem_map_page = %d\n", page_count, mem_map_size, mem_map_page);

#endif

	// 
	// Mem_map is allocated in kernel-booting memory (kernel heap).
	//
	
	mem_map = (struct page *)alloc_bootmem(mem_map_page);


	caos_printf("mem_map -> %x ~ %x \n", (unsigned long)mem_map, (unsigned long)mem_map + mem_map_page*4096 - 1);



	//
	// initialized mem_map table
	//
	virtual_addr = PAGE_OFFSET;
	for (l=0; l<page_count; l++) {
		INIT_LIST_HEAD(&mem_map[l].list);
		mem_map[l].index = l;
		mem_map[l].count = 0;
		mem_map[l].flags = 0;
		mem_map[l].private = -1;
		mem_map[l].virtual = (void *)virtual_addr;
		virtual_addr += 0x1000;
	}


	//
	// set busy pages which are allocated for kernel-booting memory
	// 
	// BIOS booting data : 0x0 ~ 0x1000 (0th page frame)
	// BIOS & ISA video  : 0x9F000 ~ 0x100000
	// kernel image      : 0x100000(0x100th page)~KERNEL_START_STACK
	// kernel heap       : KERNEL_START_STACK ~ bootmem_page_index (page number)
	// 


	//----------------------------------------------------------------
	// for future works...
	// struct resource data-structure must be defined in setup.h
	// and BIOS are will be managed as resource
	// Then busy-page couting will be modified as dynamic counting.
	//----------------------------------------------------------------
	mem_map[0].count = 1;

	for (l=0x9f; l<0x200; l++)
		mem_map[l].count = 1;
	
	for (l=bootmem_page_start; l<bootmem_page_index; l++)
		mem_map[l].count = 1;
	

	
#ifdef DEBUG

	caos_printf("mem_map[%d] : ", mem_map[0].index);
	caos_printf("list.next=%x list.prev=%x ", mem_map[0].list.next, mem_map[0].list.prev);
	caos_printf("count=%d\n", mem_map[0].count);
	caos_printf("mem_map[%d] : ", mem_map[1].index);
	caos_printf("list.next=%x list.prev=%x ", mem_map[1].list.next, mem_map[1].list.prev);
	caos_printf("count=%d\n", mem_map[1].count);
	
	caos_printf("mem_map[%d] ", mem_map[page_count-1].index);
	caos_printf("list.next=%x list.prev=%x ", mem_map[page_count-1].list.next, mem_map[page_count-1].list.prev);
	caos_printf("count=%d\n", mem_map[page_count-1].count);

	caos_printf("Free page start at %x\n",  (u64)alloc_bootmem(1));


#endif




}




//
// rebuild page-tables for kernel
// @phy_mem_size: physical memory size in MB size
//
void paging_init(size_t phy_mem_size)
{
	size_t l;
	pgd_t *pgd;
	pud_t *pud;
	pmd_t *pmd;
	pte_t *pte;


	pmd_t *pmd_reserv;
	pte_t *pte_reserv;

	
	// 1 PTE covers 2MB (4K * 512)
	// 1 PMD covers 1GB (2MB * 512)
	// 1 PUD covers 512G (1GB * 512)
	// don't care PGD coverage, it's too big??
	size_t pte_count, pmd_count, pud_count, page_count;

	u64 cr3_val;

	//
	// enable cache by clearing PCD bit!!
	// enable write-back cache-policy by clearing PWT bit!!
	// RW: page table or page is readable & writable
	// Present: page tables are currently loaded in physical memory
	//
	u64 page_attr = (_PAGE_RW + _PAGE_PRESENT);


	// one PUD table has 512 entries and
	// one entry can cover 1G 
	pud_count = phy_mem_size / (512 * 1024);
	if (pud_count == 0)
		pud_count = 1;


	// one PMD table => 1G, one entry => 2M
	pmd_count = phy_mem_size / (1024);
	if (pmd_count == 0)
		pmd_count = 1;


	// one PTE table => 2M, one entry => 4K page
	pte_count = phy_mem_size / 2;
	if (pte_count == 0) {
		caos_printf("FATAL ERROR in PAGING-TABLES BUILDING");
		halt();
		return;
	}


	// How many pages are there?
	page_count = phy_mem_size * 256; // 256 pages per 1MB



	// allocate only one PGD
	pgd = (pgd_t *)alloc_bootmem(1);
	pud = (pud_t *)alloc_bootmem(1);


	// allocate page-tables, each address is linear address.
	pmd = (pmd_t *)alloc_bootmem(pmd_count);
	pte = (pte_t *)alloc_bootmem(pte_count);


	caos_printf("pgd=0x%x/pud=0x%x \npmd=0x%x/pte=0x%x\n", 
			pgd, pud, pmd, pte);

	// clear every entries except PGD[0x100] 
	// PGD[0x100] => linear address 0xFFFF800000000000
	for (l=0; l<0x100; l++)
		set_pgd(pgd+l, __pgd(0));

	set_pgd(pgd+0x100, __pgd(__pa(pud) + page_attr) );

	for (l=0x101; l<0x200; l++)
		set_pgd(pgd+l, __pgd(0));

	
	// set entries of PUD to map PMD
	for (l = 0; l < pmd_count; l++) {
		set_pud(pud + l, __pud(__pa(pmd + (l*PAGE_SIZE)) +  page_attr) );
	}


	// set entries of PMD to map PMD
	for (l = 0; l < pte_count; l++) {
		set_pmd(pmd + l, __pmd(__pa(pte) + (l*PAGE_SIZE) +  page_attr) );
	}

	// set entries of PTE to map physical page-frames
	for (l = 0; l < page_count; l++) {
		set_pte(pte + l, __pte(l*PAGE_SIZE +  page_attr) );
	}


#ifdef DEBUG
	caos_printf("Init Paging-Table Structure, #%d pud, #%d pmd, #%d pte\n", 
			pud_count, pmd_count, pte_count);
	caos_printf("%x %x %x %x\n", pgd, pud, pmd, pte);
#endif




	//
	// reserve area - APIC, memory-mapped IO devices, BIOS PROM
	//
	// IO APIC: 0xFEC00000 -> linear 0xFFFF8000C0000000
	// Local API: 0xFEE00000 -> linear 0xFFFF8000C0200000
	//

    /* 2010.8.6 I think 0xFEC0_0000 should be mapped to 0xFFFF_8000_FEC0_0000.
     *          It is more pretty. I don't remember why I mapped like that ugly.
     *          Someday I will fix it and change LOCAL_APIC_BASE in apic.h.
     */
    
	// disable cache, only read
	page_attr = (_PAGE_PRESENT + _PAGE_PCD);

	pmd_reserv = (pmd_t *)alloc_bootmem(1);
	pte_reserv = (pte_t *)alloc_bootmem(10);
	
	// one PUD entry => 1G => 0x4000_0000
	// PUD[3] => 0xC000_0000
	set_pud(pud+3, __pud(__pa(pmd_reserv) + page_attr));

	// 10 PMD entries => 20M => 0x140_0000
	for (l=0; l<10; l++)
		set_pmd(pmd_reserv + l, __pmd(__pa(pte_reserv) + (l*PAGE_SIZE) + page_attr));

	// physical address 0xFEC0_0000 ~ 0xFFFF_F000
	// => linear address 0xFFFF_8000_C000_0000 ~ 0xFFFF_8000_C13F_F000
	for (l=0; l<512*10; l++)
		set_pte(pte_reserv + l, __pte((0xFEC00000 + l*PAGE_SIZE) + page_attr));


	caos_printf("pmd_reserv=0x%x/pte_reserv=0x%x\n", pmd_reserv, pte_reserv);

	//
	// CR3 has the physical address of process's PGD
	// enable page-level cache and write-back policy
	//
	
	cr3_val = read_cr3();
	cr3_val = __pa(pgd);
	cr3_val &= ~(X86_CR3_PCD | X86_CR3_PWT);
	write_cr3(cr3_val);


	// CR3 is changed that means address space is modified.
	// Therefore TLB and cache must be flushed.
	flush_cache();
	__flush_tlb_all();


	//
	// swapper_pg_dir is PGD of kernel
	//
	swapper_pg_dir = pgd;

	
#ifdef DEBUG
	{
		//
		// Test Paging
		//

		// check Paging-tables
		u64 *entry = (u64 *)0xFFFF800000203000;

		caos_printf("PML4[0x100]=%x, PDP[0]=%x\n PDE[0]=%x, PTE[0]=%x\n",
				*(entry +0x100), *(entry+512), *(entry+1024), *(entry+1536));


		// read/write last physical page
		entry = (u64 *)(0xFFFF800000000000 + phy_mem_size * 0x100000 - 0x1000);

		*entry = 0x123456789ABCDEF0;
		caos_printf("Paging test: 0x%x == 0x123456789ABCDEF0??\n", *entry);

	}
#endif

}






//
// return page descriptor that is mapped into linear address vaddr
// @vaddr: linear address
//
struct page *virt_to_page(u64 vaddr)
{
	size_t pfn = __pa(vaddr) >> PAGE_SHIFT;

#ifdef DEBUG
	caos_printf("virt (%x) to page #(%x)\n", vaddr, pfn);
#endif
	return mem_map+pfn;
}


//
// return linear address of page
// @pg: page descriptor
//
u64 page_to_virt(struct page *pg)
{
	size_t pfn = pg - mem_map;
#ifdef DEBUG
	caos_printf("page #(%x) to virt (%x)\n", pfn, __va(pfn<<PAGE_SHIFT));
#endif
	return (u64)__va(pfn << PAGE_SHIFT);
}






