#ifndef KERNEL_ADDR
//#define KERNEL_ADDR 0x7E00
#endif
#define KERNEL_SEG (KERNEL_ADDR >> 4)
#define STACK_SIZE 0x4000
#define START_IN_REALM
#ifdef START_IN_REALM
#define IS64BIT 0
#else
#define IS64BIT 2
#endif
.org 0				// real org will be set by elf linker, just zero it now
kernel_start:
.byte 64				// FIXME : this is a big problem - the kernel size should be computed !!!
.byte IS64BIT
.short 0
.short 0
_kernel_start:

#ifdef START_IN_REALM	// only if not done in loader
// This direct-switch-to-long-mode code heavily inspired from https://wiki.osdev.org/Entering_Long_Mode_Directly
#define FREE_SPACE      0x3000          // paging table up to 0x7000
#define PAGE_PRESENT    (1 << 0)
#define PAGE_WRITE      (1 << 1)
#define CODE_SEG        0x0008
#define DATA_SEG        0x0010

.code16
EnterLongModeDirectly:
.start:
ljmp $0,$.FlushCS
.FlushCS:
xor %eax,%eax
mov %ax,%ss
mov $0x3000,%sp         // below paging tables
mov %ax,%ds
mov %ax,%es
mov %ax,%fs
mov %ax,%gs
mov $FREE_SPACE,%edi
push %di
mov $0x1000,%ecx
cld
rep stos %eax,%es:(%di)
pop %di
lea 0x1000(%di),%eax                        // PML4
or $(PAGE_PRESENT | PAGE_WRITE),%eax
mov %eax,%es:(%di)
lea 0x2000(%di),%eax                        // PDPT
or $(PAGE_PRESENT | PAGE_WRITE),%eax
mov %eax,%es:0x1000(%di)
lea 0x3000(%di),%eax                        // PDT
or $(PAGE_PRESENT | PAGE_WRITE),%eax
mov %eax,%es:0x2000(%di)
push %di
lea 0x3000(%di),%di                         // PT
mov $(PAGE_PRESENT | PAGE_WRITE),%eax
.LoopPageTable:
mov %eax,%es:(%di)
add $0x1000,%eax
add $8,%di
cmp $0x200000,%eax
jb .LoopPageTable
pop %di
mov $0xff,%al
out %al,$0xa1
out %al,$0x21
nop
nop
lidt (IDT)
lgdt (GDT.Pointer)

mov $DATA_SEG,%ax
mov %ax,%ds
mov %ax,%es
mov %ax,%fs
mov %ax,%gs
mov %ax,%ss
mov $0b10100000,%eax
mov %eax,%cr4
mov %edi,%cr3
mov $0xc0000080,%ecx
rdmsr
or $0x00000100,%eax
wrmsr
mov %cr0,%ebx
or $0x80000001,%ebx
mov %ebx,%cr0
ljmp $CODE_SEG,$LongMode

GDT:
.Null:
.quad 0x0000000000000000
.Code:
.quad 0x00209A0000000000
.quad 0x0000920000000000

//.align 4
.byte 0x90,0x90,0x90
.word 0

GDT.Pointer:
.word GDT.Pointer-GDT-1
.long GDT

.align 4
IDT:
.Length:        .word 0
.Base:          .long 0

#endif

.align 16
.code64

LongMode:
// enable A20
in $0x92,%al
or $2,%al
out %al,$0x92

_kernel_entry:
Here64:
mov $0xdeadbeefac1dc0de,%rax
#if 0
mov $0x10,%eax
mov %eax,%ss
mov $(stack + STACK_SIZE),%rbp
mov %rbp,%rsp
#endif

call kernel_main

mov $0xac1dc0dedeadbeef,%rax

inf:
	hlt
	jmp inf

/*
#if 1
.align 4
#define MAGIC 0x1badb002
#define FLAGS 0x00010003
// multiboot header
magic: .long MAGIC
flags: .long FLAGS
chksm: .long -(MAGIC+FLAGS)
hdrad: .long magic
ldadr: .long KERNEL_ADDR
ldead: .long KERNEL_ADDR+_end
bssea: .long 0x0
entry: .long _kernel_entry
#endif
*/

.quad 0,0,0,0,0,0,0,0,0,0,0,0,0,0

kernel_end:
