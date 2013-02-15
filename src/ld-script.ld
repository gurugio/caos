OUTPUT_FORMAT(elf64-x86-64)
ENTRY(start_kernel)
SECTIONS
{
    . = KERNEL_VMA;

    . = ALIGN(4096);
    .text : AT(ADDR(.text) - KERNEL_VMA)
    {
        _code_start = .;
        *(.text)
        *(.rodata*)
    }

	_code_end = .;

   . = ALIGN(4096);
   .data : AT(ADDR(.data) - KERNEL_VMA)
   {
        _data_start = .;
	    *(.data)
		
		_per_cpu_start = .;
		*(.data.percpu)
		_per_cpu_end = .;
   }
 
	_data_end = .;



   . = ALIGN(4096);
   .bss : AT(ADDR(.bss) - KERNEL_VMA)
   {
       _bss_start = .;
       *(.bss)
   }

   _bss_end = .;

   _kernel_end = .;

   /DISCARD/ :
   {
        *(.comment)
   }

   . = KERNEL_STACK_START;
}
