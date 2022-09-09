#include "libc.h"

#include "vid.h"
#include "ioport.h"

static int col = 0;
static int row = 0;
static unsigned char attr = 0;

void setattr(int _attr) {
    attr = _attr;
}

int getattr() {
    return attr;
}

void home() {
    gotoxy(0, 0);
}

void gotoxy(int x, int y) {
    col = x;
    row = y;
}

void console_init() {
    attr = DEFAULT_ATTR;
    home();
//    cls();
}

void cls() {
#if 0
    // problem: cells with 0x00 as attr do not display the cursor
    // and using 0x20 instaead displays some green background
    memset((void *)0xb8000, 0, MAX_ROW * MAX_COL * 2);
#else
    // slow, but works, honoring the attr
    unsigned short *mem = (void *)0xb8000;
    for (int row = 0; row < MAX_ROW; row++) {
        for (int col = 0; col < MAX_COL; col++) {
            mem[MAX_COL * row + col] = (attr << 8) | 0x00;
        }
    }
#endif
}

int dputchar(int c) {
    unsigned char *ptr = (void *)0xB8000;

    int skip = 0;

    if ((c == '\n') || (c =='\r')) {
        col = 0;
        if (c == '\n')
        if (row < MAX_ROW) {
            row++;
        }
        skip = 1;
    }
    if (!skip) {
        if (col >= MAX_COL) {
            col = 0;
            row++;
        }
        ptr[(row * 80 + col) * 2] = c;
        ptr[(row * 80 + col) * 2 + 1] = attr;
        if (col < MAX_COL)
            col++;
    }

    return 0;
}

void dputs(const char *s) {
    unsigned char *ptr = (void *)0xB8000;

    if (s)
    while (*s) {
        int skip = 0;

        if (*s == '\n') {
            if (row < MAX_ROW) {
                col = 0;
                row++;
            }
            skip = 1;
        }
        if (!skip) {
            if (col >= MAX_COL) {
                col = 0;
                row++;
            }
            ptr[(row * 80 + col) * 2] = *s;
            ptr[(row * 80 + col) * 2 + 1] = attr;
            if (col < MAX_COL) {
                col++;
            }
        }
        s++;
    }
}

void setcursor(int x, int y) {
    int loc;
    unsigned char byte;

    loc = y * 80 + x;
    outb(0xf, 0x3d4);
    IODELAY();
    byte = loc & 0xFF;
    outb(byte, 0x3d5);
    outb(0xe, 0x3d4);
    IODELAY();
    byte = (loc >> 8) & 0xFF;
    outb(byte, 0x3d5);
}

void setmode03(void) {
    extern void _asm_setmode03() asm ("_asm_setmode03");
    _asm_setmode03();
}

void setmode13(void) {
    extern void _asm_setmode13() asm ("_asm_setmode13");
    _asm_setmode13();
    memset((void *)0xa0000, 0, 320 * 200);
}
