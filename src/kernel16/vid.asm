
BITS 16
CPU 8086

%define MAX_COL 80
%define MAX_ROW 25
%define BLACK 0
%define GREEN 0x2
%define BG(c) (c << 4)
%define FG(c) (c)
%define DEFAULT_ATTR (BG(BLACK) | FG(GREEN))

console_init:
call cls
call home
ret

; ds:si=strz
print:
push si
push bx
push ax
mov bx,1
mov ah,0x0E
.loop:
lodsb
or al,al
jz .end
int 10h
jmp .loop
.end:
pop ax
pop bx
pop si
ret

; al=char
print1:
push bx
push ax
mov bx,1
mov ah,0x0E
int 10h
pop ax
pop bx
ret

; row_col
gotoxy:
push bp
mov bp,sp
push ax
push bx
push dx
mov ah,2
xor bh,bh
mov dx,[bp+4]
int 10h
pop dx
pop bx
pop ax
pop bp
ret

home:
push ax
xor ax,ax
push ax
call gotoxy
add sp,2
pop ax
ret

cls:
push ax
push es
push di
push cx
mov ax,0xB800
mov es,ax
xor di,di
mov cx,MAX_COL*MAX_ROW
mov ah,DEFAULT_ATTR
rep stosw
pop cx
pop di
pop es
pop ax
ret
