#include <stdio.h>
#include <stdlib.h> 

// function prototypes
char* get_reg(int reg);
char* get_effective_address(int regmem);

// START MAIN FUNCTION
int main(int argc, char **argv)
{

	// check if no arguments to main program are given
	if (argc < 2)
	{
		printf("No file given, please provide one ASM file to decode\n");
		exit(EXIT_FAILURE);
	}

	// check if too many arguments to main program are given
	if (argc > 2)
	{
		printf("Too many arguments given. Please provide one ASM file to decode\n");
		exit(EXIT_FAILURE);
	}

	// open file in read only binary mode
	FILE *asm_file = fopen(argv[1], "rb");

	// exit with failure if the file can't be opened
	if (asm_file == NULL)
	{

		// perror to print system error message
		perror("Error opening file");
		exit(EXIT_FAILURE);
	}

	// start reading the asm binary file bytes in
	int byte;
	while ((byte = fgetc(asm_file)) != EOF)
	{


		// if immediate to register mov instruction
		if ((byte >> 4) == 0b1011)
		{
			// initialise byte2 and byte3 to zero here so printf can use them
			int byte2 = 0;
			int byte3 = 0;

			// checking the W(wide) bit for this instruction
			if ((byte >> 3) & 0b1)
			{
				// if the wide bit is 1, then a 16 bit displacement is needed
				byte2 = fgetc(asm_file);
				byte3 = fgetc(asm_file);
			} else {
				// if the wide bit is 0, then only an 8 bit displacement is needed
				byte2 = fgetc(asm_file);
			}

			// print out asm instruction
			printf(
				"mov %s, %i\n",

				// retrieve the correct register name
				get_reg(((byte >> 3) & 0b1) << 3 | (byte & 0b111)),

				// calculate the value
				((byte3 << 8) | byte2)
				);
		}

		// mov instruction
		if ((byte >> 2) == 0b100010)
		{

			// read in the secong byte for this instruction type
			int byte2 = fgetc(asm_file);

			// mod field check. mod 11 register mode, no displacement
			if ((byte2 >> 6) == 0b11)
			{

				// print out asm instruction
				printf(
					"mov %s, %s\n",

					/*
					* ternary operation.
					* if the D bit is 1 then reg is the destintion for the mov, and the regmem is the source. so the reg
					* is the first operand in the instruction, with the regmem coming afer. ie 'mov reg, regmem'
					* the w bit is bitwise or'd onto the front of the number, as it's is needed to retreive the correct reg/regmem
					* ie. if the reg is wide, the w bit is 1
					*/

					// ternary condition to check if D bit is 1
					(byte >> 1) & 0b1

						// if D is 1 reg is destination
						? get_reg(((byte&0b1)<<3) | ((byte2 >> 3) & 0b111))

						// if D is 0 regmen is destination
						: get_reg(((byte&0b1)<<3) | (byte2 & 0b111)),

					/*
					* if the D bit is 0 then the regmem is the destintion for the mov, and the reg is the source. so the regmem is the
					* first operand in the instruction, and the register comes last. ie 'mov regmem, reg'
					* the w bit is bitwise or'd onto the front of the reg and regmem, as it is needed to retreive the correct register
					* ie. if the reg is wide, the w bit is 1
					*/

					// ternary condition to check if D bit is 1
					(byte >> 1) & 0b1

					// if D is 1 regmen is source
					? get_reg(((byte&0b1)<<3) | (byte2 & 0b111))

					// if D is 0 reg is source
					: get_reg(((byte&0b1)<<3) | ((byte2 >> 3) & 0b111))
					);
			}

			// mod field check. mod is 00. no displacement except special case
			if ((byte2 >> 6) == 0b00)
			{
				// the special case for this instruction where regmem is 110 has a 2 byte displacement.
				if ((byte2 & 0b111) == 0b110)
				{
					// printf for decoding purposes
					printf("special case");

					// read in 3rd byte for displacement
					int byte3 = fgetc(asm_file);
					// read in 4th byte for displacement
					int byte4 = fgetc(asm_file);
				} else {
					// print out asm instruction
					printf(
						"mov %s, %s\n",

						/*
						* ternary operation.
						* if the D bit is 1 then reg is the destintion for the mov, and the regmem is the source. so the reg
						* is the first operand in the instruction, with the regmem coming afer. ie 'mov reg, regmem'
						* the w bit is bitwise or'd onto the front of the number, as it's is needed to retreive the correct reg/regmem
						* ie. if the reg is wide, the w bit is 1
						*/

						// ternary condition to check if D bit is 1
						(byte >> 1) & 0b1

							// if D is 1 reg is destination
							? get_reg(((byte&0b1)<<3) | ((byte2 >> 3) & 0b111))

							// if D is 0 regmen is destination
							: get_effective_address(byte2 & 0b111),

						/*
						* if the D bit is 0 then the regmem is the destintion for the mov, and the reg is the source. so the regmem is the
						* first operand in the instruction, and the register comes last. ie 'mov regmem, reg'
						* the w bit is bitwise or'd onto the front of the reg and regmem, as it is needed to retreive the correct register
						* ie. if the reg is wide, the w bit is 1
						*/

						// ternary condition to check if D bit is 1
						(byte >> 1) & 0b1

						// if D is 1 regmen is source
						? get_effective_address(byte2 & 0b111)

						// if D is 0 reg is source
						: get_reg(((byte&0b1)<<3) | ((byte2 >> 3) & 0b111))
						);

				}
				
			}

			// mod field check. mod is 01. displacement is 1 byte.
			if ((byte2 >> 6) == 0b01)
			{

				// read in 3rd byte for displacement
				int byte3 = fgetc(asm_file);

				// print out instruction
				printf(
					"mov %s, %s\n",
					get_reg(((byte&0b1)<<3) | ((byte2 >> 3) & 0b111)),
					"placeholder for mod 01 regmem"
					);
			}

			// mod field check. mod is 10. displacement is 2 byte.
			if ((byte2 >> 6) == 0b10)
			{

				// read in 3rd byte for displacement
				int byte3 = fgetc(asm_file);
				// read in 4th byte for displacement
				int byte4 = fgetc(asm_file);

				// print out instruction
				printf("mov %s, %s\n", "placeholder", "placeholder");
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

char* get_effective_address(int regmem)
{
	char *effective_address_calculation[] = 
	{
		"[bx + si]",
		"[bx + di]",
		"[bp + si]",
		"[bp + di]",
		"si",
		"di",
		"bp",
		"bx"
	};
	return effective_address_calculation[regmem];
}

