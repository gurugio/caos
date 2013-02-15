#ifndef __STRING_H__

#define __STRING_H__

#include <types.h>


size_t caos_strcat(char *A_szDest, char *A_szSrc);
size_t caos_strcmp(const char *src, const char *dst);
size_t caos_strlen(const char *);
void *caos_memcpy(void *dst, void *src, size_t len);
void *caos_memset(void *dst, size_t c, size_t len);

size_t caos_memcmp(const void *cs, const void *ct, size_t count);
#endif
