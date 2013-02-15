

%include "kernel_section.inc" ; section address of kernel image


KERNEL_CODE_SEL		equ 0x08
KERNEL_DATA_SEL		equ 0x10
KERNEL_CODE64_SEL 	equ 0x18
KERNEL_DATA64_SEL 	equ 0x20

GDT_ADDR        equ 0x8000
GDT_SIZE 		equ 0x1000 ; one page frame


PML4_addr equ 0x90000     ; PML4
PDP_addr equ 0x91000
PDE_addr equ 0x92000
PTE_addr equ 0x93000

BIT_CR4_PAE equ 5
BIT_CR4_PGE equ 7

BIT_CR3_PWT equ 3
BIT_CR3_PCD equ 4

BIT_CR0_PE equ 0 	; Protection Enable
BIT_CR0_PG equ 31 	; Paging
BIT_CR0_CD equ 30 	; Cache Disable
BIT_CR0_NW equ 29 	; Not Write-through

BIT_EFER_LME equ 8
BIT_EFER_LMA equ 10
BIT_EFER_SCE equ 0


HIGH_MEM_ADDR equ 0xFFFF800000000000
;KERNEL_IMAGE_VIRT_ADDR equ HIGH_MEM_ADDR+0x100000
BOOTING_PARAMS_ADDR equ 0x9F000

; Only 0~8MB space is used for kernel booting
; before paging is re-initialized in kernel
BOOTING_MEMORY_SIZE equ 8 	


[bits 16]
[org 0x9000]

	align 8
	
	jmp 0x0:setup_start

setup_start:


    mov ax, cs        ; cs=ds=es=0x0
    mov ds, ax
    mov es, ax



    ; tell BIOS that operating mode will be Long Mode
    mov eax, 0xec00
    mov ebx, 2
    int 0x15



    mov ax, 0xb800
    mov es, ax
    mov bx, 0x2
    mov al, byte [setup_msg]
    mov byte [es:bx], al
    
	
	; disable cache to build GDT stably
    mov eax, cr0
	or eax, (0x1<<BIT_CR0_CD)
    mov cr0, eax


	; clear GDT 
	mov di, GDT_ADDR
    xor ax, ax
    mov cx, 4096/2 ; 7 page frames
    cld
    rep stosd 	; clear 4byte at once



	; build GDT
    lea si, [gdt]    ; source : ds:si=0x0:gdt
    xor ax, ax
    mov es, ax        ; destination : es:di=0x0:0x1000
    mov di, GDT_ADDR
    mov cx, 8*5    ; 7 descriptors

    rep movsb

    cli
    lgdt [gdtr]



	; enable cache in write-back policy
    mov eax, cr0
	and eax, ~(0x1<<BIT_CR0_CD)
	and eax, ~(0x1<<BIT_CR0_NW)
	; activate protection-mode
    or eax, (0x1<<BIT_CR0_PE)
    mov cr0, eax


	; jump to 32bit instructions
    jmp dword KERNEL_CODE_SEL:protect_start



[bits 32]
	align 8
protect_start:
    mov ax, KERNEL_DATA_SEL
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax

    mov edi, 0xb8004
    mov al, byte [protect_msg]
    mov byte [edi], al





    ; temporary stack for booting process
    mov esp, 0xa0000



    ;set PAE bit in CR4
    mov eax, cr4
    or eax, (0x1<<BIT_CR4_PAE) 
    mov cr4, eax

	;
	; code to clear space of Paging-tables should be added here!!!
	;
    ; 0x90000 ~ 0x97000
    mov edi, 0x90000
    xor eax, eax
    mov ecx, 7*4096 ; 7 page frames
    cld
    rep stosd 	; clear 4byte at once




	; temporary memory mapping
	; physical 0~8MB => linear 0~8MB (direct mapping)
	; physical 0~8MB => linear 0xFFFF800000000000 ~ 0xFFFF800000800000 (kernel area)


    ; set PML4[0] to PDP address
    mov edi, PML4_addr
    mov eax, PDP_addr
    or eax, 0x7            ; User, Writable, Present
    mov [es:edi], eax

	; set PML4[128] to PDP address
	; kernel resist at linear address 0xFFFF 8000 0010 0000
	add edi, (8*0x100)	; 0x100th entry 
	mov [es:edi], eax


    ; set PDP[0] to PDE address
    mov edi, PDP_addr
    mov eax, PDE_addr
    or eax, 0x7
    mov [es:edi], eax


    ; set the PDE[3:0] to 0~8M

    mov edi, PDE_addr
    mov eax, PTE_addr+0x7
    mov ecx, (BOOTING_MEMORY_SIZE/2) ; 4 PTEs = 2MB per a PTE * 4
set_PDE:
    mov [es:edi], eax
    add eax, 0x1000
    add edi, 8
    dec ecx
    jnz set_PDE

    ; set 4 PTEs to 0~8M, one PTE covers 2MB

    mov edi, PTE_addr
    mov eax, 0x7
    mov ecx, (BOOTING_MEMORY_SIZE*256) ; 1MB area = 256 page frames
set_PTE:
    mov [es:edi], eax
    add eax, 0x1000
    add edi, 8
    dec ecx
    jnz set_PTE


    ; set CR3 to address of PML4
	; disable Page-level Cache & Write-back caching (in paging level)
	; do not use page-level cache yet, these are initializing code!
    mov eax, PML4_addr
	or eax, (0x1<<BIT_CR3_PWT)
	or eax, (0x1<<BIT_CR3_PCD)
    mov cr3, eax






    ; enable long mode by set EFER.LME bit

    ; set MSR 0xc0000080 with RDMSR, WRMSR instruction
    ; read "system programming guide 1 - appendix B"

    mov ecx, 0xc0000080 	; EFER register address
    rdmsr
    ;or eax, 0x00000100
    or eax, (0x1<<BIT_EFER_LME)
    wrmsr


	; activate paging
    mov eax, cr0
    or eax, (0x1<<BIT_CR0_PG)
    mov cr0, eax




	;-----------------------------------------------------
	; processor_sig value indicates awaken-processor count.
	;mov al, byte [processor_sig]
	;add al, 1
	;mov byte [processor_sig], al
	;-----------------------------------------------------


    
	; long jump is necessary
    jmp KERNEL_CODE64_SEL:long_start



setup_msg    db "a", 0
protect_msg    db "O", 0

processor_sig	db 0


gdtr:
    dw GDT_SIZE
    dd GDT_ADDR


gdt:
    dd 0x00000000, 0x00000000
    dd 0x0000FFFF, 0x00CF9A00 ; kernel code segment for protected mode
    dd 0x0000FFFF, 0x00CF9200 ; kernel data segment for protected mode
    dd 0x0000ffff, 0x00AF9A00 ; longmode kernel code segment, CS.L=1, CS.D=0
    dd 0x0000FFFF, 0x00AF9200 ; longmode kernel data segment





[bits 64]
	align 8
long_start:
    

	; test LMA((Long Mode Active) bit in EFER
    mov ecx, 0xc0000080
    rdmsr
    and eax, (0x1<<BIT_EFER_LMA)
	jnz longmode_ok
	
	jmp longmode_fail


longmode_ok:






    ; set PAE,PGE bit in CR4
	; Writing cr4 is to flush TLB.
    mov rax, cr4
    or rax, (0x1<<BIT_CR4_PAE|0x1<<BIT_CR4_PGE) 
    mov cr4, rax


	; set SCE(System Call Enable) bit in EFER
    mov ecx, 0xc0000080
    rdmsr
    or eax, (0x1<<BIT_EFER_SCE)
    wrmsr



	; only FS, GS are used in longmode, others are ignored

	mov ax, 0
	mov ds, ax
	mov es, ax
	mov ss, ax
	mov fs, ax
	mov gs, ax



	; if AP is executing here, do not copy kernel again.
	mov al, byte [processor_sig64]
	cmp al, 0
	jne copy_kernel_end



    ; clear Kernel area
    ; 0x2000 ~ 0x2FFF is allocated for TSS
    mov rdi, 0x100000
    xor rax, rax
    mov rcx, 7*1024*1024/8 ; area 1MB ~ (8MB-1), 7MB size
    cld
    rep stosq 	; clear 8byte at once

	

	; copy kernel-image at 0x100000 (1MB)
	mov rax, KSIZE
	mov rdx, 512
	mul rdx 	; edx:eax = kernel size

	mov rcx, rax
	mov rsi, 0xA000
	mov rdi, 0x100000
copy_kernel:
	mov rax, qword [rsi]
	mov qword [rdi], rax
	add rsi, 8
	add rdi, 8
	sub rcx, 8
	jnz copy_kernel

copy_kernel_end:


	; BSP's stack => KERNEL_STACK_START
	; AP's stack => KERNEL_STACK_START - 2 pages
    mov rsp, KERNEL_STACK_START

	mov al, byte [processor_sig64]
	cmp al, 0
	je set_stack_end

set_ap_stack:
	mov rsp, KERNEL_STACK_START - 0x2000

set_stack_end:






	; check stack area access
	mov rcx, 0x10
	mov rax, 0x12345678ABCDEF00
fill_stack:	
	push rax
	sub rcx, 1
	jnz fill_stack
	


    mov rdi, 0xb8006
    mov al, byte [long_msg]
    mov byte [rdi], al

	

	; booting paramters stored in memory will be passed to kernel
	mov rdi, BOOTING_PARAMS_ADDR

	; start code
	mov rax, _code_start
	mov qword [rdi], rax

	; end code
	add rdi, 8
	mov rax, _code_end
	mov qword [rdi], rax

	; start data
	add rdi, 8
	mov rax, _data_start
	mov qword [rdi], rax

	; end data
	add rdi, 8
	mov rax, _data_end
	mov qword [rdi], rax

	; start bss
	add rdi, 8
	mov rax, _bss_start
	mov qword [rdi], rax
	
	; end bss
	add rdi, 8
	mov rax, _bss_end
	mov qword [rdi], rax
	
	; end kernel
	add rdi, 8
	mov rax, _kernel_end
	mov qword [rdi], rax


	; MSIZE: physical memory size defined at Makefile
	add rdi, 8
	mov rax, MSIZE
	mov qword [rdi], rax

	; boot GDT
	add rdi, 8
	mov rax, GDT_ADDR
	mov qword [rdi], rax

	; boot memory size
	add rdi, 8
	mov rax, BOOTING_MEMORY_SIZE
	mov qword [rdi], rax

	; start low-BIOS
	add rdi, 8
	mov rax, 0x9F000
	mov qword [rdi], rax

	; end low-BIOS
	add rdi, 8
	mov rax, 0x100000
	mov qword [rdi], rax

	; start per-cpu section
	add rdi, 8
	mov rax, _per_cpu_start
	mov qword [rdi], rax

	; end per-cpu section
	add rdi, 8
	mov rax, _per_cpu_end
	mov qword [rdi], rax



	; rdi is first argument of start_kernel() function.
	; The first argument of start_kernel() is count of processors,
	; so that the kernel can know which processor is executing kernel code.
	mov rax, qword [processor_sig64]
	mov rdi, rax

	;-----------------------------------------------------
	; processor_sig64 value indicates awaken-processor count.
	mov rax, qword [processor_sig64]
	add rax, 1
	mov qword [processor_sig64], rax
	;-----------------------------------------------------


	; long-jump to kernel area

	;
	; jmp 0xFFFF800000100000 does not work!! I don't know why.
	; So I use iretq instruction

	;
	; iretq command needs stack like this
	; | SS      | = 0x20
	; | RSP     | = 
	; | EFLAGS  |
	; | CS      |
	; | RIP     |
	mov rax, _code_start 		; code section base address
	mov rbx, KERNEL_CODE64_SEL
	mov rcx, 0
	mov rdx, rsp
	mov rsi, 0

	push rsi
	push rdx
	push rcx
	push rbx
	push rax

	; rip <= 0xFFFF800000100000
	iretq


	

longmode_fail:
	mov rdi, 0xb8000
	mov al, 'X'
	mov byte [rdi], al

    jmp $


	align 8
long_msg    db "L", 0
	align 8
memory_size dq 0


	align 8
; ap is ok?
processor_sig64	dq 0




times 4096-($-$$) db 0
