#include "libc.h"
#include "types.h"

#include "idt.h"

#pragma pack(1)
typedef struct idtr {
	uint16_t limit;
	uint32_t base_addr;
} idtr_t;
#pragma pack()

#pragma pack(1)
typedef struct idte {
	uint16_t offset_low;
	uint16_t seg_sel;

	uint8_t reserved:5;
	uint8_t flags:3;
	uint8_t type:3;
	uint8_t op_size:1;
	uint8_t zero:1;
	uint8_t dpl:2;
	uint8_t present:1;
	uint16_t offset_high;
} idte_t;
#pragma pack()

#define IDT_NUM IDT_MAX
static idte_t idt[IDT_NUM];

extern void *idt_wrappers[IDT_NUM];

typedef void (*idt_handler_t)( int);
idt_handler_t idt_handlers[IDT_NUM];

void idt_setup()
{
	idtr_t idtr;
	
	int i;
	for (i = 0; i < IDT_NUM; i++)
	{
		idte_t *idte = &idt[i];
		memset( idte, 0, sizeof( *idte));
		idte->seg_sel = 0x8;
		idte->type = 0x6;
		idte->op_size = 1;
	}
	
	idtr.base_addr = (typeof(idtr.base_addr))idt;
	idtr.limit = sizeof( idt) - 1;
	
	asm volatile(
		"lidt %0\n\r"
		:
		: "m"(idtr)
		: "memory"
	);
}

void idt_set_handler( int num, void *handler)
{
	idte_t *idte = &idt[num];
	if (!handler)
	{
		idte->offset_low = 0;
		idte->offset_high = 0;
		idte->dpl = 0;
		idte->present = 0;
	}
	else
	{
		idte->offset_low = (typeof(idte->offset_low))(uintptr_t)handler & 0xFFFF;
		idte->offset_high = ((typeof(idte->offset_low))(uintptr_t)handler >> 16) & 0xFFFF;
		idte->dpl = 0;
		idte->present = 1;
	}
}

void exception_setup()
{
	idt_set_handler( EXCEPTION_DOUBLE, idt_wrappers[EXCEPTION_DOUBLE]);
}

void exception_set_handler( int num, void *handler)
{
	asm volatile( "cli");
	idt_handlers[num] = handler;
	idt_set_handler( num, idt_wrappers[num]);
	asm volatile( "sti");
}

#define outb(value, port) \
	__asm__ volatile (                                            \
	"outb %b0,%w1"                                          \
	::"a" (value),"Nd" (port)                               \
	)

#define inb(port)                                               \
	({                                                              \
	unsigned char _v;                                             \
	__asm__ volatile (                                            \
	"inb %w1,%0"                                            \
	:"=a" (_v)                                              \
	:"Nd" (port)                                            \
	);                                                      \
	_v;                                                           \
	})

#define MASTER 0x20
#define SLAVE 0xa0
void i8259_setup()
{
	outb( 0x11, MASTER);
	outb( 0x11, SLAVE);

	outb( 0x20, MASTER+1);
	outb( 0x28, SLAVE+1);

	outb( 0x4, MASTER+1);
	outb( 0x2, SLAVE+1);

	outb( 0x1, MASTER+1);
	outb( 0x1, SLAVE+1);

	outb( 0xFB, MASTER+1);
	outb( 0xFF, SLAVE+1);
}

void i8259_enable_line( int num)
{
	outb( inb(MASTER+1) & ~(1 << num),MASTER+1);
}

void irq_setup()
{
	i8259_setup();
}

void irq_set_handler( int num, void *handler)
{
	asm volatile( "cli");
	idt_handlers[IRQ_BASE + num] = handler;
	idt_set_handler( IRQ_BASE + num, idt_wrappers[num]);
	i8259_enable_line( num);
	asm volatile( "sti");
}

