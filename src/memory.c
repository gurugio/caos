
#include <types.h>
#include <memory.h>
#include <page.h>

#include <list.h>	// struct list_head
#include <printf.h>



#define DEBUG 1
//#undef DEBUG

#define ONE_MB (1024*1024)

static char *zone_names[MAX_NR_ZONES] = {"NORMAL"};


// x86 has only one node
pg_data_t contig_page_data;







//
// setup memory node & zone
// @phy_mem_size: physical memory size
//
void memory_init(size_t phy_mem_size)
{
	struct zone *z;
	size_t l;

	struct list_head *head;
	struct page *pg;

	//
	// system node setup
	//
	
	contig_page_data.nr_zones = MAX_NR_ZONES;
	contig_page_data.node_mem_map = mem_map;
	contig_page_data.node_start_paddr = 0;
	contig_page_data.node_size = phy_mem_size*ONE_MB;

	// nodes are for NUMA & SMP system.
	// These are reserved for future.
	contig_page_data.node_id = 0;
	contig_page_data.node_next = NULL;	// only node


	//
	// zone setup
	//
	
	z = &contig_page_data.node_zones[ZONE_NORMAL];

	z->size = contig_page_data.node_size;	// size
	z->zone_pgdat = &contig_page_data;		// including node
	z->zone_start_paddr = contig_page_data.node_start_paddr;	// physical memory address
	z->name = zone_names[ZONE_NORMAL];		// name

	z->pages = contig_page_data.node_size / PAGE_SIZE; // included page count

	// the first page index
	z->zone_start_pfn = (size_t)(contig_page_data.node_mem_map - mem_map);
	
	// descriptor of the first page belongs to the zone
	z->zone_mem_map = &mem_map[z->zone_start_pfn];	

	//
	// Pages in the same zone is linked together
	//
	for (l=z->zone_start_pfn; l<z->pages; l++) {
		list_add_tail(&mem_map[l].list, &(z->zone_mem_map->list) );
		mem_map[l].page_zone = z;

		// what is this for??
		//if (mem_map[l].count != 0) {
		//	mem_map[l].private = -1;
		//}
	}

	// count free pages in the zone
	head = &contig_page_data.node_mem_map[0].list;
	list_for_each_entry(pg, head, list) {
		if (pg->count == 0)
			(z->free_pages)++;
	}


#ifdef DEBUG
	caos_printf("zone[%s] size=%d, mem_map=%x, free=%d\n",
			z->name, z->size, z->zone_mem_map, z->free_pages);
#endif



}




