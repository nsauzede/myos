
BITS 16
CPU 8086

; ds:si=fmt, stack=args...
printf:
push bp
mov bp,sp
add bp,4		; bp points to first arg
push ax

.loop:
lodsb
or al,al
jz .end
cmp al,'%'
jne .print1

.fmtloop:
lodsb
cmp al,'p'
je .fmtp
cmp al,'x'
je .fmtp
jmp .endfmtloop

.fmtp:
mov ax,[bp]
call printw
add bp,2
jmp .loop

.endfmtloop:
.print1:
call print1

jmp .loop

.end:
pop ax
pop bp
ret

printw0:
db '????'
printw0end:
db 0
; ax=word
printw:
push si
push bx
push ax
mov si,printw0end-1
mov bx,ax
mov ah,printw0end-printw0
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

mov si,printw0
call print
pop ax
pop bx
pop si
ret
