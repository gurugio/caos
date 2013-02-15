
#ifndef __PRINTF_H__

#define __PRINTF_H__

#include <types.h>


void caos_putchar(char);
void caos_delchar(ssize_t cnt);
ssize_t asmlinkage caos_printf(const char *format, ...);
void asmlinkage caos_vsprintf(char *A_szString, const char *A_szFormat, va_list A_pAp);
ssize_t print_decimal(char *A_szBuffer, ssize_t a);
ssize_t print_hex(char *buf, ssize_t a);

ssize_t asmlinkage caos_sprintf(char *outbuf, const char *format, ...);

#endif
