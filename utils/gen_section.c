


/*
 * read system.map and generate header file, section.h
 *
 * which informs linear-address of symbols in kernel.
 *
 */


#include <stdio.h>
#include <string.h>


char line_buf[128];

char address[20];
char symbol[64];
char flag;


int main(int argc, char *argv[])
{
	FILE *symfile;
	FILE *headfile;

	if (argc != 3) {
		printf("USAGE: sym_gen system.map header.h\n");
		return 1;
	}


	symfile = fopen(argv[1], "r");
	headfile = fopen(argv[2], "w");

	if (headfile == NULL || symfile == NULL) {
		perror("file error?");
		return 1;
	}


	fprintf(headfile, ";*******************************************************************\n");
	fprintf(headfile, "; This file is generated automatically by utils/gen_section program\n");
	fprintf(headfile, ";*******************************************************************\n");

	while (fgets(line_buf, 127, symfile) != NULL) {
		sscanf(line_buf, "%s %c %s", address, &flag, symbol);

		fprintf(headfile, "%-20s equ 0x%s\n", symbol, address);
	}



	fclose(symfile);
	fclose(headfile);



	return 0;
}


