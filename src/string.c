#include <string.h>
#include <types.h>


size_t caos_memcmp(const void *cs, const void *ct, size_t count)
{
	const unsigned char *su1, *su2;
	size_t res = 0;

	for (su1 = cs, su2 = ct; 0 < count; ++su1, ++su2, count--)
		if ((res = *su1 - *su2) != 0)
			break;
	return res;
}


size_t caos_strcmp(const char *src, const char *dst)
{
	char t = 0;

	while (1) {
		if ((t = *src - *dst++) != 0 || !*src++)
			break;
	}

	return (size_t)t;

}




size_t caos_strcat(char *A_szDest, char *A_szSrc)
{
		char *p = A_szSrc;
		while (*A_szSrc != '\0')
				*A_szDest++ = *A_szSrc++;
		return A_szSrc-p;
}


size_t caos_strlen(const char *s)
{
		int i = 0;
		while (s[i] != '\0')
				i++;

		return i;
}

void *caos_memcpy(void *dst, void *src, size_t len)
{
	long *pldst = (long *)dst;
	long const *plsrc = (long const *)src;
	char *pcdst;
	char const *pcsrc;

	//
	// If address is aligned by 	// 4-bytes,
	//  memory is copied by 4-bytes size
	//
	if ( (((unsigned long)src & -4UL) == 0) && \
			(((unsigned long)dst & -4UL) == 0) ) {
		while (len >= sizeof(long)) {
			*pldst++ = *plsrc++;
			len -= sizeof(long);
		}
	}

	//
	// copy extra bytes
	//
	pcdst = (char *)pldst;
	pcsrc = (char const *)plsrc;

	while (len--)
		*pcdst++ = *pcsrc++;

	// GNU memcpy returns target address
	return dst;
}


void *caos_memset(void *dst, size_t c, size_t len)
{
	volatile char *p = (volatile char *)dst;


	while (len-- > 0) {
		*p = (char)c;
		p++;
	}

	// GNU memcpy returns target address
	return dst;
}
