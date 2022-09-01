
#define EXCEPTION_BASE		0
#define EXCEPTION_DIVIDE	0
#define EXCEPTION_SSTEP		1
#define EXCEPTION_DOUBLE	8
#define IRQ_BASE			32
#define IRQ_TIMER			0
#define IRQ_KB				1

#ifndef ASSEMBLY

//#define IDT_MAX (IRQ_BASE+IRQ_KB)
#define IDT_MAX (256)

typedef void (*idt_handler_t)( int num, uint32_t ebp);

void idt_setup( void);
void idt_set_handler( int num, idt_handler_t);
void exception_setup();
void exception_set_handler( int num, idt_handler_t handler);
void irq_setup();
void irq_set_handler( int num, idt_handler_t);

void i8254_set_freq( unsigned int freq);

void halt();
void enable();
void disable();

#endif
