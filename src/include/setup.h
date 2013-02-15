
#ifndef _SETUP_H
#define _SETUP_H 1


#include <sched.h>
#include <types.h>
#include <pgtable.h>
#include <processor.h>



#define BOOT_PARAMS (u64 *)(0x9F000+PAGE_OFFSET)

#define PARAMS_OFFSET_START_CODE 	0
#define PARAMS_OFFSET_END_CODE 		1
#define PARAMS_OFFSET_START_DATA 	2
#define PARAMS_OFFSET_END_DATA 		3
#define PARAMS_OFFSET_START_BSS 	4
#define PARAMS_OFFSET_END_BSS 		5
#define PARAMS_OFFSET_END_KERNEL 	6
#define PARAMS_OFFSET_PHYMEM_SIZE 	7
#define PARAMS_OFFSET_BOOT_GDT 		8
#define PARAMS_OFFSET_BOOT_SIZE 	9
#define PARAMS_OFFSET_START_VIDEO 	10
#define PARAMS_OFFSET_END_VIDEO 	11
#define PARAMS_OFFSET_START_PERCPU 	12
#define PARAMS_OFFSET_END_PERCPU 	13



void setup_arch(void);
void early_identify_cpu(struct cpuinfo_x86 *);


extern struct mm_struct init_mm;
extern size_t phy_mem_size;



#endif
