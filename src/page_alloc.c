


#include <types.h>
#include <memory.h>
#include <page_alloc.h>


#include <printf.h>

#define DEBUG 1
#undef DEBUG




//
// allocate pages in order-number and return page-descriptor of the first free-page
// @gfp_mask: page attributes
// @order: power of two of allocation size in pages,
// 0 is a single page, 3 is 2^3=8 pages
//
struct page *alloc_pages(flag_t gfp_mask, size_t order)
{
	if (order > MAX_PAGE_ORDER)
		return NULL;
	
	//
	// gfp_mask processing should be here
	// According to the gfp_mask, node is selected if there are several nodes.
	//
	
	// only one node is considered yet.
	return __alloc_pages(gfp_mask, order, contig_page_data.node_zones);

}



//
// heart of alloc_pages
// @gfp_mask: page attributes
// @order: power of two of allocation size in pages,
// @zonelist: a list memory zone which has free pages 
// 
struct page *__alloc_pages(flag_t gfp_mask, size_t order, struct zone *zonelist)
{
	struct page *map;
	struct zone *zone;

	size_t first, last;
	size_t count = (1UL<<order);
	size_t i;
	


	i = 0;

FIND_ZONE:
	// find zone that has enough pages
	for (; i < MAX_NR_ZONES; i++) {
		if (zonelist[i].free_pages > count) {
			zone = &zonelist[i];
			map = zone->zone_mem_map;
			break;
		}
	}

	// not enough pages
	if (i >= MAX_NR_ZONES) return NULL;



	//
	// Here should come critical-section processing!!!
	//


	// find sequence of free pages in the zone
	for (first = 0; first < zone->pages; first++) {
		// find a free page
		if (map[first].count != 0) {
			continue;
		}
		

		// map[first]~map[first+count] are free?
		for (last=0; last<count; last++) {
			// no free page
			if (map[first+last].count > 0) {
				first += last;
				break; 
			}
		}


		// found enough space
		if (last == count) {
#ifdef DEBUG
			caos_printf("alloc_pages(%x~%x) ", first, first+last-1);
#endif
			// increase usage-counter of every allocated pages
			for (last=0; last<count; last++) {
				map[first+last].count = 1; 
				map[first+last].flags = gfp_mask;
			}

			zone->free_pages = zone->free_pages - count;

			// number of pages which are allocated together
			map[first].private = order;

#ifdef DEBUG
			caos_printf("[%x]private=%d\n", map+first, map[first].private);
#endif



			// following pages are free, return page pointer
			return map + first;
		} // if(last==count)


	}

	// the number of free-pages is enough
	// but free-pages in the zone is not sequential.
	// go to next zone
	if (i < MAX_NR_ZONES) {
		i++;
		goto FIND_ZONE;
	}

	// it never come to here!
	return NULL;
}


//
// allocate pages in order-number and return linear address of the first free-page
// @gfp_mask: page attributes
// @order: power of two of allocation size in pages,
//
u64 __get_free_pages(flag_t gfp_mask, size_t order)
{
	struct page *pg;


	if (order > MAX_PAGE_ORDER)
		return (u64)NULL;

	pg = __alloc_pages(gfp_mask, order, contig_page_data.node_zones);
	
#ifdef DEBUG
	caos_printf("get_free_pages : %x..OK\n", (size_t)pg->index);
#endif

	return page_to_virt(pg);
}



//
// free pages in order-number, linear address is necessary
// @addr: linear address of the first page
// @order: power of two of pages to be free
//
void free_pages(u64 addr, size_t order)
{

	if (order > MAX_PAGE_ORDER)
		return;


	__free_pages(virt_to_page(addr), order);

}




//
// free pages in order-number
// @page: page descriptor of the first page
// @order: power of two of pages to be free
//
void __free_pages(struct page *page, size_t order)
{
	
	size_t i;
	size_t pg_count = 1<<order;

	struct zone *zone;

	if (page == NULL || order > MAX_PAGE_ORDER)
		return;

	if (page->count <= 0) {
		caos_printf("free-pages fail. page[%x] is not allocated pages.\n", page);
		return;
	}

	if (page->private != order) {
		caos_printf("allocate-order is not same to free-order for page[%d]!!\n", page-mem_map);
		return;
	}


	zone = page->page_zone;

#ifdef DEBUG

	do {
		size_t first = (size_t)(page - zone->zone_mem_map);
		size_t last = first+(1<<order)-1;

		caos_printf("free_pages(%x~%x)[%x]\n", first, last, page);

	} while (0);
#endif

	// sequences to free pages is reverse to allocate pages.
	//
	// 1. decrease page-count
	// 2. increase free-pages count of zone
	// 3. set private field of page structure
	//
	

	for (i=0; i < pg_count; i++) {

		if (page[i].count <= 0) {
			caos_printf("free_pages fail <page-counter error> at page[%d]\n", page-mem_map);
		}

		page[i].count--;
		
		if (page[i].count == 0) {
			zone->free_pages++;
		}

	}

	// page-bulk are freed
	if (page->count == 0) {
		page->private = -1;
	}


}

