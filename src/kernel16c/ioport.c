#include "ioport.h"

void outb(val, port)
uint8_t val; uint16_t port;
{
#asm
	push bp
	mov  bp, sp
	push ax
	push dx
	mov  dx, 6[bp]
	mov  al, 4[bp]
	out  dx, al
	pop  dx
	pop  ax
	pop  bp
#endasm
}

uint8_t inb(port)
uint16_t port;
{
#asm
	push bp
	mov  bp, sp
	push dx
	mov  dx, 4[bp]
	in   al, dx
	pop  dx
	pop  bp
#endasm
}
