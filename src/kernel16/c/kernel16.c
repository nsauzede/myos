#asm
#include "start.s"
#endasm

#include "libc.h"
#include "vid.h"
#include "ioport.h"

#asm
_kernel_main:
#endasm
void kernel_main()
{
//	console_init();
	
//	printf( "\n\n");
//	printf( "hello kernel16 - kernel_main=[%p]\n", kernel_main);
	while (1)
	{
		asm ( "hlt");
	}
}

//#include "vid.c"
//#include "libc.c"
#include "ioport.c"
