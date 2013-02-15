; 2009.1.20 gurugio

;====================================================
; FILE NAME: bootsect.asm
;	nasm -o boot.img bootsect.asm
;====================================================


[bits 16]

[org 0x0]

BOOT_SEG	equ 0x7c0

	jmp BOOT_SEG:start


start:
	mov ax, cs
	mov ss, ax
	mov sp, 0x0


	; STEP 1. check CPUID is available

	; eflag 의 21번 비트가 CPUID 명령어를 지원하는지를 나타내는 비트가
	; 이 비트의 값을 소프트웨어가 소프트웨어가 반전할 수 있으면 CPUID 명령을 지원함
	pushf
	pop eax
	mov ebx, eax
	xor eax, 0x200000 	; 반전
	push eax
	popf
	pushf
	pop eax
	
	mov edx, 0x11111111
	jz no_longmode

	mov ax, 0xb800
	mov es, ax
	mov bx, 0x0
	mov al, '1'
	mov byte [es:bx], al

	;-----------------------------
	; STEP 2. Dual Processor

	mov eax, 0x1
	cpuid
	mov ecx, eax

	; eax[13:12] : Processor Type
	; 10 = Dual Processor
	and eax, 0x3000
	xor eax, 0x200000
	
	mov edx, 0x22222222
	jz no_longmode


	; edx[3]: Page size Extension
	; edx[5]: Model Specific Registers
	; edx[6]: Physical Address Extension
	; edx[13]: Page Global Enable
	and edx, (0x1<<3 | 0x1<<5 | 0x1<<6 | 0x1<<13)

	mov edx, 0x33333333
	jz no_longmode

	mov ax, 0xb800
	mov es, ax
	mov bx, 0x2
	mov al, '2'
	mov byte [es:bx], al
	
	;------------------------------
	; STEP 3. Extended CPUID functions is available

	mov eax, 0x80000000
	cpuid

	; eax: the largest extended function number
	; 최소한 0x80000001 은 지원되야 함
	cmp eax, 0x80000001
	mov edx, 0x33333333
	jb no_longmode


	mov ax, 0xb800
	mov es, ax
	mov bx, 0x4
	mov al, '3'
	mov byte [es:bx], al
	

	;--------------------------------
	; STEP 4. INTEL 64 instruction setup

	mov eax, 0x80000001
	cpuid
	and edx, (0x1<<29)

	mov edx, 0x44444444
	jz no_longmode

	
	mov ax, 0xb800
	mov es, ax
	mov bx, 0x6
	mov al, '4'
	mov byte [es:bx], al


	;; 화면 첫줄에  1234가 출력되야 함
	;;

	jmp $


no_longmode:

	mov ax, 0xb800
	mov es, ax
	mov bx, 0x0
	mov al, 'X'
	mov byte [es:bx], al
	
	jmp $
	


times 510-($-$$) db 0
	dw 0xaa55



