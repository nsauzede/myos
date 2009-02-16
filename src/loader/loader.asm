%define LOADER_ADDR 0x7C00
%define LOADER_OFS LOADER_ADDR
%define LOADER_SEG (LOADER_ADDR>>4)

%define KERNEL_ADDR 0x7E00
%define KERNEL_OFS KERNEL_ADDR
%define KERNEL_SEG (KERNEL_ADDR>>4)
%define KERNEL_ENTRY 6

; mandatory, to get proper offsets, whatever segment we get initially loaded to
org 0

bits 16

start:

%ifdef ROM
db 0x55,0xaa
db 0						; dummy value, will be fixed at assembly time
%endif

cli							; useful when single stepping rom code

push cs
pop ds						; this one to be able to print local strings
call greet1
%ifdef SECTOR
greet db "secldr",0
%else
greet db "romldr",0
%endif
greet1:
pop si
call print

call copy					; copy now, that we still have the proper source ds

%ifdef ROM
push cs
pop ds
mov si,LOADER_SEG
mov es,si
xor si,si
xor di,di
;mov cx,512/4*2				; copy two sectors, comprising first kernel's one
mov cx,kernel_begin-start+2			; copy loader+kernel begin
cld
rep movsb
%endif

jmp LOADER_SEG:zero_offset	; mandatory to honour our null org
zero_offset:

push cs
pop ds

;cli
;mov ss,ax					; do we really need a special stack here ?
;mov sp,0x2000
;sti

%if 1

mov al,[kernel_begin+1]
cmp al,1
jne nprotect				; shall we switch to protected mode now ? (if 32 bit kernel)

call is32bit0
is32bit db "kernel is 32 bit",13,10,0
is32bit0:
pop si
call print

cli
jmp 0:LOADER_ADDR+_here
_here:
push cs
pop ds
lgdt [LOADER_ADDR+gdt]
mov eax,cr0
or al,1
mov cr0,eax
mov ax,0x10
mov ds,ax
mov es,ax
mov fs,ax
mov gs,ax
bits 32
db 0x66
jmp 0x8:LOADER_ADDR+here32
here32:
mov eax,0xbeefdead
call 0x8:KERNEL_ADDR+KERNEL_ENTRY	; perform an absolute call here !
mov eax,0xdeadbeef
inf32:
hlt
jmp inf32
bits 16

jmp protectdone

nprotect:
call is16bit0
is16bit db "kernel is 16 bit",13,10,0
is16bit0:
pop si
call print

protectdone:

%endif

jmp 0:KERNEL_ADDR+KERNEL_ENTRY

gdt:
NULL_Desc:
dw EndGDT-gdt-1
dd LOADER_ADDR+gdt
.unused:
dw 0
CS_Desc:
dw 0xFFFF,0
db 0,0x9B,0xCF,0
DS_Desc:
dw 0xFFFF,0
db 0,0x93,0xCF,0
EndGDT:

error:
inf:
hlt
jmp inf

print0:
mov bx,1
mov ah,0x0E
int 10h
print:
lodsb
cmp al,0
jnz print0
ret

copy:
%ifdef SECTOR
mov ax,0x0202
mov cx,0x0001
mov dx,0x0000				; 01=head1 00=fda
xor bx,bx
mov es,bx
mov bx,LOADER_OFS
int 0x13					; clobbers CF,ax
jc error
call dots
mov al,[KERNEL_OFS]
cmp al,0x1
jbe .stop
mov ah,0x02
inc al
int 0x13					; clobbers CF,ax
jc error
call dots
.stop:
%else
mov si,KERNEL_SEG
mov es,si
mov si,kernel_begin
xor di,di
xor cx,cx
mov cl,[kernel_begin+0]		; SSIZE
shl cx,7
cld
rep movsd
%endif
call scrlf0
scrlf db 13,10,0
scrlf0:
pop si
call print
ret

%ifdef SECTOR
; al=ndots
dots:
push ax
push bx
push cx
mov cl,al
inc cl
mov bx,1
mov ax,0x0E2E
.loop:
dec cl
jz .endloop
int 10h
jmp .loop
.endloop:
pop cx
pop bx
pop ax
ret
%endif

end:

%ifdef SECTOR
times (512 - (end - start) - 2) db 0
db 0x55,0xAA
%endif

kernel_begin:
