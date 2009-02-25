%define KERNEL_LOAD_ADDR 0x7E00
%define KERNEL_SSIZE ((kernel_end-kernel_start) / 512)
%define LOAD_SEG 0
%define LOAD_OFS KERNEL_LOAD_ADDR

BITS 16
CPU 8086
ORG KERNEL_LOAD_ADDR

kernel_start:

db KERNEL_SSIZE
db 0				; 1=32 bit mode (protected)
dw LOAD_SEG
dw LOAD_OFS

kernel_main:
jmp _kernel_entry

sgreet db "hello kernel16 - kernel_main=[%p]",13,10,0

; multiboot header
magic dd 0x1badb002
flags dd 0x0
chksm dd 0x0
hdrad dd magic
ldadr dd KERNEL_LOAD_ADDR
ldead dd KERNEL_LOAD_ADDR+kernel_end
bssea dd 0x0
entry dd _kernel_entry

shalted db "*system16 halted*",0
sgreet2 db "DIRECT SCREEN OUTPUT",13,10,0

loops dw 0
jiffies dw 0
kbs dw 6
divs dw 0
divi dw 0

%define KB_SIZE 20
%define KB_PORT 0x60
kb_ring times KB_SIZE db 0
kb_head dw KB_SIZE-1
kb_handler:
push ax
push si
mov al,0x20
out 0x20,al
in al,KB_PORT
%if 0
mov si,word [kb_head]
inc si
cmp si,KB_SIZE
jb .skip
xor si,si
.skip:
mov word [kb_head],si
add si,kb_ring
stosb
%endif
inc word [kbs]
pop si
pop ax
iret

timer_handler:
push ax
mov al,0x20
out 0x20,al
inc word [jiffies]
pop ax
iret

; kernel entry
;------------------------------------------------------------
_kernel_entry:
push cs
pop ds

%if 1
cli
push ds
xor si,si
mov ds,si
mov word [0x8*4+2],cs
mov word [0x8*4+0],timer_handler
pop ds
in al,0x21
and al,~(1 << 8)
out 0x21,al
%endif

%if 1
cli
push ds
xor si,si
mov ds,si
mov word [0x9*4+2],cs
mov word [0x9*4+0],kb_handler
pop ds
in al,0x21
and al,~(1 << 1)
out 0x21,al
%endif

sti

call console_init

mov si,0x0200
push si
call gotoxy
add sp,2

mov si,kernel_main
push si
mov si,sgreet
call printf
add sp,2

loop0:

inc word [loops]

mov si,0x0300
push si
call gotoxy
add sp,2

mov si,[divi]
push si
mov si,[divs]
push si
mov si,[kbs]
push si
mov si,[jiffies]
push si
mov si,[loops]
push si
mov si,sloop
call printf
add sp,10

mov cx,KB_SIZE
mov si,kb_ring
.loop:
mov al,' '
cmp cx,kb_head
jne .skip
mov al,'<'
.skip:
push ax
lodsb
push ax
mov si,skb
call printf
add sp,4
dec cx
jnz .loop

hlt

jmp loop0

call home
; halt system
mov si,shalted
call print
inf:
hlt
jmp inf
ret

sloop db "looping.. #%x jif=%x kb=%x divs=%x div=%x",13,10,0
skb db "%x:%c ",0

%if 1
%include "vid.asm"
%else
%include "vidnb.asm"
%endif
%include "printf.asm"

; to test multisector load
;times 1024 db 0xca

_kernel_end:

; round the kernel size to an integral number of 512 byte sectors
; else qemu FDC will fail to read last sector
%define MODU ((_kernel_end - kernel_start) % 512)
%if MODU > 0
times (512 - MODU) db 0xEE
%endif

kernel_end:
