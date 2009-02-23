#include "libc.h"

#include "vid.h"
#include "ioport.h"

#define IODELAY() asm volatile( ".byte 0x90,0x90,0x90,0x90,0x90,0x90,0x90,0x90")

static int col = 0;
static int row = 0;
static unsigned char attr = 0;

void setattr( int _attr)
{
    attr = _attr;
}

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
            ptr[(row * 80 + col) * 2] = *s;
            ptr[(row * 80 + col) * 2 + 1] = attr;
            if (col < MAX_COL)
                col++;
        }
        s++;
    }
}

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
	void _setmode03();
	_setmode03();
}

void setmode13( void)
{
	void _setmode13();
	_setmode13();
	memset( (void *)0xa0000,0,320*200);
}
