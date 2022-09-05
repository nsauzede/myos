#ifndef __LIBC_H__
#define __LIBC_H__

#include "types.h"

#include <stdarg.h>

#define memset mmemset
#define strlen mstrlen
#define puts dputs
#define printf mprintf
#define vsnprintf mvsnprintf
#define strncmp mstrncmp

void *mmemset(void *_s, int c, size_t n);
size_t mstrlen(const char *s);
void dputs(const char *s);
int mprintf(const char *fmt, ...) asm ("mprintf");
int mvsnprintf(char *str, size_t size, const char *fmt, va_list ap) asm ("mvsnprintf");
int mstrncmp(const char *s1, const char *s2, size_t n);

#endif/*__LIBC_H__*/
