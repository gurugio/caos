

#include <screen.h>
#include <printf.h>
#include <setup.h> // setup_arch()
#include <processor.h> // cli()
#include <types.h>
#include <memory.h> // contig_page_data
#include <page_alloc.h>	// alloc_pages()
#include <slab.h>	// kmem_cache_init(), kmalloc
#include <mp_spec.h> // init_bsp_APIC, wake_ap
#include <apic.h>
#include <smp.h>
#include <keyboard.h>
#include <bitops.h>
#include <sched.h>	// mm_struct
#include <percpu.h>	// 
#include <irq_handler.h> // irq_init
#include <pic8259.h> // pic8259_init
#include <io_apic.h> // io_apic_init

void start_kernel(size_t index)
{

    //
    // step 1. disable interrupts for system initialization
    //
	cli();
	

    // checking CPU working
	if (index == 0) {
		init_screen();
		caos_printf("Hello! I am BSP!!\n");
	} else {
		caos_printf("Hello!! I am AP!! I am running??\n");
		while (1) {
			screen_info.video_mem[79]++;

		}
	}

	
	// 6 arguments are stored in register,
	// and others are stored in stack memory.
	// Therefore if there is not type-specifier,
	// compiler cannot know how many byte is valid data in stack.
	// If arguments 6UL, 7UL is 6, 7 without type-specifier,
	// output data is the thrash data from stack with 6 at al or dl register.
	caos_printf("Hello, CaOS. Test printf <%d,%d,%d,%d,%d,%d,0x%x>\n", 0, 1, -3U, -4L, -5, 6UL, 0xaUL);	// variable list

	caos_printf("size int=%d, long=%d, long long=%d\n", sizeof(int), sizeof(long), sizeof(long long));


	{
		u64 *entry = (u64 *)0xFFFF800000090000;

		caos_printf("PML4[0]=%x, PDP[0]=%x\n PDE[0]=%x, PTE[0]=%x\n",
				*entry, *(entry+512), *(entry+1024), *(entry+1536));
	}


    //
    // step 2. processor
    //
        
	//
	// Booting parameters
	// GDT, IDT
	// Paging-Tables
	// memory layout, page management
    //
    // Linear address is enabled after setup_arch()
	//
	setup_arch();


    //
    // step 3. memory management
    //
    
	// setup zones/nodes of system
	// Now page allocator is available.
	memory_init(phy_mem_size);

	//
	// setup memory allocator
	//
	kmem_cache_init();



    //
    // step 4. interrupt
    //

    //
    // external IRQs
    // current IRQs are accepted via 8259A,
    // but I/O APIC will be used instead.
    //

    // init local APIC of BSP processor
	init_bsp_APIC();

    //pic8259_init();
    io_apic_init(); // not implemented yet
    
	irq_init();	

    


    

    //
    // step 5. multi-processor
    //

	// prepare smp execution
	smp_init();

	// BSP's local timer
	init_bsp_timer();

	// wake AP (second processor)
	wake_ap();



    //
    // step 6. process management
    //
	caos_printf("\n### MAKE PROCESS MANAGEMENT!! ###\n");



    //
    // step 7. devices, shell
    //

	keyboard_init();


	// kernel booting ends...ready to run init-thread
	sti();


	while(1) ;
}




