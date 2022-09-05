#ifndef __LIBC_H__
#define __LIBC_H__

#include "types.h"

#define memset mmemset
void *mmemset( void *_s, int c, size_t n);

#define puts dputs
void dputs( const char *s);

#define printf mprintf
int mprintf( const char *fmt, ...) asm ("mprintf");

#define strncmp mstrncmp
int mstrncmp(const char *s1, const char *s2, size_t n);

#endif/*__LIBC_H__*/
