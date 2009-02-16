#include "libc.h"

#if 1
unsigned char stack[0x4000] = {
	[0] = 0xde,
	[sizeof(stack) - 1] = 0xef
};
#endif

void kernel_main()
{
	console_init();
	
	printf( "hello kernel32 - kernel_main=[%p]\n", kernel_main);
	
	printf( "*system32 halted*");
	while (1)
	{
		asm volatile( "hlt");
	}
}
