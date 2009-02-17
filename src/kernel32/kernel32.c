#include "libc.h"

#include "idt.h"

unsigned char stack[0x4000] = {
	[0] = 0xde,
	[sizeof(stack) - 1] = 0xef
};

static void divideerror( int id)
{
	static int count = 0;
	printf( "*divide error #%p - system32 halted*", (void *)count++);
#if 0
	while (1)
	{
		asm volatile( "hlt");
	}
#endif
}

#if 1
static void kbhandler( int foo)
{
	static int count = 0;
	printf( "*kb handler #%p - system32 halted*", (void *)count++);
#if 0
	while (1)
	{
		asm volatile( "hlt");
	}
#endif
}
#endif

void kernel_main()
{
	console_init();		// this will initialize internal console data for subsequent printouts
	
	printf( "hello kernel32 - kernel_main=[%p]\n", kernel_main);
	
	idt_setup();		// init idt to all disabled interrupts
	exception_setup();	// init at least double fault to default wrapper
	irq_setup();
	exception_set_handler( EXCEPTION_DIVIDE, divideerror);
	irq_set_handler( IRQ_KB, kbhandler);
	
	asm volatile( "sti");
	
#if 0
	int i = 5;
	while (1)
	{
		asm volatile( "mov $1,%%eax;div %0,%%eax"::"g"(i));
		i--;
	}
#endif
	
	printf( "*system32 halted*");
	while (1)
	{
		asm volatile( "hlt");
	}
}
