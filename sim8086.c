#include <stdio.h>
#include <stdlib.h> // library that contains exit() and EXIT_FAILURE

// function prototypes
char* get_reg(int reg);

// START MAIN FUNCTION
int main(int argc, char **argv)
{
	if (argc < 2) // check if no arguments to main program are given
	{
		printf("No file given, please provide one ASM file to decode\n");
		exit(EXIT_FAILURE);
	}

	if (argc > 2) // check if too many arguments to main program are given
	{
		printf("Too many arguments given. Please provide one ASM file to decode\n");
		exit(EXIT_FAILURE);
	}

	FILE *asm_file = fopen(argv[1], "rb"); // open file in read only binary mode

	if (asm_file == NULL) // exit with failure if the file can't be opened
	{
		perror("Error opening file"); // perror to print system error message
		exit(EXIT_FAILURE);
	}

	int byte;
	while ((byte = fgetc(asm_file)) != EOF) // start reading the bytes in
	{
		if ((byte >> 4) == 0b1011) // if the byte is a mov, immediate to register instruction
		{
			printf("0b1011");
		}

		if ((byte >> 2) == 0b100010) // mov instruction
		{
			int byte2 = fgetc(asm_file); // read in the secong byte for this instruction
			if ((byte2 >> 6) == 0b11) // mod field check. mod is 11, register to register mov. no displacement bytes to follow
			{
				printf
				(
					"mov %s, %s\n",  // print out instruction

					/*
					* ternary operation.
					* if the D bit is 1 then reg is the destintion for the mov, and the regmem is the source. so the reg
					* is the first operand in the instruction, with the regmem coming afer. ie 'mov reg, regmem'
					* the w bit is bitwise or'd onto the front of the number, as it's is needed to retreive the correct reg/regmem
					* ie. if the reg is wide, the w bit is 1
					*/
					(byte >> 1) & 0b1 ?									// ternary condition to check if D bit is 1
					get_reg(((byte&0b1)<<3) | ((byte2 >> 3) & 0b111))	// if D is 1 reg is destination
					: get_reg(((byte&0b1)<<3) | (byte2 & 0b111)),		// if D is 0 regmen is destination

					/*
					* if the D bit is 0 then the regmem is the destintion for the mov, and the reg is the source. so the regmem is the
					* first operand in the instruction, and the register comes last. ie 'mov regmem, reg'
					* the w bit is bitwise or'd onto the front of the reg and regmem, as it is needed to retreive the correct register
					* ie. if the reg is wide, the w bit is 1
					*/
					(byte >> 1) & 0b1 ?									// ternary condition to check if D bit is 1
					get_reg(((byte&0b1)<<3) | (byte2 & 0b111))			// if D is 1 regmen is source
					: get_reg(((byte&0b1)<<3) | ((byte2 >> 3) & 0b111)) // if D is 0 reg is source
				);
			}
		}
	}



	// close the file when done(to release the resources it was using)
	fclose(asm_file);

	// exit with success code
	return(0);
}



char* get_reg(int reg)
{
	char *register_name[] =
	{
		"al",
		"cl",
		"dl",
		"bl",
		"ah",
		"ch",
		"dh",
		"bh",
		"ax",
		"cx",
		"dx",
		"bx",
		"sp",
		"bp",
		"si",
		"di"
	};
	return register_name[reg];
}
