

#include <printf.h>
#include <except_handler.h>
#include <types.h>
#include <apic.h>



void do_divide_error(int err, u64 rip)	
{
	caos_printf("EXCEPTION: divide_error, error=%x, rip=%x\n", err, rip);

}


void do_debug(int err, u64 rip)
{
	caos_printf("EXCEPTION: debug, error=%x, rip=%x\n", err, rip);
}


void do_nmi(int err, u64 rip)
{
	caos_printf("EXCEPTION: nmi, error=%x, rip=%x\n", err, rip);
}


void do_int3(int err, u64 rip)	
{
	caos_printf("EXCEPTION: int3, error=%x, rip=%x\n", err, rip);
}


void do_overflow(int err, u64 rip)
{
	caos_printf("EXCEPTION: overflow, error=%x, rip=%x\n", err, rip);
}


void do_bounds(int err, u64 rip)
{
	caos_printf("EXCEPTION: bounds, error=%x, rip=%x\n", err, rip);
}


void do_invalid_op(int err, u64 rip)
{
	caos_printf("EXCEPTION: invalid operatioin, error=%x, rip=%x\n", err, rip);
}


void do_device_not_available(int err, u64 rip)
{
	caos_printf("EXCEPTION: device not avalible, error=%x, rip=%x\n", err, rip);
}


void do_doublefault_fn(int err, u64 rip)	
{
	caos_printf("EXCEPTION: doublefault, error=%x, rip=%x\n", err, rip);
}


void do_coprocessor_segment_overrun(int err, u64 rip)	
{
	caos_printf("EXCEPTION: coprocessor segment overrun, error=%x, rip=%x\n", err, rip);
}


void do_invalid_TSS(int err, u64 rip)
{
	caos_printf("EXCEPTION: invalid TSS, error=%x, rip=%x\n", err, rip);
}


void do_segment_not_present(int err, u64 rip)
{
	caos_printf("EXCEPTION: segment not present, error=%x, rip=%x\n", err, rip);
}


void do_stack_segment(int err, u64 rip)
{
	caos_printf("EXCEPTION: stack segment, error=%x, rip=%x\n", err, rip);
}


void do_general_protection(int err, u64 rip)
{
	caos_printf("EXCEPTION: general protection, error=%x, rip=%x\n", err, rip);
}


void do_page_fault(int err, u64 rip)
{
	caos_printf("EXCEPTION: page fault, error=%x, rip=%x\n", err, rip);
}


void do_coprocessor_error(int err, u64 rip)
{
	caos_printf("EXCEPTION: coprocessor error, error=%x, rip=%x\n", err, rip);
}


void do_alignment_check(int err, u64 rip)	
{
	caos_printf("EXCEPTION: alignment check, error=%x, rip=%x\n", err, rip);
}


void do_machine_check(int err, u64 rip)
{
	caos_printf("EXCEPTION: machine check , error=%x, rip=%x\n", err, rip);
}


void do_simd_coprocessor_error(int err, u64 rip)
{
	caos_printf("EXCEPTION: SIMD coprocessor error, error=%x, rip=%x\n", err, rip);
}







