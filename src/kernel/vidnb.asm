
BITS 16
CPU 8086
ORG KERNEL_LOAD_ADDR

%define BLACK 0
%define GREEN 0x2
%define BG(c) (c << 4)
%define FG(c) (c)
%define DEFAULT_ATTR (BG(BLACK) | FG(GREEN))
%define MAX_COL 80
%define MAX_ROW 25

; display some text without bios support
; ds:si=str
printnb:
push es
push di
push ax
mov di,0xB800
mov es,di
xor di,di
.loop:
lodsb
or al,al
jz .end
stosb
mov al,DEFAULT_ATTR
stosb
jmp .loop
.end
pop ax
pop di
pop es
ret

;---------------------------------
; al=byte, stack=seg,ofs,count
memset:
push bp
mov bp,sp
push es
push di
push cx
mov di,[bp+4]
mov es,di
mov di,[bp+6]
mov cx,[bp+8]
rep stosb
pop cx
pop di
pop es
pop bp
ret

%if 1
console_initnb:
%if 0
push ax
mov al,DEFAULT_ATTR
call setattr
pop ax
call home
%endif
call cls
ret

attr db 0
; al=attr
setattr:
mov [attr],al
ret

col dw 0
row dw 0
; ax=col bx=row
gotoxynb:
mov [col],ax
mov [row],bx
ret

homenb:
push ax
push bx
xor ax,ax
xor bx,bx
call gotoxy
pop bx
pop ax
ret

clsnb:
mov ax,MAX_ROW*MAX_COL*2
push ax
xor ax,ax
push ax
mov ax,0xb800
push ax
mov al,0
call memset
add sp,6
ret
%endif

