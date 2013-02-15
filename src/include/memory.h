
#ifndef	__MEMORY_H__
#define	__MEMORY_H__

#include <list.h>
#include <pgtable.h>
#include <page.h>
#include <types.h>


// 
// zoned buddy allocator.
// Maximum order of page number to be a buddy.
//
#define MAX_ORDER 5


//
// I have only one node.
//
enum zone_type {
	ZONE_NORMAL,	// 0
	MAX_NR_ZONES 	// 1
};





//
// memory-zone structure
//
// zone is contiguos page-sequence, 
// that page-allocation search free pages in each zone at a time.
// Only page in the same zone can be allocated, not across zones.
//
struct zone {
	// number of free pages in zone
	size_t free_pages;
	// node includes zone
	struct pglist_data *zone_pgdat;
	// physical memory address
	u64 zone_start_paddr;
	// zone name
	char *name;
	// memory size (in byte)
	size_t size;
	// number of total pages
	size_t pages;

	// first mem_map entry belongs to zone
	struct page *zone_mem_map;
	// first page-frame number belongs to zone
	size_t zone_start_pfn;

};



// system-node structure
typedef struct pglist_data {
	// zone
	struct zone node_zones[MAX_NR_ZONES];
	// number of zones
	size_t nr_zones;
	// pointer to mem_map of this node
	struct page *node_mem_map;
	// physical memory address
	u64 node_start_paddr;
	// memory size
	size_t node_size;
	// node index
	size_t node_id;
	// pointer to next node
	struct pglist_data *node_next;
} pg_data_t;




void memory_init(size_t);


extern pg_data_t contig_page_data;


#endif
