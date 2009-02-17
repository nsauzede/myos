#ifndef KERNEL_ADDR
//#define KERNEL_ADDR 0x7E00
#endif
#define KERNEL_SEG (KERNEL_ADDR >> 4)
#define STACK_SIZE 0x4000
//#define START_IN_REALM
#ifdef START_IN_REALM
#define IS32BIT 0
#else
#define IS32BIT 1
#endif
.org 0				// real org will be set by elf linker, just zero it now
kernel_start:
.byte 64				// FIXME : this is a big problem - the kernel size should be computed !!!
.byte IS32BIT
.short 0
.short 0
_kernel_start:

#ifdef START_IN_REALM	// only if not done in loader
.code16

cli
ljmp $KERNEL_SEG,$(_here-KERNEL_ADDR)		/* mandatory : reset offset */
//ljmp $KERNEL_SEG,$_here		/* mandatory : reset offset */

_here:
push %cs
pop %ds				/* mandatory : for lgdt to reach gdt */
//lgdt gdt
lgdt gdt-KERNEL_ADDR
movl %cr0,%eax
orb $1,%al
movl %eax,%cr0
mov $0x10,%ax
mov %ax,%ds
mov %ax,%es
mov %ax,%fs
mov %ax,%gs
.code32
.byte 0x66
//ljmp $0x8,$(KERNEL_ADDR+(Here32))
ljmp $0x8,$((Here32))

#endif

.code32

_kernel_entry:
Here32:
mov $0xbeefdead,%eax
#if 1
mov $0x10,%eax
mov %eax,%ss
mov $(stack + STACK_SIZE),%ebp
mov %ebp,%esp
#endif

call kernel_main

mov $0xdeadbeef,%eax
inf:
	hlt
	jmp inf

#if 1
.align 4
#define MAGIC 0x1badb002
#define FLAGS 0x00010003
/* multiboot header */
magic: .long MAGIC
flags: .long FLAGS
chksm: .long -(MAGIC+FLAGS)
hdrad: .long magic
ldadr: .long KERNEL_ADDR
ldead: .long KERNEL_ADDR+_end
bssea: .long 0x0
entry: .long _kernel_entry
#endif

gdt:
NULL_Desc:
.word EndGDT-gdt-1
//.long gdt+KERNEL_ADDR
.long gdt
.word 0
CS_Desc:
.word 0xFFFF,0
.byte 0,0x9B,0xCF,0
DS_Desc:
.word 0xFFFF,0
.byte 0,0x93,0xCF,0
EndGDT:
nop;nop;nop
kernel_end:
