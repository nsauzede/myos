#if 1
#asm
.data           // workaround to force data section to follow text (!?)
org 0x0
kernel_start:
db 1            // total number of kernel sectors
db 0
dw 0
dw 0
push cs
pop ds
jmp _kernel_main
#endasm
#endif

void dputchar(char c) {
#asm
push bp
mov  bp, sp
mov bx,#1
mov ax,4[bp]
mov ah,#0x0E
int 0x10
pop bp
#endasm
}

void gotoxy(int row, int col) {
#asm
push bp
mov bp,sp
mov ah,#2
xor bh,bh
mov dh,[bp+6]
mov dl,[bp+4]
int 0x10
pop bp
#endasm
}

void home() {
    gotoxy(0, 0);
}

#define MAX_ROW (25)
#define MAX_COL (80)

void cls() {
#asm
push es
push di
mov di,#0xb800
mov es,di
xor di,di
mov cx,#MAX_ROW * MAX_COL * 2 / 2
mov ax,#0x0720
rep
stosw
pop di
pop es
#endasm
}

void console_init() {
    home();
    cls();
}

#define KERNEL_LOAD 0x7e00
#define PTR(ofs) (ofs+KERNEL_LOAD)

void dputs(char *s) {
    while (*s) {
        dputchar(*s++);
    }
}

#define printf dputs

void kernel_main()
{
    console_init();
    printf(PTR("\n\n"));
    printf(PTR("hello kernel16 - kernel_main=[%p]\n"), kernel_main);
    while (1) {
        asm ( "hlt");
    }
}

#asm
.data
//#include "k_end.asm"
#endasm
