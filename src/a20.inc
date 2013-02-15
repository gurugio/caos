;****************************************************************
; Enable A20.  This is at the very best an annoying procedure.
; A20 code ported from SYSLINUX 1.52-1.63 by H. Peter Anvin.
; AMD Elan bug fix by Robert Schwebel.
;****************************************************************

;****************************************************************
;CPU가 AMD ELAN 일 경우 
;
;mov al, 0x02                 ; 또 하나의 A20 게이트
;out 0x92, al                 ; 이것은 SC410/SC520에서 동작한다.
;a20_elan_wait:
;call a20_test
;jz a20_elan_wait
;jmp a20_done
;
;*****************************************************************

A20_TEST_LOOPS          equ  32           ; wait 할 때마다의 반복 수
A20_ENABLE_LOOPS        equ 255           ; 시험할 loop 의 합
A20_TEST_ADDR           equ 4*0x80


a20_try_loop:

; 먼저, 컴퓨터에 A20 게이트가 없는지 알아본다.
a20_none:
call    a20_test
jnz     a20_done

; 다음에,  BIOS (INT 0x15, AX=0x2401)를 시험해 본다.
a20_bios:
mov     ax, 0x2401 
pushfd                                  ; 좀 지나치게 FLAG를 의심한다. 
int     0x15
popfd

call    a20_test
jnz     a20_done

; 키보드 컨트롤러를 통해 A20 게이트를 켜본다.
a20_kbc:
call    empty_8042

call    a20_test                        ; BIOS 기능으로 구현이 되었다면 
jnz     a20_done                        ; delay 된 반응

mov     al, 0xD1                      ; command를 라이트한다.
out     0x64, al
call    empty_8042

mov     al, 0xDF                      ; A20 on
out    0x60, al
call    empty_8042

; A20이 진짜로 켜질 때 까지 기다린다. 이것은 어떤 시스템에서는
; 상당히 오랜 시간이 걸릴 수도 있다.
; Toshiba Tecras는 이러한 문제를 가지고 있다고 전해진다.
a20_kbc_wait:
xor     cx, cx
a20_kbc_wait_loop:
call    a20_test
jnz     a20_done
loop    a20_kbc_wait_loop

; 마지막 시도: "제어포트 A"
a20_fast:
in      al, 0x92                      ; 제어포트 A
or      al, 0x02                      ; "fast A20" 버젼은
and     al, 0xFE                      ; 갑자기 RESET되지 않는다.
out     0x92, al

; 제어포트 A에 효과가 나타나기를 기다린다.
a20_fast_wait:
xor    cx, cx
a20_fast_wait_loop:
call    a20_test
jnz     a20_done
loop    a20_fast_wait_loop

; A20이 아직도 반응을 보이지 않는다. 다시 해 본다. 
; 
dec     byte [a20_tries]
jnz     a20_try_loop

a20_die:
hlt
jmp     a20_die

a20_tries:
db   A20_ENABLE_LOOPS

; 여기까지 온다면, A20이 켜진 것이다.
a20_done:
ret

a20_test:
push    cx
push    ax
xor     cx, cx
mov     fs, cx                        ; Low memory
dec     cx
mov     gs, cx                        ; High memory area
mov     cx, A20_TEST_LOOPS
mov     ax, word [fs:A20_TEST_ADDR]
push    ax
a20_test_wait:
inc     ax
mov     word [fs:A20_TEST_ADDR], ax
call    delay                           
cmp     ax, word [gs:A20_TEST_ADDR+0x10]
loop   a20_test_wait

pop     word [fs:A20_TEST_ADDR]
pop     ax
pop     cx
ret     


empty_8042:
push   ecx
mov    ecx, 100000

empty_8042_loop:
dec    ecx
jz     empty_8042_end_loop

call   delay

in     al, 0x64                      ; 8042 상태포트 
test   al, 1                         ; 출력 버퍼 테스트
jz     no_output

call   delay
in    al, 0x60                      ; 읽는다.
jmp    empty_8042_loop

no_output:
test   al, 2                         ; 입력 버퍼가 가득찼는가? 
jnz    empty_8042_loop               ; yes - loop를 돈다.

empty_8042_end_loop:
pop    ecx
ret

delay:
out    0x80, al
ret

