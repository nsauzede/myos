#include <varargs.h>

#include "libc.h"

#include "vid.h"

void *mmemset( _s, _o, c, n)
void *_s; void *_o; int c; size_t n;
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

//static char *hextostr( char *_s, void *ptr, int _len, int fixed)
static char *hextostr( _s, ptr, _len, fixed)
char *_s; void *ptr; int _len; int fixed;
{
	int len = _len;
	char *s = _s + len * 2;
	if (s && ptr)
	{
		int skip = 0;
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
		
		s = _s;
		while (*s)
		{
			if (*s != '0')
				break;
			skip++;
			s++;
		}
		s = _s;
		if (skip == (_len * 2))
			skip--;
		if (skip && !fixed)
		while (1)
		{
			*s = *(s + skip);
			if (!*s)
				break;
			s++;
		}
	}
	
	return _s;
}

//int mprintf( const char *fmt, ...)
int mprintf( fmt, va_alist)
/*const ??*/ char *fmt;
va_dcl
{
	char buf[20];
	va_list ap;
	void *ptr;
	int chr;
	
	memset( buf, '?', sizeof( buf) - 1);
	va_start( ap);
	if (fmt)
	while (*fmt)
	{
		int upper = 0;
		int size = -1;
		int fixed = 0;
		if (*fmt == '%')
		{
			while (1)
			{
				fmt++;
				switch (*fmt)
				{
					case 'l':
						continue;
//					case '0'...'9':
case'0':case'1':case'2':case'3':case'4':case'5':case'6':case'7':case'8':case'9':
						size = *fmt - '0';
						fixed = 1;
						continue;
						break;
					case 'X':
						upper = 1;
					case 'p':
					case 'x':
//						ptr = va_arg( ap, typeof( ptr));
						ptr = va_arg( ap, void *);
//						dputs( "0x");
						if ((size < 0) || (size > (sizeof( ptr))))
							size = sizeof( ptr);
						else
						{
							size /= 2;
						}
						hextostr( buf, &ptr, size, fixed);
						dputs( buf);
						break;
					case 'c':
//						chr = va_arg( ap, typeof( chr));
						chr = va_arg( ap, int);
						dputchar( chr);
						break;
					case 's':
//						ptr = va_arg( ap, typeof( ptr));
						ptr = va_arg( ap, void *);
						dputs( ptr);
						break;
#if 0
					case 'd':
//						ptr = va_arg( ap, typeof( ptr));
						ptr = va_arg( ap, void *);
						dectostr( buf, &ptr, sizeof( ptr));
						dputs( buf);
						break;
#endif
				}
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
