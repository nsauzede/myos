#ifndef __LIBC_H__
#define __LIBC_H__

#include "types.h"

//#define memset mmemset
//void *mmemset(/*void **/_s, /*void **/_o, /*int*/ c, /*size_t*/ n);

void console_init();
void cls();

#define BLACK 0 
#define RED 0xC 
#define GREEN 0x2 
#define BG_BLACK (BLACK << 4) 
#define FG_RED (RED) 
#define FG_GREEN (GREEN) 
void setattr(/*int*/ attr);

#define puts dputs
void dputs(/*const char **/s);

#define printf mprintf
int mprintf(/*const char **/fmt /*, ...*/);

#endif/*__LIBC_H__*/
