#include "idt.h"

.code64

.extern idt_handlers
.globl idt_wrappers
.globl def_int_wrappers

/* wrapper for exceptions without error code */
.irp id,0,1,2,3,4,5,6,7,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32,33
.p2align 2,0x90
exc_wrapper_\id:
cld
push %rax
push %rcx
push %rdx
push %rsi
push %rdi
push %r8
push %r9
push %r10
push %r11
lea idt_handlers,%rax
mov %rbp,%rsi
mov $\id,%rdi
// abi64: rdi rsi rdx rcx r8 r9
call *\id*8(%rax)
mov $0x20,%al
outb %al,$0x20
pop %r11
pop %r10
pop %r9
pop %r8
pop %rdi
pop %rsi
pop %rdx
pop %rcx
pop %rax
iretq
.endr

/* wrapper for exceptions with error code */
.irp id,8
.p2align 2,0x90
exc_wrapper_\id:
movw $0x0c41,0xb8000
1:
hlt
jmp 1b
.endr

/* wrapper irqs without error code */
.irp id,IRQ_TIMER,IRQ_KB
.p2align 2,0x90
irq_wrapper_\id:
movw $0x0c42,0xb8000
1:
hlt
jmp 1b
.endr

#if 0
dblflt_stack0:
.string "EEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEE"
dblflt_stack:
msg:
.string "DBLFLT@%p:%p %p"

.irp id,EXCEPTION_DOUBLE
.p2align 2,0x90
exc_wrapper_\id:
// print double fault message before infinite loop
movw $0x0c43,0xb8000
1:
hlt
jmp 1b
.endr
#endif

/*
Present on stack at entry: rip+ cs rflags rsp ss
*/
def_int_stack0:
.string "EEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEE"
def_int_stack:
def_int_msg:
.string "int #%x : esp=%p : %p %p %p %p %p"
.irp id,0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32,33
.p2align 2,0x90
def_int_wrapper_\id:
mov %rsp,%rbp
mov $def_int_stack,%rsp
push 8*4(%rbp)
push 8*3(%rbp)
push 8*2(%rbp)
push 8*1(%rbp)
push (%rbp)
push %rbp
push $\id
push $def_int_msg
mov $0x0C,%rdi
call setattr
call home
pop %rdi
pop %rsi
pop %rdx
pop %rcx
pop %r8
pop %r9
call mprintf
1:
hlt
jmp 1b
.endr
argh:
.string "ARGH def_int_wrapper_%p: %p %p %p"

.section .rodata
.p2align 5,0x0
idt_wrappers:
.irp id,0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32,33
.quad exc_wrapper_\id
.endr

irq_wrappers:
.irp id,0,1
.quad irq_wrapper_\id
.endr

def_int_wrappers:
.irp id,0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32,33
.quad def_int_wrapper_\id
.endr
