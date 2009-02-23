#include "libc.h"
#include "vid.h"

#include "ioport.h"
#include "idt.h"

unsigned char stack[0x4000] = {
	[0] = 0xde,
	[sizeof(stack) - 1] = 0xef
};

void int_handler( int id, uint32_t _ebp,
	uint32_t gs, uint32_t fs, uint32_t es, uint32_t ds, uint32_t ss,
	uint32_t eax, uint32_t ebx, uint32_t ecx, uint32_t edx, uint32_t esi, uint32_t edi, uint32_t ebp,
	uint32_t code, uint32_t eip, uint32_t cs, uint32_t eflags
	)
{
	home();
	setattr( BG_BLACK | FG_RED);
	printf( "int#%p: ", id);
	printf( "code=%x eip=%x cs=%x eflags=%x ",
		code, eip, cs, eflags
	);
	printf( "gs=%x fs=%x es=%x ds=%x ss=%x ", gs, fs, es, ds, ss);
	printf( "\n");
	printf( "eax=%x ebx=%x ecx=%x edx=%x esi=%x edi=%x ebp=%x\n",
		eax, ebx, ecx, edx, esi, edi, ebp
	);
	setcursor( 0, 0);
	while (1)
	{
		asm volatile( "hlt");
	}
}

static int jiffies = 0;
static int kbhits = 0;
static int divs = 0;

void timer_handler( int id, uint32_t ebp)
{
	jiffies++;
}

#define KB_PORT 0x60
void kb_handler( int id, uint32_t ebp)
{
	inb( KB_PORT);
	kbhits++;
}

void div_handler( int id, uint32_t ebp)
{
	divs++;
}

void kernel_main()
{
	console_init();		// this will initialize internal console data for subsequent printouts
	
	printf( "\n\n");	// leave first lines for interrupts
	printf( "hello kernel32 - kernel_main=[%p]\n", kernel_main);
	
	idt_setup();		// init idt with default int handlers

//	exception_set_handler( EXCEPTION_DIVIDE, div_handler);
	exception_set_handler( EXCEPTION_DIVIDE, (idt_handler_t)int_handler);

	irq_setup();
	irq_set_handler( IRQ_KB, kb_handler);
	irq_set_handler( IRQ_TIMER, timer_handler);
	i8254_set_freq( 1);

	asm volatile( "sti");	
	
	int i = 1;
	printf( "\n");
	while (1)
	{
		static int count = 0;
		static int div = 0;
		
		printf( "\rlooping.. #%p jif=%p kb=%p divs=%p div=%x", (void *)count++, jiffies, kbhits, divs, div);
//		asm volatile( "int $0");
#if 0
		asm volatile( "mov $0x12345678,%%ecx;mov $0xdeadbeef,%%ebx;mov $0xcafedeca,%%eax;idivl %0"::"g"(i):"eax");
		div = 1 / i;
#endif
		if (i)i--;
	}
	
	printf( "*system32 halted*");
	while (1)
	{
		asm volatile( "hlt");
	}
}
