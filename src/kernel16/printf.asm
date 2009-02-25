
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
