#include "idt.h"

.extern idt_handlers
.globl idt_wrappers
.globl def_int_wrappers

/* wrapper for exceptions without error code */
.irp id,0,1,2,3,4,5,6,7,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31
.p2align 2,0x90
exc_wrapper_\id:
					// eflags	4 4
					// cs		4 8
					// eip		4 12
push $0				// error	4 16		// fake error code
push %ebp			// ebp		4 20
mov %esp,%ebp		// 0(ebp)=ebp 4(ebp)=error 8(ebp)=eip 12(ebp)=cs 14(ebp)=eflags
push %edi
push %esi
push %edx
push %ecx
push %ebx
push %eax
push %ss
push %ds
push %es
push %fs
push %gs
push %ebp			// provide SEH params
push $\id
lea idt_handlers,%edi
call *\id*4(%edi)
add $8,%esp			// pop id+ebp
pop %gs
pop %fs
pop %es
pop %ds
pop %ss
pop %eax
pop %ebx
pop %ecx
pop %edx
pop %esi
pop %edi
pop %ebp
add $4,%esp		// remove fake error code
iret
.endr

/* wrapper for exceptions with error code */
.irp id,8
.p2align 2,0x90
exc_wrapper_\id:
					// eflags	4 4
					// cs		4 8
					// eip		4 12
					// error	4 16
push %ebp			// ebp		4 20
mov %esp,%ebp		// 0(ebp)=ebp 4(ebp)=error 8(ebp)=eip 12(ebp)=cs 14(ebp)=eflags
push %edi
push %esi
push %edx
push %ecx
push %ebx
push %eax
push %ss
push %ds
push %es
push %fs
push %gs
push %ebp			// provide SEH params
push $\id
lea idt_handlers,%edi
call *\id*4(%edi)
add $8,%esp
pop %gs
pop %fs
pop %es
pop %ds
pop %ss
pop %eax
pop %ebx
pop %ecx
pop %edx
pop %esi
pop %edi
pop %ebp
add $4,%esp			// remove error code
iret
.endr

/* wrapper irqs without error code */
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
push %ebp
push $\id
lea idt_handlers,%edi
call *(IRQ_BASE+\id)*4(%edi)
add $8,%esp
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
cli

#if 1
movl $0xCF44CF2A,0xb8000
movl $0xCF4CCF42,0xb8004
movl $0xCF4CCF46,0xb8008
movl $0xCF2ACF54,0xb800c
#elif 1
mov %esp,%ecx
mov (%esp),%edx
mov 4(%esp),%ebx
mov $dblflt_stack-4,%eax
mov %eax,%esp
push %ebx
push %edx
push %ecx
push $msg
call home
push $0x0C
call setattr
pop %eax
call mprintf
pop %eax
pop %eax
pop %eax
pop %eax
push $0
push $0
call setcursor
#endif
1:
hlt
jmp 1b
.endr
#endif

def_int_stack0:
.string "EEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEE"
def_int_stack:
def_int_msg:
.string "int #%x : esp=%p : %p %p %p %p"
.irp id,0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32,33
.p2align 2,0x90
def_int_wrapper_\id:
mov %esp,%ebp
mov $def_int_stack-4,%esp
push 12(%ebp)
push 8(%ebp)
push 4(%ebp)
push (%ebp)
push %ebp
push $\id
push $def_int_msg
call home
push $0x0C
call setattr
pop %eax
call mprintf
pop %eax
pop %eax
pop %eax
pop %eax
push $0
push $0
call setcursor
1:
hlt
jmp 1b
.endr

.section .rodata
.p2align 5,0x0
idt_wrappers:
.irp id,0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31
.long exc_wrapper_\id
.endr

irq_wrappers:
.irp id,0,1
.long irq_wrapper_\id
.endr

def_int_wrappers:
.irp id,0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32,33
.long def_int_wrapper_\id
.endr
