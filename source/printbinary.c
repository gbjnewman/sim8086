#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

int main(int argc, char *argv[])
{
	FILE *asm_file = fopen(argv[1], "rb");

	int byte;
	while ((byte = fgetc(asm_file)) != EOF)
	{
		printf("%-.3i: %x \n", byte, byte);
	}

	return 0;
}

