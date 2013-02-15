#include <printf.h>
#include <types.h>
#include <sched.h>

#include <percpu.h>
#include <setup.h>	// init_mm
#include <page.h>	// PAGE_SIZE
#include <page_alloc.h>	// alloc_ages
#include <string.h>		// memcpy
#include <processor.h>	// NR_CPUS



#define DEBUG 1
#undef DEBUG


//
// !!  This must be moved to the CPU-descriptor
//
size_t cpu_offset[5];


//
// The address of per-cpu array.
// The per-cpu array is the NR_CPUS-copies of the .data.percpu section of kernel-image
// If 5 cores are there, 5 copies of .data.percpu are there in per-cpu array.
//
static char *percpu_buf;





/*
 * implementation of linux-2.6.29
 *
#define RELOC_HIDE(ptr, off) \
	({ ssize_t __ptr;\
	 __ptr = (ssize_t)(ptr);\
	 (typeof(ptr))(__ptr + (off)); })

#define per_cpu(var, cpu) (*(RELOC_HIDE(&per_cpu__##var, per_cpu_offset(cpu))))
*/



//
// return cpu-offset
// !!  this must be modified to get cpu-offset from the CPU-descriptor
#define per_cpu_offset(cpu) cpu_offset[cpu]

// 
// get the address of per-cpu data in per-cpu array
// by adding cpu-offset to the original address of per-cpu data
//
#define per_cpu(var, cpu) (*((typeof(&per_cpu__##var))((u64)&per_cpu__##var + per_cpu_offset(cpu))))


//
// test data
DEFINE_PER_CPU(long, cpu_id1) = 5;
DEFINE_PER_CPU(long, cpu_id2) = 6;
DEFINE_PER_CPU(long, cpu_id3) = 7;
DEFINE_PER_CPU(long, cpu_id4) = 8;
DEFINE_PER_CPU(long, cpu_id5) = 9;


//
// setup per-cpu array.
// This copies .data.percpu section of the kernel-image NR_CPU times,
// so that per-cpu array has NR_CPUS entries of per-cpu data.
//
void setup_per_cpu_areas(void)
{
	u64 *p = (u64 *)init_mm.start_percpu;
	size_t percpu_size = init_mm.end_percpu - init_mm.start_percpu;
	struct page *percpu_page;
	ssize_t i;


	// test: .data.percpu section is OK?
	caos_printf("per-cpu section: %x ~ %x (%d-bytes)\n", 
			init_mm.start_percpu, init_mm.end_percpu, 
			init_mm.end_percpu-init_mm.start_percpu);
	caos_printf("TEST PER-CPU SECTION: 5 == %d?\n", *p);



	// buffer for per-cpu array 
	percpu_page = alloc_pages(0, PAGE_ORDER((percpu_size/PAGE_SIZE) * NR_CPUS));
	percpu_buf = (char *)page_to_virt(percpu_page);

#if defined(DEBUG)
	// test: per-cpu buffer is OK?
	caos_printf("order->%d\n", PAGE_ORDER((percpu_size/PAGE_SIZE) * NR_CPUS));
	caos_printf("%d-pages at %x\n", percpu_page->private, percpu_buf);
#endif

	//
	// copy .data.percpu section NR_CPUS-times
	//

	//for (i=0; i<NR_CPUS; i++) {
	for (i=0; i<5; i++) {	// test

		// copy section to per-cpu buffer
		caos_memcpy(percpu_buf, (void *)init_mm.start_percpu, percpu_size);
		// offset from .data.percpu section to copy data.
		cpu_offset[i] = (u64)percpu_buf - init_mm.start_percpu;
		// next entry in per-cpu array
		percpu_buf += percpu_size;
#if defined(DEBUG)
		// test: cpu-offset is OK?
		caos_printf("cpu-offset %d: %x\n", i, cpu_offset[i]);
#endif
	}
	
#if defined(DEBUG)
	// test: per_cpu macro is OK?
	caos_printf(">>> %x\n", (u64)&per_cpu__cpu_id1 + cpu_offset[0]);
	caos_printf("cpu0 id1 5=%x, %x\n", per_cpu(cpu_id1, 0), &per_cpu(cpu_id1, 0));
	caos_printf("cpu0 id2 6=%x, %x\n", per_cpu(cpu_id2, 0), &per_cpu(cpu_id2, 0));
	caos_printf("cpu0 id3 7=%x, %x\n", per_cpu(cpu_id3, 0), &per_cpu(cpu_id3, 0));
	caos_printf("cpu0 id4 8=%x, %x\n", per_cpu(cpu_id4, 0), &per_cpu(cpu_id4, 0));
	caos_printf("cpu0 id5 9=%x, %x\n", per_cpu(cpu_id5, 0), &per_cpu(cpu_id5, 0));
	caos_printf("cpu1 id1 5=%x, %x\n", per_cpu(cpu_id1, 1), &per_cpu(cpu_id1, 1));
	caos_printf("cpu2 id1 5=%x, %x\n", per_cpu(cpu_id1, 2), &per_cpu(cpu_id1, 2));
#endif


}










