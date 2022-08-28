.text
use16 86
;kernel_start:
;.globl kernel_start
;nop
kernel_end:
.blkb 512 - (kernel_end - kernel_start) - 1
;.blkb 13
nop
