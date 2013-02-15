
#ifndef __USER_SYSCALL_H__
#define __USER_SYSCALL_H__


#include "syscall.h"



//
// definitions of system-call library
//
int sys_syscall_test(void);				// 0
int sys_cputstr(const char *);			// 1
int sys_cgetstr(const char *, long);	// 2
int sys_cprintf(const char *, long);	// 3
int sys_system_info(void);				// 4
int sys_turnoff(void);					// 5

int sys_open(const char *, int);	// 6
int sys_close(int);					// 7
int sys_read(int, char *, int);		// 8
int sys_write(int, char *, int);	// 9


#define _syscall0(type, name)\
inline type name(void)\
{\
	long __res;\
	asm volatile (\
		"int $0x80\n\t"\
		:"=a"(__res)\
		:"a"(__NR_##name)\
	);\
	return __res;\
}


#define _syscall1(type, name, type1, arg1)\
inline type name(type1 arg1)\
{\
	long __res;\
	asm volatile (\
		"int $0x80\n\t"\
		:"=a"(__res)\
		:"a"(__NR_##name), "b"((long)arg1)\
	);\
	return __res;\
}


#define _syscall2(type, name, type1, arg1, type2, arg2)\
inline type name(type1 arg1, type2 arg2)\
{\
	long __res;\
	asm volatile (\
		"int $0x80\n\t"\
		:"=a"(__res)\
		:"a"(__NR_##name), "b"((long)arg1), "c"((long)arg2)\
	);\
	return __res;\
}

#define _syscall3(type, name, type1, arg1, type2, arg2, type3, arg3)\
inline type name(type1 arg1, type2 arg2, type3 arg3)\
{\
	long __res;\
	asm volatile (\
		"int $0x80\n\t"\
		:"=a"(__res)\
		:"a"(__NR_##name), "b"((long)arg1), "c"((long)arg2), "d"((long)arg3)\
	);\
	return __res;\
}



#endif
