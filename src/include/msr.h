

#ifndef __MSR_H__
#define __MSR_H__ 1



static inline void cpuid(int op, unsigned int *eax, unsigned int *ebx,
		unsigned int *ecx, unsigned int *edx)
{

	__asm__("cpuid"
			: "=a" (*eax), "=b" (*ebx), "=c" (*ecx), "=d" (*edx)
			: "0" (op)
		   );

}




#endif

