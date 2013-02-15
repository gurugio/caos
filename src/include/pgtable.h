#ifndef __PGTABLE_H__
#define __PGTABLE_H__

/*
 * pgtable.h
 *
 * Here are macros and constant values for page table processing
 *
 * Everything here is almost same with the linux kernel include/asm_x86-64/pgtable.h
 * and comment is copied from Understanding Linux Kernel book.
 *
 * - 2008.11.24 gurugio
 *
 */


/*
 * linear address of kernel area
 *
 * Physical memory 0~8MB is mapped to 0xFFFF800000000000UL
 */
#define PAGE_OFFSET (0xFFFF800000000000UL)


//#define KERNEL_TEXT_START (0xFFFF800000100000UL)
//#define KERNEL_STACK_START (0xFFFF800000200000UL)

#define KERNEL_TEXT_START KERNEL_VIRTUAL_ADDR
#define KERNEL_STACK_START KERNEL_STACK_ADDR


#define BSP_STACK_START KERNEL_STACK_START
#define AP_STACK_START (0xFFFF80000010E000UL) // 2 pages are for BSP stack


//
// The first implementation fo Intel 64 support 48bit linear address
// and 40bit physical address
//
#define __PHYSICAL_MASK_SHIFT 40
#define __VIRTUAL_MASK_SHIFT 48
#define __PHYSICAL_MASK ((1UL<<__PHYSICAL_MASK_SHIFT)-1)
#define __VIRTUAL_MASK ((1UL<<__VIRTUAL_MASK_SHIFT)-1)


/*
 * bit field of page table entry
 */
#define _PAGE_BIT_PRESENT   0
#define _PAGE_BIT_RW        1
#define _PAGE_BIT_USER      2
#define _PAGE_BIT_PWT 		3
#define _PAGE_BIT_PCD 		4
#define _PAGE_BIT_ACCESSED  5
#define _PAGE_BIT_DIRTY     6 // only for page-table entry
#define _PAGE_BIT_PSE 		7
#define _PAGE_BIT_GLOBAL 	8


#define _PAGE_PRESENT	0x001
#define _PAGE_RW    0x002
#define _PAGE_USER  0x004
#define _PAGE_PWT 	0x008
#define _PAGE_PCD 	0x010
#define _PAGE_ACCESSED  0x020
#define _PAGE_DIRTY 0x040
#define _PAGE_PSE 	0x080
#define _PAGE_GLOBAL 	0x100




//
// Physical address <-> Virtual address
//
#define __pa(x)         ((unsigned long long)(x)-PAGE_OFFSET)
#define __va(x)         ((void *)((unsigned long long)(x)+PAGE_OFFSET))




/* entry format of page-table */
// In x86-64 arch, entry of every paging structure is 64bit
typedef struct { unsigned long long pte; } pte_t;
typedef struct { unsigned long long pmd; } pmd_t;
typedef struct { unsigned long long pud; } pud_t;
typedef struct { unsigned long long pgd; } pgd_t;
typedef struct { unsigned long long pgprot; } pgprot_t;



//
// access page DIR/Table entry, cast specialized type into an unsigned integer
//
#define pte_val(x) ((x).pte)
#define pmd_val(x) ((x).pmd)
#define pud_val(x) ((x).pud)
#define pgd_val(x) ((x).pgd)
#define pgprot_val(x) ((x).pgprot)


//
// cast an unsigned integer into the required type
//
#define __pte(x) ((pte_t) { (x) } )
#define __pmd(x) ((pmd_t) { (x) } )
#define __pud(x) ((pud_t) { (x) } )
#define __pgd(x) ((pgd_t) { (x) } )
#define __pgprot(x) ((pgprot_t) { (x) } )







/*
 * macros for page table handling
 *
 */


// the address of page-descriptor of page-frame number pfn.
#define __pfn_to_page(pfn) (mem_map + (pfn))

// the number of physical page-frame of page-descriptor 'page'.
#define __page_to_pfn(page) ((unsigned long)((page) - mem_map))



/*
 * PGD (Page Global DIR) is PML4 table of x86-64
 * Level 4 access.
 */
#define PGDIR_SHIFT 39 // length in bits of the PGDIR field, linear address [47:39]
#define PGD_SIZE (1UL<<PGD_SHIFT)	// one PGD entry cover 2^39 byte memory
#define PGD_MASK (~(PGDIR_SIZE-1))



#define pgd_none(x)	(!pgd_val(x))
#define pgd_page_vaddr(pgd) ((unsigned long long) __va((unsigned long long)pgd_val(pgd) & PTE_MASK))

// Yield the page descriptor address of the page frame
// containing the PUD refereed to by the PGD entry pgd.
#define pgd_page(pgd)		(pfn_to_page(pgd_val(pgd) >> PAGE_SHIFT))

// index of the entry of PGD that map the linear address 'address'
#define pgd_index(address) (((address) >> PGDIR_SHIFT) & (PTRS_PER_PGD-1))

// Receives as parameters of the address of a memory descriptor mm,
// and a linear address addr.
// This macro yields the linear address of the entry in PGD
// that corresponds to the address addr.
#define pgd_offset(mm, addr) ((mm)->pgd + pgd_index(addr))
#define pgd_present(pgd) (pgd_val(pgd) & _PAGE_PRESENT)

static inline void set_pgd(pgd_t *dst, pgd_t val)
{
	pgd_val(*dst) = pgd_val(val); 
} 

static inline void pgd_clear (pgd_t * pgd)
{
	set_pgd(pgd, __pgd(0));
}





/*
 * PUD (Page Upper DIR) is page-directory-pointer table of x86-64
 * PUD - Level3 access
 */
#define PUD_SHIFT 30 	// linear address [38:30]
#define PUD_SIZE (1UL<<PUD_SHIFT)
#define PUD_MASK	(~(PUD_SIZE-1))

/* to find an entry in a page-table-directory. */
#define pud_page_vaddr(pud) ((unsigned long long) __va(pud_val(pud) & PHYSICAL_PAGE_MASK))

// return the linear address of the PMD referred to by the PUD entry pud.
#define pud_page(pud)		(pfn_to_page(pud_val(pud) >> PAGE_SHIFT))

// index of entry in PUD to the address
#define pud_index(address) (((address) >> PUD_SHIFT) & (PTRS_PER_PUD-1))

// Yield the inear address of the entry in a PUD that corresponds to pgd, addr.
// pgd: PGD entry corresponds to the pud_offset
// address: linear address of somewhere in memory
#define pud_offset(pgd, address) ((pud_t *) pgd_page_vaddr(*(pgd)) + pud_index(address))
#define pud_present(pud) (pud_val(pud) & _PAGE_PRESENT)
#define pud_none(x)	(!pud_val(x))

static inline void set_pud(pud_t *dst, pud_t val)
{
	pud_val(*dst) = pud_val(val);
}

static inline void pud_clear (pud_t *pud)
{
	set_pud(pud, __pud(0));
}



/*
 * PMD (Page Middle DIR) is page-directory table of x86-64
 * PMD  - Level 2 access
 */
#define PMD_SHIFT 21 	// linear address [29:21]
#define PMD_SIZE (1UL<<PMD_SHIFT)
#define PMD_MASK	(~(PMD_SIZE-1))

#define pmd_page_vaddr(pmd) ((unsigned long long) __va(pmd_val(pmd) & PTE_MASK))
#define pmd_page(pmd)		(pfn_to_page(pmd_val(pmd) >> PAGE_SHIFT))

// index of PMD entry
#define pmd_index(address) (((address) >> PMD_SHIFT) & (PTRS_PER_PMD-1))

// linear address of the PMD entry that maps address
#define pmd_offset(dir, address) ((pmd_t *) pud_page_vaddr(*(dir)) + \
			pmd_index(address))
#define pmd_none(x)	(!pmd_val(x))
#define pmd_present(x)	(pmd_val(x) & _PAGE_PRESENT)
#define pmd_clear(xp)	do { set_pmd(xp, __pmd(0)); } while (0)
#define pfn_pmd(nr,prot) (__pmd(((nr) << PAGE_SHIFT) | pgprot_val(prot)))
#define pmd_pfn(x)  ((pmd_val(x) & __PHYSICAL_MASK) >> PAGE_SHIFT)


static inline void set_pmd(pmd_t *dst, pmd_t val)
{
        pmd_val(*dst) = pmd_val(val); 
} 



/*
 * PTE (page Table) is page-table of x86-64
 * PTE - Level 1 access.
 */
#define PTE_SHIFT 12 	// linear address [20:12]
#define PTE_SIZE (1UL<<PTE_SHIFT)
#define PTE_MASK (~(PAGE_SIZE-1))


/* page, protection -> pte */
#define mk_pte(page, pgprot)	pfn_pte(page_to_pfn(page), (pgprot))
#define mk_pte_huge(entry) (pte_val(entry) |= _PAGE_PRESENT | _PAGE_PSE)
 
#define pte_present(x)	(pte_val(x) & (_PAGE_PRESENT | _PAGE_PROTNONE))
#define pte_clear(mm,addr,xp)	do { set_pte(mm, addr, xp, __pte(0)); } while (0)


// page-descriptor for PTE entry x
#define pte_page(x)	pfn_to_page(pte_pfn(x))
// page-frame number of PTE entry x
#define pte_pfn(x)  ((pte_val(x) & __PHYSICAL_MASK) >> PAGE_SHIFT)

#define pte_none(x)	(!pte_val(x))

#define pte_index(address) \
		(((address) >> PAGE_SHIFT) & (PTRS_PER_PTE - 1))

// address of PTE entry that corresponds the address and PMD dir
#define pte_offset_kernel(dir, address) ((pte_t *) \
		pmd_page_vaddr(*(dir)) + pte_index(address))


static inline void set_pte(pte_t *dst, pte_t val)
{
	pte_val(*dst) = pte_val(val);
} 


/*
 * PAGE is physical page-frame
 *
 */
#define PAGE_SHIFT 12
#define PAGE_SIZE (1UL<<PAGE_SHIFT)
#define PAGE_MASK (~(PAGE_SIZE-1))


// mask for physical address
#define PHYSICAL_PAGE_MASK ((~(PAGE_SIZE-1UL)) & ((1UL<<__PHYSICAL_MASK_SHIFT) - 1UL))

/* align the pointer(linear address) to the (next) page boundary */
#define PAGE_ALIGN(addr) (((addr)+PAGE_SIZE-1) & PAGE_MASK)



/* number of entry in page table/dir */
#define PTRS_PER_PUD 512
#define PTRS_PER_PMD 512
#define PTRS_PER_PGD 512
#define PTRS_PER_PTE 512





//
// read/write attribute of page-table entry
//
static inline int pte_dirty(pte_t pte)		
{ return pte_val(pte) & _PAGE_DIRTY; }

static inline int pte_young(pte_t pte)		
{ return pte_val(pte) & _PAGE_ACCESSED; }

static inline int pte_write(pte_t pte)		
{ return pte_val(pte) & _PAGE_RW; }

static inline int pte_user(pte_t pte)
{ return pte_val(pte) & _PAGE_USER; }


static inline pte_t pte_mkclean(pte_t pte)	
{ set_pte(&pte, __pte(pte_val(pte) & ~_PAGE_DIRTY)); return pte; }

static inline pte_t pte_mkold(pte_t pte)	
{ set_pte(&pte, __pte(pte_val(pte) & ~_PAGE_ACCESSED)); return pte; }

static inline pte_t pte_wrprotect(pte_t pte)	
{ set_pte(&pte, __pte(pte_val(pte) & ~_PAGE_RW)); return pte; }

static inline pte_t pte_mkdirty(pte_t pte)	
{ set_pte(&pte, __pte(pte_val(pte) | _PAGE_DIRTY)); return pte; }

static inline pte_t pte_mkyoung(pte_t pte)	
{ set_pte(&pte, __pte(pte_val(pte) | _PAGE_ACCESSED)); return pte; }

static inline pte_t pte_mkwrite(pte_t pte)	
{ set_pte(&pte, __pte(pte_val(pte) | _PAGE_RW)); return pte; }




#endif
