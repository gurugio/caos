

#ifndef __SLAB_H__
#define __SLAB_H__



#include <types.h>
#include <list.h>
#include <printf.h>



#define SLAB_DEFAULT_NUM 10
#define CACHE_CACHE_PAGE 1

#define KMALLOC_RETRY_MAX 3

#define BUFCTL_END (u16)0xFFFF


typedef u16 kmem_bufctl_t;


//
// struct kmem_cache
//
// manages a cache, which manages slabs.
// A cache size is statically fixed, multiples of 16bytes.
//
struct kmem_cache {
	//size_t limit;	// max # of objs per cache
	size_t objsize;	// size of obj
	flag_t flag;	// flags of cache
	size_t num;		// # of objs per slab

	size_t gfporder;	// # of page per slab
	struct list_head slabs;	// pointer to slab

	u64 colour_off;

	const char *name;
};


//
// struct slab
//
// Manages the objs in a slab. Placed at the beginning of page allocated for a slab.
// A kmem_cache can have list of slabs
//
struct slab {
	struct list_head list;
	u64 s_mem;	// pointer to free object
	size_t inuse;	// busy objects count
	size_t free;	// index of free object
	u64 offset;		// offset of the first object from page boundary
	kmem_bufctl_t *bufctl;

	struct kmem_cache *cache;
};



extern struct kmem_cache *cache_cache;



void kmem_cache_init(void);
struct kmem_cache *kmem_cache_create(const char *name, size_t size, size_t align, flag_t flags);

size_t kmem_cache_grow(struct kmem_cache *cachep, flag_t flags);
void *kmem_getpages(struct kmem_cache *cachep, flag_t flag);
void kmem_freepages(struct kmem_cache *cachep, void *addr);


void slab_destroy(struct kmem_cache *cachep, struct slab *slabp);


void kmem_cache_shrink(struct kmem_cache *cachep, flag_t flags);


void *caos_kmalloc(size_t size, flag_t flags);
void caos_kfree(void *objp);

#endif
