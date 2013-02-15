
#ifndef	__PROCESS_H__
#define	__PROCESS_H__


#include <types.h>

/* declare process's statements... */
#define	PROCESS_RUNNING				1
#define	PROCESS_INTERRUPTIBLE		2
#define	PROCESS_UNINTERRUPTIBLE		3
#define	PROCESS_STOPPED				4


/* process table... */
struct task_struct
{
	ssize_t status;
	size_t pid;
	char name[16];

	u64 cr3;

	//struct thread_struct thread;
	
};




#endif
