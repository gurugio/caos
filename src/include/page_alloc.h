#ifndef __PAGE_ALLOC_H__
#define __PAGE_ALLOC_H__

#include <memory.h>
#include <types.h>
#include <bitops.h>


#define MAX_PAGE_ORDER 0x5



// convert page-count to page-order
#define PAGE_ORDER(x) fls64(x)


struct page *alloc_pages(flag_t gfp_mask, size_t order);
struct page *__alloc_pages(flag_t, size_t, struct zone *);
unsigned long get_free_pages(flag_t gfp_mask, size_t order);

void free_pages(u64 addr, size_t order);
void __free_pages(struct page *, size_t order);






#endif
