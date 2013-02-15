
#ifndef	__PAGE_H__
#define	__PAGE_H__


#include <list.h>
#include <pgtable.h>
#include <types.h>

#include <bitops.h>
#include <page-flags.h>




/* mem_mep_t presents a page frame */
struct page {
	struct list_head list;	// free page-list in the same zone
	size_t private;	// for buddy system, order of buddy-pages
	size_t index;	// page number
	size_t count;	// reference counter
	flag_t flags;	// status flag
	void * virtual;	// virtual address the page is mapped

	struct zone *page_zone;	// including zone

	void *slab;				// including slab
};


extern struct page *mem_map;

void bootmem_init(void);
void *alloc_bootmem(size_t);

void paging_init(size_t);
void mem_map_init(void);

struct page *virt_to_page(u64 vaddr);
u64 page_to_virt(struct page *);




#endif
