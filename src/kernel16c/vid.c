/*
#include "libc.h"
#include "ioport.h"
#include "vid.h"
*/


#define IODELAY() asm ("db 0x90,0x90,0x90,0x90,0x90,0x90,0x90,0x90")

static int col = 0;
static int row = 0;
static unsigned char attr = 0;


void setcursor(x, y)
int x; int y;
{
	int loc;
	unsigned char byte;

	loc = y * 80 + x;
/*	outb(0xe, 0x3d4);*/
	IODELAY();
	byte = (loc >> 8) & 0xFF;
/*	outb(byte, 0x3d5);
	outb(0xf, 0x3d4);*/
	IODELAY();
	byte = loc & 0xFF;
/*	outb(byte, 0x3d5);*/
}

void setmode03()
{
/*
	void _setmode03();
	_setmode03();
*/
}

void setmode13()
{
/*
	void _setmode13();
	_setmode13();
	memset((void *)0xa000, 0,0,320*200);
*/
}
/*
//void setattr(int _attr)
//void gotoxy(int x, int y)
//int dputchar(int c)
//void dputs(const char *s)
//void setcursor(int x, int y)
//void setmode03(void)
//void setmode13(void)
*/
void setattr(_attr)
int _attr;
{
    attr = _attr;
}

void home()
{
	gotoxy(0, 0);
}

void gotoxy(x, y)
int x; int y;
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
/*
	memset((void *)0xb800, 0, attr, MAX_ROW * MAX_COL * 2);
*/
}

int dputchar(c)
int c;
{
	unsigned char *ptr = (void *)0xB800;
	
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

void dputs(s)
char *s;
{
    unsigned char *ptr = (void *)0xB800;
    
    if (s)
    while (*s)
    {
        dputchar(*s++);
    }
}
