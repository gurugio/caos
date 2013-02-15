#include <printf.h>
#include <types.h>
#include <screen.h>
#include <string.h>

#include <bitops.h> // test_bit




//
// Print one character on screen, only black-on-white color is supported.
// Then cursor move forward by a column
// @A_byteChar: ascii code
//
void caos_putchar(char A_byteChar)
{
		screen_info.video_mem[screen_info.cursor_offset] = (unsigned short)(BG_BLACK|A_byteChar);
		screen_info.cursor_offset++;
		set_cursor(screen_info.cursor_offset);
}


//
// Delete cnt-characters on screen and move cursor backward by a column
// @cnt: number of characters to be deleted.
//
void caos_delchar(ssize_t cnt)
{
	
	for ( ; cnt > 0; cnt--) {
		screen_info.cursor_offset--;
		set_cursor(screen_info.cursor_offset);
		screen_info.video_mem[screen_info.cursor_offset] = (unsigned short)(BG_BLACK);
	}
	

}


/*
 * printf implementation refers to http://wiki.osdev.org/Printing_to_Screen
 */


//
// memory buffer, printable string is stored by vsprintf.
//
static char sz[128];

//
// asmlinkage keyword is useless for x86_64 
// because every parameter is passed in register
// Every parameter is 64bit long type.
// @format: format string includes format-specifier and escape-sequences.
// return: the number of printed character
//
ssize_t caos_printf(const char *format, ...)
{
		va_list ap;
		ssize_t i = 0;


		/*
		// 
		// < test code >
		//
		// function arguments are passed as long type?
		//
		// case : caos_printf("%x\n", 0x123456789abcdef1UL)
		// rsi <= 0x123456789abcdef1UL
		// therefore, arguments are long type
		//
		do {
			unsigned long rsi;
			__asm__ __volatile__ ("movq %%rsi, %0": :"a"(rsi));
			i = print_hex(sz, rsi);
			sz[i] = '\0';
		} while (0);
		*/


		va_start(ap, format);

		// format specifier (%d,%c,%s,%x)
		// Every format-specifier is translated 
		// and printable string is stored at sz.
		caos_vsprintf(sz, format, ap);
		va_end(ap);

		i = 0;
		
		// escape character is printed now.
		while (sz[i] != '\0') {
			if (sz[i] == '\n') {
				// move to next line
				set_cursor( ((size_t)(screen_info.cursor_offset/SCREEN_COL) + 1)*SCREEN_COL );
				i++;
				continue;
			} else if (sz[i] == '\b') {
				// delete one character
				caos_delchar(1);
				i++;
				continue;
			}

			caos_putchar(sz[i++]);
		}

		return i;
}


//
// translate hexadecimal value to string
// @buf: buffer to store translated string
// @a: hexadecimal value to be translated
// return: the length of output string
//
ssize_t print_hex(char *buf, ssize_t a)
{
	unsigned char *p;
	unsigned char v[32];
	ssize_t i=0, j=0;
	ssize_t count=0;

	// zero
	if (a == 0) {
		*buf = 0x30;
		return 1;
	}

	p = (unsigned char *)&a;

	for (i = sizeof(ssize_t); i > 0; i--) {
		if ( (p[i-1] >> 4) <= 9)
			v[count] = (p[i-1] >> 4) + 0x30;
		else 
			v[count] = (p[i-1] >> 4) + 0x37;
		count++;

		if ( (p[i-1] & 0x0f) <= 9)
			v[count] = (p[i-1] & 0x0f) + 0x30;
		else 
			v[count] = (p[i-1] & 0x0f) + 0x37;
		count++;
	}


	i=0;
	while (v[i] == '0')	// skip initial zeros
		i++;
				
	count = sizeof(ssize_t)*2 - i;	// how many digits
	for (j=0; j<count; j++)
		buf[j] = v[i++];

	return count;

}


//
// format-specifier is converted and merged into string.
// @A_szString: buffer to store output string
// @A_szFormat: format-string
// @A_pAp: function paramter list of printf
//
asmlinkage void caos_vsprintf(char *A_szString, const char *A_szFormat, va_list A_pAp)
{

	//
	//==============================================
	// WARNING!! Very Important!!
	// Every value passed in stack is processor-dependent!
	// Therefore arguments must be declare ssize_t type.
	//=============================================
	//
	ssize_t arg_value = 0;	// store poped data from stack
	ssize_t digit_num = 0;	// count of printing characters

	while (*A_szFormat != '\0') {
		// print decimal number
		if (*A_szFormat == '%' && *(A_szFormat+1) == 'd') {
			arg_value = va_arg(A_pAp, ssize_t);
			digit_num = print_decimal(A_szString, arg_value);
			A_szString += digit_num;
			A_szFormat += 2;
			// print hexa number
		} else if (*A_szFormat == '%' && *(A_szFormat+1) == 'x' ) {
			arg_value = va_arg(A_pAp, ssize_t);
			digit_num = print_hex(A_szString, arg_value);
			A_szString += digit_num;
			A_szFormat += 2;
			// print scring
		} else if (*A_szFormat == '%' && *(A_szFormat+1) == 's') {
			digit_num = caos_strcat(A_szString, va_arg(A_pAp, char *));
			A_szString += digit_num;
			A_szFormat += 2;
			// print one character
		} else if (*A_szFormat == '%' && *(A_szFormat+1) == 'c') {
			// character data is stored into stack with the same size of integer
			*A_szString++ = va_arg(A_pAp, ssize_t);
			A_szFormat += 2;
			// normal character is copied as itself
		} else {
			*A_szString++ = *A_szFormat++;
		}
	}
	*A_szString = '\0';

}



//
// translate decimal number to string
// @A_szBuffer: memory buffer to store the string
// @a: signed decimal number
//
ssize_t print_decimal(char *A_szBuffer, ssize_t a)
{
	unsigned char v[32];
	int i = 0;
	int count = 0;

		
	if (a == 0) {
		*A_szBuffer = (char)'0';
		return 1;
	}

	// check MSB and print signed integer
	//if (a & 0x8000000000000000) {
	if (test_bit(BITS_PER_LONG-1, &a) != 0) {
		// print minus
		*A_szBuffer++ = '-';
		count++;
		a = -a;
	}
				
	while (a > 0) {
		v[i++] = a % 10;
		a /= 10;
	}

	for (--i; i >= 0; i--) {
		*A_szBuffer++ = v[i]+(char)'0';
		count++;
	}
				

	return count;
}



//
// caos_sprintf do not print message on display
// and store message in memory buffer.
//
ssize_t caos_sprintf(char *outbuf, const char *format, ...)
{
		va_list ap;
		ssize_t i = 0;


		va_start(ap, format);
		caos_vsprintf(outbuf, format, ap);
		va_end(ap);

		return i;
}


