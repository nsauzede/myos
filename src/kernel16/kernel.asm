%define KERNEL_LOAD_ADDR 0x7E00
%define KERNEL_SSIZE ((kernel_end-kernel_start) / 512)
%define LOAD_SEG 0
%define LOAD_OFS 0

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

_kernel_entry:
push cs
pop ds
call here
here:
pop word [orig]
mov si,[orig]
add si,greet-here
call print

; display some text
mov di,0xB800
mov es,di
xor di,di
mov si,[orig]
add si,greet2-here
.loop:
lodsb
or al,al
jz .end
stosb
inc di
jmp .loop
.end:

; halt system
mov si,[orig]
add si,shalted-here
call print
inf:
hlt
jmp inf
ret

orig dw 0
greet db "hello kernel16",13,10,0
shalted db "*system16 halted*",0
greet2 db "DIRECT SCREEN OUTPUT",0

print0:
mov bx,1
mov ah,0x0E
int 10h
print:
lodsb
cmp al,0
jnz print0
ret

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
