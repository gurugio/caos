/********************************************************************
 * DESCRIPTION : port I/O
 * FILE NAME : IO.h
 *******************************************************************/

#ifndef __IO_H__

#define __IO_H__


#include <pgtable.h>
#include <types.h>


#define __outb(p,v) __asm__ __volatile__ ("outb %b0,%w1"::"a"(v),"Nd"(p))

static inline u64 virt_to_phys(volatile void *address)
{
	return __pa(address);
}

static inline void *phys_to_virt(u64 address)
{
	return __va(address);
}


/*
 * gcc inline assembly indentifier
 * Output/Input list
 * = : value will be changed
 * a : ax
 * d : dx
 * N : 0~255
 */

/****************************************************************
 * NAME : inb
 * SYNOPSIS : in byte
 * EXTERNAL EFFECTS : N/A
 * ARGUMENTS : port  port number 
 * RETURNS : _v read value from AL
 * ERRNOS : N/A
 ***************************************************************/
static inline unsigned char inb(unsigned short port)
{
	unsigned char _v;
	__asm__ __volatile__ ("inb %w1, %0":"=a" (_v):"Nd" (port));
	return _v;
}

/****************************************************************
 * NAME : inw
 * SYNOPSIS : in word
 * EXTERNAL EFFECTS : N/A
 * ARGUMENTS : port  port number
 * RETURNS : _v read value from word
 * ERRNOS : N/A
 ***************************************************************/
static inline unsigned short inw(unsigned short port)
{
	unsigned short _v;
	__asm__ __volatile__ ("inw %w1,%0":"=a" (_v):"Nd"(port));
	return _v;
}

/****************************************************************
 * NAME : outb
 * SYNOPSIS : out byte
 * EXTERNAL EFFECTS : N/A
 * ARGUMENTS : port  port number
 *             value  write value
 * RETURNS : N/A
 * ERRNOS : N/A
 ***************************************************************/
static inline void outb(unsigned short port, unsigned char value)
{
	__asm__ __volatile__ ("outb %b0,%w1": :"a" (value), "Nd"(port));
}

/****************************************************************
 * NAME : outw
 * SYNOPSIS : out word
 * EXTERNAL EFFECTS : N/A
 * ARGUMENTS : port  port number
 *             value write value
 * RETURNS : N/A
 * ERRNOS : N/A
 ***************************************************************/
static inline void outw(unsigned short port, unsigned short value)
{
	__asm__ __volatile__ ("outw %w0,%w1": :"a" (value), "Nd"(port));
}




/*
 * readX/writeX() are used to access memory mapped devices. On some
 * architectures the memory mapped IO stuff needs to be accessed
 * differently. On the x86 architecture, we just read/write the
 * memory location directly.
 */

static inline u8 __readb(const volatile void  *addr)
{
	return *( volatile u8 *)addr;
}
static inline u16 __readw(const volatile void  *addr)
{
	return *( volatile u16 *)addr;
}
static inline u32 __readl(const volatile void  *addr)
{
	return *( volatile u32 *)addr;
}
static inline u64 __readq(const volatile void  *addr)
{
	return *( volatile u64 *)addr;
}
#define readb(x) __readb(x)
#define readw(x) __readw(x)
#define readl(x) __readl(x)
#define readq(x) __readq(x)

static inline void __writel(u32 b, volatile void  *addr)
{
	*( volatile u32 *)addr = b;
}
static inline void __writeq(u64 b, volatile void  *addr)
{
	*( volatile u64 *)addr = b;
}
static inline void __writeb(u8 b, volatile void  *addr)
{
	*( volatile u8 *)addr = b;
}
static inline void __writew(u16 b, volatile void  *addr)
{
	*( volatile u16 *)addr = b;
}
#define writeq(val,addr) __writeq((val),(addr))
#define writel(val,addr) __writel((val),(addr))
#define writew(val,addr) __writew((val),(addr))
#define writeb(val,addr) __writeb((val),(addr))




#endif
