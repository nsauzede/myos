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

loops dw 0
jiffies dw 0
kbs dw 0
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

def_handler0 db "int#%x: "
db "code=%x "
db "ip=%x cs=%x flags=%x "
db "es=%x ds=%x ss=%x "
db 13,10
db "ax=%x bx=%x cx=%x dx=%x si=%x di=%x "
db "bp=%x sp=%x "
db 0
def_handler:
				; 30 (flags)
				; 28 (cs)
				; 26 (ip)
				; 24 (ax)
push sp			; 22
push bp			; 20
push di			; 18
push si			; 16
push dx			; 14
push cx			; 12
push bx			; 10
push es			; 8
push ds			; 6
push ss			; 4
push ax			; 2 (int number)
mov ax,0xc0de
push ax			; 0 (int code)

mov bp,sp
mov ax,word [bp+22]
add ax,8		; compensate 4 words (flags, cs, ip, ax)
push ax
push word [bp+20]
push word [bp+18]
push word [bp+16]
push word [bp+14]
push word [bp+12]
push word [bp+10]
push word [bp+24]
push word [bp+4]
push word [bp+6]
push word [bp+8]
push word [bp+30]
push word [bp+28]
push word [bp+26]
push word [bp+0]
push word [bp+2]

push cs
pop ds
mov di,0xB800
mov es,di
xor di,di
mov ax,0x0C00
mov cx,80*2
rep stosw
call home

mov si,def_handler0
call printf

.inf:
hlt
jmp .inf

def_handler_0:
push ax
mov ax,0
jmp def_handler

def_handler_1:
push ax
mov ax,1
jmp def_handler

; kernel entry
;------------------------------------------------------------
_kernel_entry:
push cs
pop ds

cli
push ds
xor si,si
mov ds,si

in al,0x21

mov word [0x00*4+2],cs
mov word [0x00*4+0],def_handler_0

mov word [0x01*4+2],cs
mov word [0x01*4+0],def_handler_1

mov word [0x08*4+2],cs
mov word [0x08*4+0],timer_handler
and al,~(1 << 0)

mov word [0x09*4+2],cs
mov word [0x09*4+0],kb_handler
and al,~(1 << 1)

out 0x21,al

pop ds

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

mov bx,0xbbbb
mov cx,0xcccc
mov dx,0xdddd
mov bp,0xb0b0
mov si,0x5151
mov di,0xd1d1
mov ax,0xe5e5
mov es,ax
mov ax,0xd5d5
mov ds,ax
mov ax,0xaaaa
int 1

jmp loop0
;----------------------------------------------------------------------

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
