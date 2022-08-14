#include "libc.h"
#include "vid.h"

#include "ioport.h"
#include "idt.h"

#include "tasks.h"

unsigned char stack[0x4000] asm("stack") = {
	[0] = 0xde,
	[sizeof(stack) - 1] = 0xef
};

void int_handler( int id, uint32_t _ebp,
	uint32_t gs, uint32_t fs, uint32_t es, uint32_t ds, uint32_t ss,
	uint32_t eax, uint32_t ebx, uint32_t ecx, uint32_t edx, uint32_t esi, uint32_t edi, uint32_t ebp,
	uint32_t code, uint32_t eip, uint32_t cs, uint32_t eflags
	)
{
	extern void home() asm ("home");
	extern void setattr() asm ("setattr");
	extern void setcursor() asm ("setcursor");
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
#define KB_SIZE 20
static unsigned char kb_ring[KB_SIZE];
int kb_head = 0;
void kb_handler( int id, uint32_t ebp)
{
	unsigned char byte;
	byte = inb( KB_PORT);
	kb_ring[kb_head % KB_SIZE] = byte;
	kb_head = (kb_head + 1) % KB_SIZE;
	kbhits++;
}

void div_handler( int id, uint32_t ebp)
{
	divs++;
}

int t1( struct pt *pt, int tid, void *arg)
{
	int startl = (uintptr_t)arg;
	PT_BEGIN( pt);
	
	while (1)
	{
		static int count = 0;
		static int div = 0;
		
		gotoxy( 0, startl + 0);
		printf( "%s: looping.. #%08lx jif=%08lx kb=%p divs=%p div=%x", __func__, (void *)(uintptr_t)count++, jiffies, kbhits, divs, div);
		gotoxy( 0, startl + 1);
		int j;
		for (j = 0; j < KB_SIZE; j++)
		{
			printf( "%02x%c", kb_ring[j], j == kb_head ? '<' : ' ');
		}
		PT_YIELD( pt);
	}
	PT_END( pt);
}

int t2( struct pt *pt, int tid, void *arg)
{
	int startl = (uintptr_t)arg;
	PT_BEGIN( pt);
	
	while (1)
	{
		static int count = 0;
		
		gotoxy( 0, startl + 0);
		printf( "%s: looping.. #%08lx jif=%08lx kb=%x divs=%x", __func__, (void *)(uintptr_t)count++, jiffies, kbhits, divs);
		PT_YIELD( pt);
	}
	PT_END( pt);
}

int scheduler()
{
	init_tasks();
	create_task( t1, (void *)6);
	create_task( t2, (void *)9);
	while (1)
	{
		static int count = 0;
		gotoxy( 0, 3);
		printf( "%s: having some hlt.. %x\n", __func__, count++);
		asm volatile( "hlt");
		schedule_tasks();
	}
	return 0;
}

asm(
".globl s\n"
"s: .string \"Hello 64bit World!\\n\"\n"
);
extern char s[];
extern void kernel_main() asm("kernel_main");
void kernel_main()
{
	console_init();		// this will initialize internal console data for subsequent printouts
//	dputchar('A');
//	dputchar('\n');
//	dputchar('B');
	dputs(s);
//	dputs("hello\nWorld!\n");
//	printf( "\n\n");	// leave first lines for interrupts
//	printf( "hello kernel64 - kernel_main=[%p]\n", kernel_main);
#if 0	
	idt_setup();		// init idt with default int handlers

//	exception_set_handler( EXCEPTION_DIVIDE, div_handler);
	exception_set_handler( EXCEPTION_DIVIDE, (idt_handler_t)int_handler);

	irq_setup();
	irq_set_handler( IRQ_KB, kb_handler);
	irq_set_handler( IRQ_TIMER, timer_handler);
	i8254_set_freq( 1);

	asm volatile( "sti");	

	scheduler();	// infinite loop, with hlt's
	
	printf( "*system64 halted*");
#endif
	while (1)
	{
		asm volatile( "hlt");
	}
}
