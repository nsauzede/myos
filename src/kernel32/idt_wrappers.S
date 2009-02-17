#include "idt.h"

.extern idt_handlers
.globl idt_wrappers

.irp id,EXCEPTION_DIVIDE
.p2align 2,0x90
exc_wrapper_\id:
push $0
push %ebp
mov %esp,%ebp
push %edi
push %esi
push %edx
push %ecx
push %ebx
push %eax
sub $2,%esp
push %ss
push %ds
push %es
push %fs
push %gs
push $\id
lea idt_handlers,%edi
call *\id*4(%edi)
add $4,%esp
pop %gs
pop %fs
pop %es
pop %ds
pop %ss
add $2,%esp
pop %eax
pop %ebx
pop %ecx
pop %edx
pop %esi
pop %edi
pop %ebp
add $4,%esp
iret
.endr

.irp id,IRQ_TIMER,IRQ_KB
.p2align 2,0x90
irq_wrapper_\id:
push $0
push %ebp
mov %esp,%ebp
push %edi
push %esi
push %edx
push %ecx
push %ebx
push %eax
sub $2,%esp
push %ss
push %ds
push %es
push %fs
push %gs
mov $0x20,%al
outb %al,$0x20
push $\id
lea idt_handlers,%edi
call *\id*4(%edi)
add $4,%esp
pop %gs
pop %fs
pop %es
pop %ds
pop %ss
add $2,%esp
pop %eax
pop %ebx
pop %ecx
pop %edx
pop %esi
pop %edi
pop %ebp
add $4,%esp
iret
.endr

.irp id,EXCEPTION_DOUBLE
.p2align 2,0x90
exc_wrapper_\id:
1: hlt
jmp 1b
.endr

idt_wrappers:
.irp id,0
.long exc_wrapper_\id
.endr

.irp id,1,2,3,4,5,6,7
.long 0
.endr

.irp id,8
.long exc_wrapper_\id
.endr

.irp id,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31
.long 0
.endr

.irp id,32,33
.long irq_wrapper_\id
.endr
