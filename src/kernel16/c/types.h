#ifndef __TYPES_H__
#define __TYPES_H__

#define NULL 0

typedef unsigned long size_t;

// model i/l/p

// ILP16 2/4/2

//  LP32 2/4/4	// pc world
// ILP32 4/4/4

//  LP64 4/8/8
// ILP64 8/8/8
// LLP64 4/4/8 (new type : long long)

typedef unsigned long uint32_t;
typedef unsigned short uint16_t;
typedef unsigned char uint8_t;
typedef uint16_t uintptr_t;

#endif/*__TYPES_H__*/
