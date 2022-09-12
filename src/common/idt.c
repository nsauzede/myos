#include "libc.h"
#include "types.h"

#include "ioport.h"
#include "idt.h"
#include "vid.h"

#pragma pack(1)
typedef struct idtr {
    uint16_t limit;
#ifdef _LP64
    uint64_t base_addr;
#else
    uint32_t base_addr;
#endif
} idtr_t;
#pragma pack()

#pragma pack(1)
typedef struct idte {
    uint16_t offset_low;
    uint16_t seg_sel;

#ifdef _LP64
    uint8_t ist:3;
    uint8_t reserved:5;
#else
    uint8_t reserved:5;
    uint8_t flags:3;
#endif
    uint8_t type:3;
    uint8_t op_size:1;
    uint8_t zero:1;
    uint8_t dpl:2;
    uint8_t present:1;
    uint16_t offset_high;
#ifdef _LP64
    uint32_t offset_highbis;
    uint32_t rsvd;
#endif
} idte_t;
#pragma pack()

#define IDT_NUM IDT_MAX
static idte_t idt[IDT_NUM];

extern void *idt_wrappers[IDT_NUM] asm("idt_wrappers");
extern void *def_int_wrappers[IDT_NUM] asm("def_int_wrappers");

idt_handler_t idt_handlers[IDT_NUM] asm("idt_handlers");

void halt()
{
	asm volatile("hlt");
}

// FIXME : implement flags backup
void disable()
{
	asm volatile("cli");
}

void enable()
{
	asm volatile("sti");
}

void idt_setup()
{
	idtr_t idtr;
	
	int i;
	for (i = 0; i < IDT_NUM; i++)
	{
		idte_t *idte = &idt[i];
		memset(idte, 0, sizeof(*idte));
		idte->seg_sel = 0x8;
		idte->type = 0x6;
		idte->op_size = 1;
		idt_set_handler(i, def_int_wrappers[i]);
	}
	
	idtr.base_addr = (typeof(idtr.base_addr))idt;
	idtr.limit = sizeof(idt) - 1;
	
	asm volatile(
		"lidt %0\n\r"
		:
		: "m"(idtr)
	);
}

void idt_set_handler(int num, idt_handler_t handler) {
    idte_t *idte = &idt[num];
    if (!handler) {
        idte->offset_low = 0;
        idte->offset_high = 0;
        idte->dpl = 0;
        idte->present = 0;
#ifdef _LP64
        idte->offset_highbis = 0;
        idte->rsvd = 0;
#endif
    } else {
        idte->offset_low = (typeof(idte->offset_low))((uintptr_t)handler & 0xFFFF);
        idte->offset_high = (typeof(idte->offset_high))(((uintptr_t)handler >> 16) & 0xFFFF);
        idte->dpl = 0;
        idte->present = 1;
#ifdef _LP64
        idte->offset_highbis = (typeof(idte->offset_highbis))(((uintptr_t)handler >> 32) & 0xFFFFFFFF);
        idte->rsvd = 0;
#endif
    }
}

void exception_setup()
{
	idt_set_handler(EXCEPTION_DOUBLE, idt_wrappers[EXCEPTION_DOUBLE]);
}

void exception_set_handler(int num, idt_handler_t handler)
{
	disable();
	idt_handlers[num] = handler;
	idt_set_handler(num, idt_wrappers[num]);
//	enable();
}

#define MASTER 0x20
#define SLAVE 0xa0
void i8259_setup()
{
	outb(0x11, MASTER);
	outb(0x11, SLAVE);

	outb(0x20, MASTER+1);
	outb(0x28, SLAVE+1);

	outb(0x4, MASTER+1);
	outb(0x2, SLAVE+1);

	outb(0x1, MASTER+1);
	outb(0x1, SLAVE+1);

	outb(0xFB, MASTER+1);
	outb(0xFF, SLAVE+1);
}

void i8259_enable_line(int num)
{
	outb(inb(MASTER+1) & ~(1 << num),MASTER+1);
}

void i8259_disable_line(int num)
{
	outb(inb(MASTER+1) | (1 << num),MASTER+1);
}

void irq_setup()
{
	i8259_setup();
	
	i8259_disable_line(0);
	i8259_disable_line(1);
}

void irq_set_handler(int num, idt_handler_t handler)
{
	disable();
#if 1
	idt_handlers[IRQ_BASE + num] = handler;
	idt_set_handler(IRQ_BASE + num, idt_wrappers[IRQ_BASE + num]);
#endif
	i8259_enable_line(num);
//	enable();
}

#define MAX_FREQ 1193180
#define TIMER0 0x40
#define CONTROL 0x43
void i8254_set_freq(unsigned int freq)
{
	unsigned int nb_tick = MAX_FREQ / freq;
	outb(0x34, CONTROL);
	outb(nb_tick & 0xFF, TIMER0);
	outb((nb_tick >> 8) & 0xFF, TIMER0);
}
