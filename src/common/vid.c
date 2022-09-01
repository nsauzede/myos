#include "libc.h"

#include "vid.h"
#include "ioport.h"

static int col = 0;
static int row = 0;
static unsigned char attr = 0;

extern void setattr() asm ("setattr");
void setattr( int _attr)
{
    attr = _attr;
}

extern void home() asm ("home");
void home()
{
	gotoxy( 0, 0);
}

void gotoxy( int x, int y)
{
    col = x;
    row = y;
}

void console_init()
{
    attr = DEFAULT_ATTR;
	home();
	cls();
}

void cls()
{
	memset( (void *)0xb8000, 0, MAX_ROW * MAX_COL * 2);
}

int dputchar( int c)
{
    unsigned char *ptr = (void *)0xB8000;

        int skip = 0;

        if ((c == '\n') || (c =='\r'))
        {
            col = 0;
            if (c == '\n')
            if (row < MAX_ROW)
            {
                row++;
            }
            skip = 1;
        }
        if (!skip)
        {
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

void dputs( const char *s)
{
    unsigned char *ptr = (void *)0xB8000;
    
    if (s)
    while (*s)
    {
        int skip = 0;

        if (*s == '\n')
        {
            if (row < MAX_ROW)
            {
                col = 0;
                row++;
            }
            skip = 1;
        }
        if (!skip)
        {
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

extern void setcursor() asm ("setcursor");
void setcursor( int x, int y)
{
	int loc;
	unsigned char byte;

	loc = y * 80 + x;
	outb( 0xe, 0x3d4);
	IODELAY();
	byte = (loc >> 8) & 0xFF;
	outb( byte, 0x3d5);
	outb( 0xf, 0x3d4);
	IODELAY();
	byte = loc & 0xFF;
	outb( byte, 0x3d5);
}

void setmode03( void)
{
	extern void _asm_setmode03() asm ("_asm_setmode03");
	_asm_setmode03();
}

void setmode13( void)
{
	extern void _asm_setmode13() asm ("_asm_setmode13");
	_asm_setmode13();
	memset( (void *)0xa0000,0,320*200);
}
