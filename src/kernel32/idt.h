
#define EXCEPTION_BASE		0
#define EXCEPTION_DIVIDE	0
#define EXCEPTION_DOUBLE	8
#define IRQ_BASE			32
#define IRQ_TIMER			0
#define IRQ_KB				1

#ifndef ASSEMBLY

#define IDT_MAX (IRQ_BASE+IRQ_KB)

void idt_setup( void);
void idt_set_handler( int num, void *);
void exception_setup();
void exception_set_handler( int num, void *handler);
void irq_setup();
void irq_set_handler( int num, void *handler);

#endif
