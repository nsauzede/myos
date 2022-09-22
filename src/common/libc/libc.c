#include <stdarg.h>

#include "libc.h"
#include "vid.h"        // dputchar, ...

void *mmemset(void *_s, int c, size_t n) {
    void *s = _s;

    if (s) {
        while (n--) {
            *(unsigned char *)s++ = (unsigned char)c;
        }
    }

    return _s;
}

void *mmemcpy(void *dest, const void *src, size_t n) {
    if (dest && src) {
    void *d = dest;
        while (n--) {
            *(unsigned char *)d++ = *(unsigned char *)src++;
        }
    }
    return dest;
}

size_t mstrlen(const char *s) {
    size_t result = 0;
    if (s)
    while (*s++) {
        result++;
    }
    return result;
}

int mstrncmp(const char *s1, const char *s2, size_t n) {
    for (int i = 0; i < n; i++) {
        if (s1[i] > s2[i])
            return 1;
        if (s1[i] < s2[i])
            return -1;
    }
    return 0;
}

enum {
    FLAG_HASH = 1 << 0,
    FLAG_ZERO = 1 << 1,
    FLAG_MINUS = 1 << 2,
    FLAG_SPACE = 1 << 3,
    FLAG_PLUS = 1 << 4,
    FLAG_UPPER = 1 << 5,
};
// size: max number of bytes to write to str
// width: desired number of bytes written
static int number(char *str, size_t size, unsigned long num, int base, int width, int flags) {
    size_t written = 0, ofs;
    int n = 0;
    if (base == 16) {
        int upper = flags & FLAG_UPPER;
        if (flags & FLAG_HASH) {
            str[written++] = '0';
            str[written++] = upper ? 'X' : 'x';
        }
        for (int v = num; ;) {
            n++;
            v = v / base;
            if (!v) break;
        }
        ofs = written;
        if (width > n) ofs += width - n;
        for (; n > 0;) {
            written++;
            int d = num % base;
            str[ofs + --n] = d >= 10 ? d + (upper ? 'A' : 'a') - 10 : d + '0';
            num /= base;
        }
    } else {
        base = 10;
#if 0
        if (written < (size - 1) && num < 0) {
            str[written++] = '-';
            num = -num;
        }
#endif
        for (int v = num; ;) {
            n++;
            v = v / base;
            if (!v) break;
        }
        ofs = written;
        if (width > n) ofs += width - n;
        for (; n > 0;) {
            written++;
            int d = num % base;
            str[ofs + --n] = d + '0';
            num /= base;
        }
    }
    char filler = flags & FLAG_ZERO ? '0' : flags & FLAG_HASH ? 0 : ' ';
    for (; filler && ofs > 0;) {
        written++;
        str[--ofs] = filler;
    }
    if (written < size)
        str[written] = 0;
    return written;
}

int mvsnprintf(char *str, size_t size, const char *format, va_list ap) {
    void *ptr;
    int chr;
    int num;
    size_t written = 0;
    if (!str || !format) return 0;
    while (*format && written < (size - 1)) {
        int width = 0;
        int flags = 0;
        if (*format == '%') {
            while (written < size) {
                format++;
                switch (*format) {
                    case 'l':
                        continue;
                    case '0':
                        flags |= FLAG_ZERO;
                        continue;
                    case '#':
                        flags |= FLAG_HASH;
                        continue;
                    case '1'...'9':
                        width = *format - '0';
                        continue;
                    case 'X':
                        flags |= FLAG_UPPER; /* fallthrough */
                    case 'p':
                    case 'x':
                        num = va_arg(ap, typeof(num));
                        written += number(str + written, size - written, num, 16, width, flags);
                        break;
                    case 'c':
                        chr = va_arg(ap, typeof(chr));
                        str[written++] = chr;
                        break;
                    case 's': {
                        ptr = va_arg(ap, typeof(ptr));
                        int len = strlen(ptr);
                        if (written + len > size) {
                            len = size - written;
                        }
                        memcpy(str + written, ptr, len);
                        written += len;
                        break;
                    }
                    case 'd':
                        num = va_arg(ap, typeof(num));
                        written += number(str + written, size - written, num, 10, width, flags);
                        break;
                }
                break;
            }
        }
        else
            str[written++] = *format;
        format++;
    }
    if (written < size) {
        str[written] = 0;
    }
    return written;
}

int mprintf(const char *fmt, ...) {
    int result;
    va_list ap;
    va_start(ap, fmt);
    static char buf[1024];
    memset(buf, 'X', sizeof(buf));
    result = vsnprintf(buf, sizeof(buf), fmt, ap);
    va_end(ap);
    dputs(buf);
    return result;
}
