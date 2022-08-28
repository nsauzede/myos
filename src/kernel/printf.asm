
BITS 16
CPU 8086

printflen db 0
; ds:si=fmt, stack=args...
printf:
push ds
push cs
pop ds
push bp
mov bp,sp
add bp,6		; bp points to first arg
push ax

.loop:
lodsb
or al,al
jz .end
cmp al,'%'
jne .print1
mov byte [printflen],4

.fmtloop:
lodsb
cmp al,'9'
jg .ndig
cmp al,'0'
jge .fmtdig
.ndig:
cmp al,'p'
je .fmtp
cmp al,'x'
je .fmtp
cmp al,'c'
je .fmtc
jmp .endfmtloop

.fmtdig:
sub al,'0'
mov [printflen],al
jmp .fmtloop

.fmtp:
mov bx,[bp]
mov ah,[printflen]
call printw
add bp,2
jmp .loop

.fmtc:
mov ax,[bp]
add bp,2
jmp .endfmtloop

.endfmtloop:
.print1:
call print1

jmp .loop

.end:
pop ax
pop bp
pop ds
ret

%if 1
printw0:
db '????'
printw0end:
db 0
; bx=word ah=len (1-4)
printw:
push si
push bx
push ax
mov si,printw0end-1
.loop:
mov al,bl
and al,0xf
cmp al,9
jg .grt
add al,'0'
jmp .ngrt
.grt:
add al,'a'-10
.ngrt:
mov [si],al
shr bx,1
shr bx,1
shr bx,1
shr bx,1
dec si
dec ah
jnz .loop

pop ax
push ax
mov si,printw0end
mov al,ah
xor ah,ah
sub si,ax
call print
pop ax
pop bx
pop si
ret
%else	; this was just to test AAM [n] ; broken, don't use
; bx=word ah=len (1-4)
printw:
push ax
push bx
push cx
mov cl,ah
mov al,bh
aam 16
xchg al,ah
cmp al,9
jg .grt
add al,'0'
jmp .ngrt
.grt:
add al,'a'-10
.ngrt:
call print1
dec cl
jz .end
mov al,ah
cmp al,9
jg .grt2
add al,'0'
jmp .ngrt2
.grt2:
add al,'a'-10
.ngrt2:
call print1
dec cl
jz .end
mov al,bl
aam 16
xchg ah,al
cmp al,9
jg .grt3
add al,'0'
jmp .ngrt3
.grt3:
add al,'a'-10
.ngrt3:
call print1
dec cl
jz .end
mov al,ah
cmp al,9
jg .grt4
add al,'0'
jmp .ngrt4
.grt4:
add al,'a'-10
.ngrt4:
call print1
.end
pop cx
pop bx
pop ax
ret
%endif
