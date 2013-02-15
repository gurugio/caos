
#ifndef __EXCEPT_HANDLER_H__
#define __EXCEPT_HANDLER_H__

#include <types.h>


//
// processor exception handlers that are invoked by assembly routines
//
void do_divide_error(int err, u64 rip);
void do_debug(int err, u64 rip);
void do_nmi(int err, u64 rip);
void do_int3(int err, u64 rip);	
void do_overflow(int err, u64 rip);
void do_bounds(int err, u64 rip);
void do_invalid_op(int err, u64 rip);
void do_device_not_available(int err, u64 rip);
void do_doublefault(int err, u64 rip);
void do_coprocessor_segment_overrun(int err, u64 rip);
void do_invalid_TSS(int err, u64 rip);
void do_segment_not_present(int err, u64 rip);
void do_stack_segment(int err, u64 rip);
void do_general_protection(int err, u64 rip);
void do_page_fault(int err, u64 rip);
void do_coprocessor_error(int err, u64 rip);
void do_alignment_check(int err, u64 rip);
void do_machine_check(int err, u64 rip);
void do_simd_coprocessor_error(int err, u64 rip);



#endif
