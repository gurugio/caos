#ifndef __TYPES_H__

#define __TYPES_H__

#define NULL (void *)0



//
// x86_64 processor saves function-parameters in registers,
// so that gcc built-in functions are used like this.
//
typedef __builtin_va_list va_list;
#define va_start(v,l) __builtin_va_start(v,l)
#define va_arg(v,l) __builtin_va_arg(v,l)
#define va_end(v) __builtin_va_end(v)


#define asmlinkage __attribute__((regparm(0)))


#define BITS_PER_LONG 64
#define BYTES_PER_LONG 8
#define BITS_PER_BYTE 8



#define BITS_TO_LONGS(bits) \
	(((bits)+BITS_PER_LONG-1)/BITS_PER_LONG)
#define DECLARE_BITMAP(name,bits) \
	unsigned long name[BITS_TO_LONGS(bits)]


typedef signed char s8;
typedef unsigned char u8;

typedef signed short s16;
typedef unsigned short u16;

typedef signed int s32;
typedef unsigned int u32;

typedef signed long s64;
typedef unsigned long u64;


// coungting, index of something..
typedef unsigned long size_t;
typedef signed long ssize_t;


// flag type for bit-attributes
typedef unsigned long flag_t;



// for atomic operations
typedef struct {
	volatile long counter;	// long type for 64bit
} atomic_t;


#define CAOS_TRUE 1
#define CAOS_FALSE 0

#define CAOS_SUCCESS 0


#endif
