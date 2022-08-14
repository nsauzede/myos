#ifndef __TYPES_H__
#define __TYPES_H__


#ifdef _LP64
#error You must implement integer types for your architecture
#endif
typedef unsigned long uint32_t;
typedef unsigned short uint16_t;
typedef unsigned char uint8_t;

typedef unsigned long uintptr_t;
typedef unsigned long size_t;

#endif/*__TYPES_H__*/
