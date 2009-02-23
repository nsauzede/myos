#include <stdarg.h>

#include "libc.h"

#include "vid.h"

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

static char *hextostr( char *_s, void *ptr, int _len)
{
	int len = _len;
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
		
		s = _s;
		int skip = 0;
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
		if (skip)
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
				case 'x':
					ptr = va_arg( ap, typeof( ptr));
//					dputs( "0x");
					hextostr( buf, &ptr, sizeof( ptr));
					dputs( buf);
					break;
#if 0
				case 'd':
					ptr = va_arg( ap, typeof( ptr));
					dectostr( buf, &ptr, sizeof( ptr));
					dputs( buf);
					break;
#endif
			}
		}
		else
			dputchar( *fmt);
		fmt++;
	}
	va_end( ap);

	return 0;
}
