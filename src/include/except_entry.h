

#ifndef __EXCEPT_ENTRY_H__
#define __EXCEPT_ENTRY_H__



//void _local_timer_test(void);

void _test_except(void);
//void _ignore_isr(void);
//void ignore_isr_handler(size_t err, u64 rip);


//
// assembly routines that are invoked by processor exceptions
//
extern void _divide_error(void);
extern void _debug(void);
extern void _nmi(void);
extern void _int3(void);
extern void _overflow(void);
extern void _bounds(void);
extern void _invalid_op(void);
extern void _device_not_available(void);
extern void _doublefault_fn(void);
extern void _coprocessor_segment_overrun(void);
extern void _invalid_TSS(void);
extern void _segment_not_present(void);
extern void _stack_segment(void);
extern void _general_protection(void);
extern void _page_fault(void);
extern void _coprocessor_error(void);
extern void _alignment_check(void);
extern void _machine_check(void);
extern void _simd_coprocessor_error(void);



#endif



