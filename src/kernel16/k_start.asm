.text
use16 86
;org 0x7e00
org 0x0
;.globl kernel_start
kernel_start:
db 1    ; total number of kernel sectors
db 0
dw 0
dw 0
_kernel_start:
jmp _kernel_main
