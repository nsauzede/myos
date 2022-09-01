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

#define IODELAY() asm volatile( ".byte 0x90,0x90,0x90,0x90,0x90,0x90,0x90,0x90")
