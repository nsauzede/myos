#include "libc.h"
#include "vid.h"

void kernel_main()
{
	console_init();
	
	printf( "\n\n");
	printf( "hello kernel16 - kernel_main=[%p]\n", kernel_main);
	
	while (1)
	{
//		asm volatile( "hlt");
		asm ( "hlt");
	}
}
