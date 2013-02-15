;====================================================
; DESCRIPTION: 1. load setup.asm at 0x9000
;              2. load kernel at 0x9200
; FILE NAME: bootsect.asm
;====================================================


[org 0x0]


SETUP_SECT	equ 8 ; sutup.img size is 4096 (8 sectors)
KERNEL_SECT	equ KERNEL_SIZE	

BOOT_SEG	equ 0x7c0
SETUP_SEG	equ 0x900

; Kernel is copied to 0xA000 at first,
; and to 0x100000 in setup.asm
KERNEL_SEG	equ 0xA00

SETUP_ADDR	equ 0x9000


	jmp BOOT_SEG:start

boot_drv		db 0	; floppy drive A:
sect_per_track	dw 18	; sectors per one track
head_per_drv	dw 2	; floppy has 2 heads
current_sect	dw 1	; current reading sector # (relative)
read_sect		db 0	; it read how many sectors

; define routines for A20 line activation
%include "a20.inc"

start:
	mov ax, cs
	mov ds, ax		; ds=es=0x7c0
	mov es, ax

	mov ss, ax		; stack: 0x7c0:0 -> low memory
	mov sp, 0x0

	mov [boot_drv], dl	; bios store drive number in dl


	mov ax, SETUP_SEG	; read image of setup.asm
	mov es, ax
	mov si, 0			; store image at es:si=0x900:0
	mov cx, SETUP_SECT	; how many sectors?
	call read_sectors

	mov ax, KERNEL_SEG
	mov es, ax
	mov si, 0
	mov cx, KERNEL_SECT
	call read_sectors

	call a20_try_loop	; activate A20 line


	mov ax, 0xb800
	mov es, ax
	mov bx, 0x0
	mov al, byte [hello_msg]	; Greeting!
	mov byte [es:bx], al
	
	
	mov ax, KERNEL_SECT

	;==========================================
	; segment addres = 0x0
	; offset = 0x9000
	; Segment must be 0x0000.
	;==========================================
	jmp 0x0:SETUP_ADDR	; execute setup


	jmp $				; or die!

; es: segment address
; cx: sector count
read_sectors:
	push cx
	call read_one_sect		; load one sector
	pop cx

	add si, 512				; offset for next sector
	dec cx					; next sector
	
	or cx, cx
	jz _read_sectors

	jmp read_sectors
_read_sectors:
	ret



read_one_sect:
	; relative sector # (ax) / sectors per one track (di)
	; = relative track # (ax), absolute sector # in track (dx)
	mov ax, [current_sect]
	mov di, [sect_per_track]
	xor dx, dx
	div di

	; sector # is 1-based count
	inc dl
	mov byte [read_sect], dl

	; relative track # (ax) / headers count
	; = absolute track # (ax), head number (dx)
	xor dx, dx
	div word [head_per_drv]

	mov ch, al			; track(cylinder) #
	mov cl, [read_sect]	; sector #
	mov ah, 2			; service #
	mov al, 1			; sector count
	mov dh, dl			; head #
	mov dl, [boot_drv]	; floppy
	mov bx, si			; offset
	int 0x13

	; next sector to be read
	inc byte [current_sect]

	ret


hello_msg	db "C"

times 510-($-$$) db 0
	dw 0xaa55



