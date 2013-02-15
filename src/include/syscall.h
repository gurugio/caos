
#ifndef __SYSCALL_H__
#define __SYSCALL_H__


#define SYSCALL_VECTOR 0x80



#define __NR_sys_syscall_test 0
#define __NR_sys_cputstr 1
#define __NR_sys_cgetstr 2
#define __NR_sys_cprintf 3

#define __NR_sys_system_info 4
#define __NR_sys_turnoff 5
#define __NR_sys_open 6
#define __NR_sys_close 7
#define __NR_sys_read 8
#define __NR_sys_write 9

extern int sys_call_max;



#endif
