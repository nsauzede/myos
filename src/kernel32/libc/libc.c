#include <stdarg.h>

#include "libc.h"

#define MAX_ROW 25
#define MAX_COL 80
#define DEFAULT_ATTR	0x7
static int col = 0;
static int row = 0;
static unsigned char attr = 0;

void *mmemset( void *_s, int c, size_t n)
{
	void *s = _s;
	
	if (s)
	{
		while (n--)
		{
			*(unsigned char *)s++ = (unsigned char)c;
		}
	}
	
	return _s;
}

void console_init()
{
	col = 0;
	row = 0;
	attr = DEFAULT_ATTR;
#if 0
	void setmode03();
	setmode03();
#elif 0
	void setmode13();
	setmode13();
	unsigned char *screen = (void *)0xA0000;
	screen[320 * 0 + 0] = 0x0;
	screen[320 * 0 + 1] = 0x5;
	screen[320 * 0 + 2] = 0x0;
	screen[320 * 1 + 0] = 0x5;
	screen[320 * 1 + 1] = 0x0;
	screen[320 * 1 + 2] = 0x5;
	screen[320 * 2 + 0] = 0x0;
	screen[320 * 2 + 1] = 0x5;
	screen[320 * 2 + 2] = 0x0;
#endif
}

void cls()
{
	col = 0;
	row = 0;
	attr = DEFAULT_ATTR;
	memset( (void *)0xb8000, 0, MAX_ROW * MAX_COL * 2);
}

static int dputchar( int c)
{
	unsigned char *ptr = (void *)0xB8000;

		int skip = 0;

		if (c == '\n')
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

static char *hextostr( char *_s, void *ptr, int len)
{
	char *s = _s + len * 2;
	if (s && ptr)
	{
		*s-- = 0;
		while (len)
		{
			char c = *(char *)ptr++;
			char digit;

			digit = c & 0xf;
			*s-- = digit >= 10 ? digit + 'A' - 10 : digit + '0';
			digit = (c >> 4) & 0xf;
			*s-- = digit >= 10 ? digit + 'A' - 10 : digit + '0';
			len--;
		}
	}
	
	return _s;
}

int mprintf( const char *fmt, ...)
{
	char buf[20];
	va_list ap;
	void *ptr;
	
	memset( buf, '?', sizeof( buf) - 1);
	va_start( ap, fmt);
	if (fmt)
	while (*fmt)
	{
		if (*fmt == '%')
		{
			fmt++;
			switch (*fmt)
			{
				case 'p':
					ptr = va_arg( ap, typeof( ptr));
					dputs( "0x");
					hextostr( buf, &ptr, sizeof( ptr));
					dputs( buf);
					break;
			}
		}
		else
			dputchar( *fmt);
		fmt++;
	}
	va_end( ap);

	return 0;
}
