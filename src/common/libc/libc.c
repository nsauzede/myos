#include <stdarg.h>

#include "libc.h"
#include "vid.h"        // dputchar

void *mmemset( void *_s, int c, size_t n) {
    void *s = _s;

    if (s) {
        while (n--) {
            *(unsigned char *)s++ = (unsigned char)c;
        }
    }

    return _s;
}

static char *dectostr( char *_s, int size, void *ptr) {
    char *s = _s;
    int v = *(int *)ptr;
    if (v < 0) {
        *s++ = '-';
        v = -v;
    }
    for (int v_ = v;;) {
        s++;
        v_ = v_ / 10;
        if (!v_) break;
    }
    *s-- = 0;
    for (;;) {
        int d = v % 10;
        *s-- = d + '0';
        v = v / 10;
        if (!v) break;
    }
    return _s;
}

static char *hextostr( char *_s, void *ptr, int _len, int fixed) {
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

int mprintf( const char *fmt, ...) {
	char buf[20];
	va_list ap;
	void *ptr;
	int chr;
	int num;
	
	memset( buf, '?', sizeof( buf) - 1);
	va_start( ap, fmt);
	if (fmt)
	while (*fmt)
	{
//		int upper = 0;	//TODO
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
					case '0'...'9':
						size = *fmt - '0';
						fixed = 1;
						continue;
						break;
					case 'X':
//						upper = 1;	//TODO
					case 'p':
					case 'x':
						ptr = va_arg( ap, typeof( ptr));
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
						chr = va_arg( ap, typeof( chr));
						dputchar( chr);
						break;
					case 's':
						ptr = va_arg( ap, typeof( ptr));
						dputs( ptr);
						break;
					case 'd':
						num = va_arg( ap, typeof(num));
						dectostr( buf, sizeof(buf), &num);
						dputs( buf);
						break;
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
