%define KERNEL_LOAD_ADDR 0x7E00
%define KERNEL_SSIZE ((kernel_end-kernel_start) / 512)
%define LOAD_SEG 0
%define LOAD_OFS 0

BITS 16
CPU 8086
ORG KERNEL_LOAD_ADDR

kernel_start:

db KERNEL_SSIZE
db 0				; 1=32 bit mode (protected)
dw LOAD_SEG
dw LOAD_OFS

kernel_entry:
jmp _kernel_entry

; multiboot header
magic dd 0x1badb002
flags dd 0x0
chksm dd 0x0
hdrad dd magic
ldadr dd KERNEL_LOAD_ADDR
ldead dd KERNEL_LOAD_ADDR+kernel_end
bssea dd 0x0
entry dd _kernel_entry

greet db 13,10,13,10,"hello kernel16",13,10,0
shalted db "*system16 halted*",0
greet2 db "DIRECT SCREEN OUTPUT",13,10,0

_kernel_entry:
push cs
pop ds

call console_init

mov si,greet
call print

call home
; display some text
mov si,greet2
call print

; halt system
mov si,shalted
call print
inf:
hlt
jmp inf
ret

%include "vid.asm"
;%include "vidnb.asm"

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
