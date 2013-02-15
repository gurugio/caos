
[bits 64]
[CPU X64]
[DEFAULT REL]




extern ignore_isr_handler
extern handle_irq




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






%macro IRQ_ENTRY 2
	align 8
	global %1
%1:
	SAVE_ALL

	mov rdi, %2
	call handle_irq
	
	RESTORE_ALL
	
	iretq
%endmacro










section .text



	

;-----------------------------------------------------

	IRQ_ENTRY _irq_0, 0
	IRQ_ENTRY _irq_1, 1
	IRQ_ENTRY _irq_2, 2
	IRQ_ENTRY _irq_3, 3
	IRQ_ENTRY _irq_4, 4
	IRQ_ENTRY _irq_5, 5
	IRQ_ENTRY _irq_6, 6
	IRQ_ENTRY _irq_7, 7
	IRQ_ENTRY _irq_8, 8
	IRQ_ENTRY _irq_9, 9
	IRQ_ENTRY _irq_10, 10
	IRQ_ENTRY _irq_11, 11
	IRQ_ENTRY _irq_12, 12
	IRQ_ENTRY _irq_13, 13
	IRQ_ENTRY _irq_14, 14
	IRQ_ENTRY _irq_15, 15
	IRQ_ENTRY _irq_16, 16
	IRQ_ENTRY _irq_17, 17
	IRQ_ENTRY _irq_18, 18
	IRQ_ENTRY _irq_19, 19
	IRQ_ENTRY _irq_20, 20
	IRQ_ENTRY _irq_21, 21
	IRQ_ENTRY _irq_22, 22
	IRQ_ENTRY _irq_23, 23
	IRQ_ENTRY _irq_24, 24
	IRQ_ENTRY _irq_25, 25
	IRQ_ENTRY _irq_26, 26
	IRQ_ENTRY _irq_27, 27
	IRQ_ENTRY _irq_28, 28
	IRQ_ENTRY _irq_29, 29
	IRQ_ENTRY _irq_30, 30
	IRQ_ENTRY _irq_31, 31




	;-----------------------------
	; Common ISR for unused interrupt
	;-----------------------------
	align 8
	global _ignore_isr
_ignore_isr:

	SAVE_ALL

	mov rdi, 0xff
	call ignore_isr_handler
	;call handle_irq


	RESTORE_ALL



	;=================================================
	; !! VERY IMPORTANT !!! 
	; Interrupt handlers must be end with 'iretq'!!
	;=================================================
	iretq



