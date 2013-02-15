
[bits 64]
[CPU X64]
[DEFAULT REL]




extern do_divide_error

extern do_divide_error	; 0
extern do_debug			;1
extern do_nmi				;2
extern do_int3			;3
extern do_overflow		;4
extern do_bounds			;5
extern do_invalid_op		;6
extern do_device_not_available	;7
extern do_doublefault_fn			;8
extern do_coprocessor_segment_overrun		;9
extern do_invalid_TSS		;10
extern do_segment_not_present	;11
extern do_stack_segment	;12
extern do_general_protection	;13
extern do_page_fault		;14
extern do_coprocessor_error	;16
extern do_alignment_check		;17
extern do_machine_check		;18
extern do_simd_coprocessor_error	;19



_ERROR 		equ 17*8
_RIP 		equ 18*8
_CS 		equ 19*8



; every general register is 
%macro SAVE_ALL 0
	
	push rax
	push rbx
	push rcx
	push rdx

	push rsi
	push rdi
	push rbp

	push r8
	push r9
	push r10
	push r11
	push r12
	push r13
	push r14
	push r15

	push fs
	push gs

%endmacro


; SAVE_ALL is referred to linux-kernel
%macro RESTORE_ALL 0
	
	pop fs
	pop gs

	pop r15
	pop r14
	pop r13
	pop r12
	pop r11
	pop r10
	pop r9
	pop r8

	;pop rsp
	pop rbp
	pop rdi
	pop rsi

	pop rdx
	pop rcx
	pop rbx
	pop rax


%endmacro



section .text


	global _local_timer_test

	global _test_except
	global _ignore_isr

	
	global _divide_error	; 0
	global _debug			;1
	global _nmi				;2
	global _int3			;3
	global _overflow		;4
	global _bounds			;5
	global _invalid_op		;6
	global _device_not_available	;7
	global _doublefault_fn			;8
	global _coprocessor_segment_overrun		;9
	global _invalid_TSS		;10
	global _segment_not_present	;11
	global _stack_segment	;12
	global _general_protection	;13
	global _page_fault		;14
	; 15th exception is reserved for INTEL
	global _coprocessor_error	;16
	global _alignment_check		;17
	global _machine_check		;18
	global _simd_coprocessor_error	;19



	align 8
_divide_error:

	push 0 		; no error code
	SAVE_ALL

	mov rdi, [rsp+_ERROR]
	mov rsi, [rsp+_RIP]
	call do_divide_error

	RESTORE_ALL
	add rsp, 8

	jmp $


	align 8
_debug:
	push 0 		; no error code
	SAVE_ALL

	mov rdi, [rsp+_ERROR]
	mov rsi, [rsp+_RIP]
	call do_debug


	RESTORE_ALL
	add rsp, 8

	jmp $


	align 8
_nmi:
	push 0 		; no error code
	SAVE_ALL

	mov rdi, [rsp+_ERROR]
	mov rsi, [rsp+_RIP]
	call do_nmi

	RESTORE_ALL
	add rsp, 8

	jmp $


	align 8
_int3:
	push 0 		; no error code
	SAVE_ALL

	mov rdi, [rsp+_ERROR]
	mov rsi, [rsp+_RIP]
	call do_int3

	RESTORE_ALL
	add rsp, 8

	jmp $


	align 8
_overflow:
	push 0 		; no error code
	SAVE_ALL

	mov rdi, [rsp+_ERROR]
	mov rsi, [rsp+_RIP]
	call do_overflow

	RESTORE_ALL
	add rsp, 8

	jmp $



	align 8
_bounds:
	push 0 		; no error code
	SAVE_ALL

	mov rdi, [rsp+_ERROR]
	mov rsi, [rsp+_RIP]
	call do_bounds

	RESTORE_ALL
	add rsp, 8

	jmp $


	align 8
_invalid_op:
	push 0 		; no error code
	SAVE_ALL

	mov rdi, [rsp+_ERROR]
	mov rsi, [rsp+_RIP]
	call do_invalid_op

	RESTORE_ALL
	add rsp, 8

	jmp $



	align 8
_device_not_available:
	push 0 		; no error code
	SAVE_ALL

	mov rdi, [rsp+_ERROR]
	mov rsi, [rsp+_RIP]
	call do_device_not_available

	RESTORE_ALL
	add rsp, 8

	jmp $



	align 8
_doublefault_fn:
	push 0
	SAVE_ALL

	mov rdi, [rsp+_ERROR]
	mov rsi, [rsp+_RIP]
	call do_doublefault_fn

	RESTORE_ALL
	add rsp, 8

	jmp $



	align 8
_coprocessor_segment_overrun:
	push 0 		; no error code
	SAVE_ALL

	mov rdi, [rsp+_ERROR]
	mov rsi, [rsp+_RIP]
	call do_coprocessor_segment_overrun

	RESTORE_ALL
	add rsp, 8

	jmp $



	align 8
_invalid_TSS:
	SAVE_ALL

	mov rdi, [rsp+_ERROR]
	mov rsi, [rsp+_RIP]
	call do_invalid_TSS

	RESTORE_ALL
	add rsp, 8

	jmp $


	align 8
_segment_not_present:
	SAVE_ALL

	mov rdi, [rsp+_ERROR]
	mov rsi, [rsp+_RIP]
	call do_segment_not_present

	RESTORE_ALL
	add rsp, 8

	jmp $


	align 8
_stack_segment:
	SAVE_ALL

	mov rdi, [rsp+_ERROR]
	mov rsi, [rsp+_RIP]
	call do_stack_segment

	RESTORE_ALL
	add rsp, 8

	jmp $


	align 8
_general_protection:
	SAVE_ALL

	mov rdi, [rsp+_ERROR]
	mov rsi, [rsp+_RIP]
	call do_general_protection

	RESTORE_ALL
	add rsp, 8

	iretq


	align 8
_page_fault:
	SAVE_ALL

	mov rdi, [rsp+_ERROR]
	mov rsi, [rsp+_RIP]
	call do_page_fault

	RESTORE_ALL
	add rsp, 8

	jmp $


	align 8
_coprocessor_error:
	push 0 		; no error code
	SAVE_ALL

	mov rdi, [rsp+_ERROR]
	mov rsi, [rsp+_RIP]
	call do_coprocessor_error

	RESTORE_ALL
	add rsp, 8

	jmp $


	align 8
_alignment_check:
	SAVE_ALL

	mov rdi, [rsp+_ERROR]
	mov rsi, [rsp+_RIP]
	call do_alignment_check

	RESTORE_ALL
	add rsp, 8

	jmp $


	align 8
_machine_check:
	push 0 		; no error code
	SAVE_ALL

	mov rdi, [rsp+_ERROR]
	mov rsi, [rsp+_RIP]
	call do_machine_check

	RESTORE_ALL
	add rsp, 8

	jmp $


	align 8
_simd_coprocessor_error:
	push 0 		; no error code
	SAVE_ALL

	mov rdi, [rsp+_ERROR]
	mov rsi, [rsp+_RIP]
	call do_simd_coprocessor_error

	RESTORE_ALL
	add rsp, 8

	jmp $




;-----------------------------------------------------

	align 8
_local_timer_test:
	SAVE_ALL

	

	mov rdi, [rsp+_ERROR]
	mov rsi, [rsp+_RIP]
	call do_machine_check

	;set EOI


	RESTORE_ALL



	iretq



	align 8
_test_except:


	push rbx
	push rbp

	push r12
	push r13
	push r14
	push r15



	int 100


	pop r15
	pop r14
	pop r13
	pop r12
	pop rbp
	pop rbx

	ret


