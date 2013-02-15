
#include <slab.h>
#include <printf.h>
#include <bitops.h> // fls64()
#include <pgtable.h> // PAGE_SIZE
#include <page.h>	// struct page

#include <page_alloc.h> // alloc_pages
#include <string.h>	// memset
#include <types.h>

#include <processor.h> // halt

#define DEBUG 1
#undef DEBUG

//
// cache of cache
//
struct kmem_cache *cache_cache;

//
// page frame that cache_cache is stored
//
struct page *cache_page;


size_t cache_cache_max;

	




#define KMALLOC_SMALL_SIZE 16
#define KMALLOC_MAX_SIZE 2048

// cache names to indicate size of the caches
static const char *cache_names[] = {
	"16bytes",
	"32bytes",
	"64bytes",
	"128bytes",
	"256bytes",
	"512bytes",
	"1024bytes",
	"2048bytes"
};

void *kmem_getpages(struct kmem_cache *cachep, flag_t flag)
{

	struct page *page;
	ssize_t i;

	page = alloc_pages(flag, cachep->gfporder);
	if (page == NULL)
		return NULL;


	i = 1 << cachep->gfporder;
	while (i--) {
		SetPageSlab(&page[i]);

		// slab exists at head of first page
		page[i].slab = page->virtual;
	}



	return (void *)(page->virtual);
}



void kmem_freepages(struct kmem_cache *cachep, void *addr)
{
	size_t i = (1 << cachep->gfporder);
	struct page *page = virt_to_page((u64)addr);


	if (page->private != cachep->gfporder) {
		caos_printf("kmem_freepages fail: page order error\n");
		return;
	}

	while (i--) {
		// clear slab-field
		page->slab = NULL;
		// clear flag-field
		ClearPageSlab(page++);
	}

	free_pages((u64)addr, cachep->gfporder);

}


//
// create initial cache, cache_cache
// It has cache-descriptors for various size caches.
//
void kmem_cache_init(void)
{
	size_t page_order;

	size_t table_size;	
	struct kmem_cache *tmp;
	size_t csize;
	size_t pcnt;

	// number of pages to store cache_cache
	page_order = CACHE_CACHE_PAGE;	// order-size
	pcnt = 1<<page_order;

	// max number of caches 
	// eg) if page_order = 1 and sizeof(struct kmem_cache) = 64,
	// (8192/64) = 128 caches are available.
	cache_cache_max = (1<<page_order)*PAGE_SIZE/sizeof(struct kmem_cache);


	// allocate pages for cache_cache
	cache_page = alloc_pages(0, page_order);
	
	
	// linear address of cache_cache table
	cache_cache = (struct kmem_cache *)page_to_virt(cache_page);


	caos_memset(cache_cache, 0x0, pcnt*PAGE_SIZE);


	// set page-flag that the page is used for slab
	while (pcnt--)
		SetPageSlab(&cache_page[pcnt]);

	
	// smallest cache size
	csize = KMALLOC_SMALL_SIZE;

	// default cache descriptors - 16~1024 byte caches
	for (table_size = 0; table_size < sizeof(cache_names)/sizeof(char *); table_size++) {
		
		tmp = kmem_cache_create(cache_names[table_size], csize, X86_64_CACHE_ALIGN, 0);
		csize *= 2;

		// fail to create cache
		if (tmp == NULL) {
			caos_printf("\n\nERROR: Memory management is failed!\n\n");
			halt();
		}
	
	}

	caos_printf("cache_cache created at <%x>\n", cache_cache);


}


//
// architecture of slab pages
//
// offset from page boundary to where		: item
// 0 			~ sizeof(slab) 				: struct slab
// sizeof(slab) ~ kmem_cache.colour_off 	: array of kmem_bufctl_t
// kmem_cache.colour_off ~ end of slab pages : object
//


//
// Create a cache
//
// @name: A string to idnetify cache
// @size: The size of objects
// @align: The required alignment for the objects
// @flags: SLAB flags
//
struct kmem_cache *kmem_cache_create(const char *name, size_t size, size_t align, flag_t flags)
{
	struct kmem_cache *cachep = cache_cache;
	size_t cache_index;
	size_t page_need, page_order;
	size_t obj_offset;

	size_t obj_num, bufctl_size;


	if (name == NULL || size < BYTES_PER_LONG || size > KMALLOC_MAX_SIZE) {
		caos_printf("FAIL TO CREATE SLAB, <%s>\n", name);
		return NULL;
	}

	cache_index = 0;


	// not duplicated?
	while (cache_cache[cache_index].name != NULL && cache_index < cache_cache_max) {

		// search a cache that has the same name
		if (caos_strcmp(cache_cache[cache_index].name, name) == 0) {
			caos_printf("kmem_cache_create: duplicate cache %s\n", name);
			return NULL;
		}

		cache_index++;

	}

	// no free cache_cache
	if (cache_index >= cache_cache_max) {
		caos_printf("kmem_cache_create: cache_cache table is full\n");
		return NULL;
	}


	// new cache is allocated from cache_cache table
	cachep = &cache_cache[cache_index];

#ifdef DEBUG
	caos_printf("index=%d  ", cache_index);
#endif

	// object size must be aligned with processor-word size
	if ((size & (BYTES_PER_LONG-1)) != 0) {
		size += (BYTES_PER_LONG-1);
		size &= ~(BYTES_PER_LONG-1);
	}

	if ((align & (BYTES_PER_LONG-1)) != 0) {
		align += (BYTES_PER_LONG-1);
		align &= ~(BYTES_PER_LONG-1);
	}

	
#ifdef DEBUG
	caos_printf("name:%s size=%d align=%d\n", name, size, align);
#endif


	// setup cache_cache
	cachep->name = name;
	cachep->objsize = size;
	cachep->flag = flags;
	INIT_LIST_HEAD(&cachep->slabs);


	// pages for storing slabs
	// In slab-page, slab structure and kmem_bufctl_t array and objects are stored.
	page_need = (sizeof(struct slab) + (size * SLAB_DEFAULT_NUM) + \
			(sizeof(kmem_bufctl_t) * SLAB_DEFAULT_NUM) ) / PAGE_SIZE;


	// order of request page count
	if (page_need == 0)
		page_order = 1;
	else
		page_order = PAGE_ORDER(page_need);

#ifdef DEBUG
	caos_printf("need:%d order=%d ", page_need, page_order);
#endif

	// number of slab-pages
	cachep->gfporder = page_order;


	//
	// The organization of slab-pages
	//
	// obj_num objects can be store in slab-pages,
	// but kmem_bufctl_t array occufies space for objects.
	// Therefore the number of objs in slab-pages is 
	// (pages size - slab size - bufctl size) / obj size
	//
	
	// 1. approximate number of objs that one slab can have (if no bufctl)
	obj_num = ((1<<page_order)*PAGE_SIZE - sizeof(struct slab)) / cachep->objsize;
	// 2. maximum number of bufctl, actual bufctl is less than this
	bufctl_size = obj_num * sizeof(kmem_bufctl_t);	// size of kmem_bufctl_t array
	
	// 3. obj number considering bufctl
	obj_num -= (bufctl_size/size + 1);
	cachep->num = obj_num;

	// 4. objs start at next of bufctl
	obj_offset = sizeof(struct slab) + bufctl_size;
	if ( (obj_offset & (align-1)) != 0) {
		obj_offset += (align-1);
		obj_offset &= ~(align-1);
	}
	cachep->colour_off = obj_offset;

#ifdef DEBUG
	caos_printf("bufctl_num=%d cache_num=%d off=%d\n", bufctl_size/sizeof(kmem_bufctl_t),cachep->num, cachep->colour_off);
#endif

	return cachep;
	
}






//
// Create slabs involved to a cache
//
// @cachep: cache that needs slabs
// @flags: SLAB flags
//
size_t kmem_cache_grow(struct kmem_cache *cachep, flag_t flags)
{

	struct slab *slabp;
	size_t i;

	slabp = (struct slab *)kmem_getpages(cachep, flags);

	if (slabp == NULL)
		return 0;


	//
	// init slab structure
	//
	
	// the number of busy obj
	slabp->inuse = 0;

	// offset of the first obj from the base of page
	slabp->offset = cachep->colour_off;

	// slab is just followed by bufctl array.
	slabp->bufctl = (kmem_bufctl_t *)(slabp+1);

	// pointer to the first obj
	slabp->s_mem = ((u64)slabp + slabp->offset);

	// cache including this slab
	slabp->cache = cachep;

	// bufctl element is the index of next free object
	for (i=0; i < cachep->num; i++) {
		(slabp->bufctl)[i] = i+1;
	}
	// signature of last bufctl
	(slabp->bufctl)[i-1] = BUFCTL_END;
	
	// index of the first free object
	slabp->free = 0;

	// add slab into slab-list of cache
	INIT_LIST_HEAD(&slabp->list);

	// cache can have many slabs
	list_add(&slabp->list, &cachep->slabs);

#ifdef DEBUG
	caos_printf("slabp->0x%x, offset=0x%x, free=%d s_mem=0x%x\n", 
			slabp, slabp->offset, slabp->free, slabp->s_mem);
#endif

	return 1;

}


//
// decrease slabs which has no busy objs
//
// @cachep: cache that have free slabs
// @flags: SLAB flags
//
void kmem_cache_shrink(struct kmem_cache *cachep, flag_t flags)
{
	struct list_head *slabs_list = &(cachep->slabs);
	struct slab *slabp;

    flags = flags; /* remove compiler warning */

	// travel slab-list
	list_for_each_entry(slabp, slabs_list, list) {
#ifdef DEBUG
		caos_printf("slabs=%x\n", slabp);
#endif
	
		// remove one slab at a time
		if (slabp->inuse == 0) {
			// desctroy one slab
			slab_destroy(cachep, slabp);
			list_del(&(slabp->list));
			break;
		}
		
	}


	return;
}




//
// destrory one slab by free slab-pages
// @cachep: cache including slabs
// @slabp: slab that have no busy objs
//
void slab_destroy(struct kmem_cache *cachep, struct slab *slabp)
{
	
	// free pages slab is in
	caos_printf("destroy=%x\n", (u64)slabp->s_mem - (u64)slabp->offset);

	// s_mem - offset => address of struct slab
	kmem_freepages(cachep, (void *)((u64)slabp->s_mem - (u64)slabp->offset));

}




//
// memory allocation in byte size
// @size: requesting size
// @flags: memory attributes, default=0
//
void *caos_kmalloc(size_t size, flag_t flags)
{
	struct kmem_cache *cachep;
	struct slab *slabp;
	struct list_head *slabs_list;
	size_t retry_count = 0;

	size_t cache_index;

	u64 obj;


	cache_index = 0;

	// seek proper cache which has bigger object size than the request
	while (cache_cache[cache_index].name != NULL && cache_index < cache_cache_max) {

		if (size <= cache_cache[cache_index].objsize) {
			break;
		}

		cache_index++;
	}


	// no proper cache
	if (cache_cache[cache_index].name == NULL || cache_index >= cache_cache_max) {
		caos_printf("FAIL TO caos_kmalloc(%d), <no cache>\n", size);
		goto fail_kmalloc;
	}
	



retry:
	// find slab
	cachep = &cache_cache[cache_index];
	slabs_list = &(cachep->slabs);


	//
	// If memory allocation is failed, 
	// it can wait for free memory in consequence of 
	// slab supplement or process termination and etc.
	//
	
	if (retry_count > KMALLOC_RETRY_MAX) {
		caos_printf("FAIL TO caos_kmalloc(%d), <kmem_cache_grow fails>\n", size);
		goto fail_kmalloc;
	}


	// travel slab-list, find a slab which has free object
	list_for_each_entry(slabp, slabs_list, list) {
		if (slabp->free != BUFCTL_END && slabp->inuse < cachep->num) {
			break;
		}
	}

	
	// no slab is found, create new slab-pages
	if (slabs_list == &(slabp->list)) {

		// add a new slab to the cache
		kmem_cache_grow(cachep, flags);

		retry_count++;

		// Adding new slab can takes long time, 
		// so that other processes can be scheduled.
		// Therefore other processes occupies new created slab
		// and still no slab is available.
		// In that case, it tries other caches which has bigger object size.
		if (retry_count > 1)
			cache_index++, retry_count=0;
		
		goto retry;
	}


	obj = slabp->s_mem + slabp->free * cachep->objsize;

	slabp->inuse++;

	// index to next free object
	slabp->free = (slabp->bufctl)[slabp->free];


#ifdef DEBUG
	caos_printf("kmalloc: s_mem=0x%x, inuse=%d obj=0x%x\n", slabp->s_mem, slabp->inuse, obj);

	do {
		int i;
		caos_printf("bufctl=");
		for(i=0; i<cachep->num;i++) {
			caos_printf("%d ", (slabp->bufctl)[i]);
		}
		caos_printf("\n");

	} while(0);

#endif

	return (void *)obj;



fail_kmalloc:
	return NULL;


}



//
// free allocated memory
// @objp: object address to be freed
//
void caos_kfree(void *objp)
{
	struct slab *slabp;
	struct kmem_cache *cachep;
	size_t obj_index;


#ifdef DEBUG
	caos_printf("FREE=%x ", objp);
#endif


	// every page-descriptor has slab field
	// to descibe what slab the page is included
	slabp = virt_to_page((u64)objp)->slab;

	if (slabp == NULL) {
		caos_printf("caos_free error: page structure error\n");
		return;
	}


#ifdef DEBUG
	caos_printf("slab->%x \n", slabp);
#endif

	cachep = slabp->cache;
	
	if (cachep == NULL) {
		caos_printf("caos_free error: broken slab\n");
		return;
	}

	if (slabp->inuse <= 0) {
		caos_printf("caos_free error: free non-allocated object\n");
		return;
	}


	// modify slab descriptor
	obj_index = ((u64)objp - (u64)(slabp->s_mem)) / cachep->objsize;

	// freed object is the latest free object
	(slabp->bufctl)[obj_index] = slabp->free;
	slabp->free = obj_index;
	slabp->inuse--;

#ifdef DEBUG
	caos_printf("slab: objindex=%d inuse=%d, free=%d\n", obj_index, slabp->inuse, slabp->free);

	do {
		int i;
		caos_printf("bufctl=");
		for(i=0; i<cachep->num;i++) {
			caos_printf("%d ", (slabp->bufctl)[i]);
		}
		caos_printf("\n");

	} while(0);
#endif


	// write thrash signature
	//caos_memset(objp, 0xa5, cachep->objsize);

}










