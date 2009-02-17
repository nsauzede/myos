#ifndef __TYPES_H__
#define __TYPES_H__

typedef unsigned long size_t;

#ifdef _LP64
#error You must implement integer types for your architecture
#else
typedef unsigned long uint32_t;
typedef unsigned short uint16_t;
typedef unsigned char uint8_t;
typedef uint32_t uintptr_t;
#endif

#endif/*__TYPES_H__*/
