#asm
#include "k_start.asm"
#endasm

#include "libc.h"
#include "vid.h"
#include "ioport.h"

#asm
_kernel_main:
#endasm
void kernel_main()
{
#if 0
    console_init();
    printf( "\n\n");
    printf( "hello kernel16 - kernel_main=[%p]\n", kernel_main);
#endif
#if 1
#asm
//    mov si,#0xb800
//    mov [si],0x41
    push ds
    push bx
    mov bx,#0xb800
    mov ds,bx
    mov bx,#0x0c41
    mov [0],bx
    pop bx
    pop ds
#endasm
#endif
    while (1) {
        asm ( "hlt");
    }
}

//#include "vid.c"
//#include "libc.c"
//#include "ioport.c"

#asm
//#include "vid.s"
//#include "libc.s"
#include "ioport.s"
#include "k_end.asm"
#endasm

#asm
//#include "k_end.asm"
#endasm

#asm
//kernel_end:
#endasm
