
BITS 16
CPU 8086

print0:
mov bx,1
mov ah,0x0E
int 10h
print:
lodsb
cmp al,0
jnz print0
ret

