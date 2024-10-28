#include <stdio.h>
#include <stdlib.h>

// function prototypes
char* get_reg(int reg);
char* get_effective_address(int regmem);

// START MAIN FUNCTION
int main(int argc, char *argv[])
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

	/* read the asm binary file bytes. fgetc gets one byte at a time. call it each time another
	byte is needed. */
	int byte;
	while ((byte = fgetc(asm_file)) != EOF)
	{



		// mov regmem to/from reg
		if ((byte >> 2) == 0b100010)
		{
			// read in the secong byte for this instruction type
			int byte2 = fgetc(asm_file);
			// mod field check. mod 11 register mode, no displacement
			if ((byte2 >> 6) == 0b11)
			{
					// D bit is one, reg is the destination
					if (((byte >> 1) & 0b1) == 0b1)
					{
						printf
							(
							"mov %s, %s\n",
							// add wide bit to reg to get the correct register
							get_reg(((byte&0b1)<<3) | ((byte2 >> 3) & 0b111)),
							// add wide bit to regmem to get the correct register
							get_reg(((byte&0b1)<<3) | (byte2 & 0b111))
							);
					}
					// D bit is zero, regmem is the destination
					else
					{
						printf
							(
							"mov %s, %s\n",
							// add wide bit to regmem to get the correct register
							get_reg(((byte&0b1)<<3) | (byte2 & 0b111)),
							// add wide bit to reg to get the correct register
							get_reg(((byte&0b1)<<3) | ((byte2 >> 3) & 0b111))
							);
					}
			}
			// mod field check. mod 00. no displacement except special case
			if ((byte2 >> 6) == 0b00)
			{
				/*
				 * the special case. for this instruction where regmem is 110 has a 2 byte
				 * displacement.
				 */
				if ((byte2 & 0b111) == 0b110)
				{
					// read in 3rd and 4th byte for displacement
					int byte3 = fgetc(asm_file);
					int byte4 = fgetc(asm_file);
					// if d bit is one, reg is destination
					if (((byte >> 1) & 0b1) == 0b1)
					{
						printf
							(
							"mov %s, [%i]\n",
							// add wide bit onto reg to retriece the correct register
							get_reg(((byte&0b1)<<3) | ((byte2 >> 3) & 0b111)),
							// bitwise OR byte 4 onto front of byte3 so the number is correct
							((byte4 << 8) | byte3)
							);
					}
					// if d bit is zero, regmem is destination
					else
					{
						printf
							(
							"mov [%i], %s\n",
							((byte4 << 8) | byte3),
							get_reg(((byte&0b1)<<3) | ((byte2 >> 3) & 0b111))
							);
					}
				}
				// the not spicial case section, there is no dispacement needed
				else
				{
					// if D bit is one, reg is the destination
					if (((byte >> 1) & 0b1) == 0b1)
					{
						printf
							(
							"mov %s, [%s]\n",
							get_reg(((byte&0b1)<<3) | ((byte2 >> 3) & 0b111)),
							get_effective_address(byte2 & 0b111)
							);
					}
					// if D bit is zero, regmem is the destination
					else
					{
						printf
							(
							"mov [%s], %s\n",
							get_effective_address(byte2 & 0b111),
							get_reg(((byte&0b1)<<3) | ((byte2 >> 3) & 0b111))
							);
					}
				}
			}
			// mod field check. mod 01. displacement is 1 byte.
			if ((byte2 >> 6) == 0b01)
			{
				// read in 3rd byte for displacement
				int byte3 = fgetc(asm_file);
				// ternary condition to check if D bit is 1
				if (((byte >> 1) & 0b1) == 0b1)
				{
				// if D is 1 reg is destination
					// if byte3 is not zero add it into printf
					if (byte3 != 0)
					{
						printf
							(
							"mov %s, [%s + %i]\n",
							get_reg(((byte&0b1)<<3) | ((byte2 >> 3) & 0b111)),
							get_effective_address(byte2 & 0b111),
							byte3
							);
					} 
					// if byte3 is zero then don't add it into printf
					else
					{
						printf
							(
							"mov %s, [%s]\n",
							get_reg(((byte&0b1)<<3) | ((byte2 >> 3) & 0b111)),
							get_effective_address(byte2 & 0b111)
							);
					}
				}
				else
				{
				// if D is 0 regmen is destination
					// if byte3 is not zero add it into printf
					if (byte3 != 0)
					{
						printf
							(
							"mov %s, [%s + %i]\n",
							get_effective_address(byte2 & 0b111),
							get_reg(((byte&0b1)<<3) | ((byte2 >> 3) & 0b111)),
							byte3
							);
					}
					// if byte3 is zero then don't add it into printf
					else
					{
						printf
							(
							"mov [%s], %s\n",
							get_effective_address(byte2 & 0b111),
							get_reg(((byte&0b1)<<3) | ((byte2 >> 3) & 0b111))
							);
					}
				}
			}
			// mod field check. mod 10. displacement is 2 byte.
			if ((byte2 >> 6) == 0b10)
			{
				// read in 3rd byte for displacement
				int byte3 = fgetc(asm_file);
				// read in 4th byte for displacement
				int byte4 = fgetc(asm_file);
				// if D is 1 reg is destination
				if (((byte >> 1) & 0b1) == 0b1)
				{
					// if byte3 and byte4 are not zero, add there value into printf
					if ((byte3 != 0) && (byte4 !=0))
					{
						printf
							(
							"mov %s, [%s + %i]\n",
							get_reg(((byte&0b1)<<3) | ((byte2 >> 3) & 0b111)),
							get_effective_address(byte2 & 0b111),
							((byte4 << 8) | byte3)
							);
					}
					// if byte3 and byte4 are zero, don't add their value into printf
					else
					{
						// if byte 3 is 0 then don't add it into printf
						printf
							(
							"mov %s, [%s]\n",
							get_reg(((byte&0b1)<<3) | ((byte2 >> 3) & 0b111)),
							get_effective_address(byte2 & 0b111)
							);
					}
				}
				// if D is 0 regmen is destination
				else
				{
					// if byte3 and byte4 are not zero, add there value into printf
					if ((byte3 != 0) && (byte4 !=0))
					{
						printf
							(
							"mov %s, [%s + %i]\n",
							get_effective_address(byte2 & 0b111),
							get_reg(((byte&0b1)<<3) | ((byte2 >> 3) & 0b111)),
							((byte4 << 8) | byte3)
							);
					}
					// if byte3 and byte4 are zero, don't add their value into printf
					else
					{
						// if byte 3 is 0 then don't add it into printf
						printf
							(
							"mov [%s], %s\n",
							get_effective_address(byte2 & 0b111),
							get_reg(((byte&0b1)<<3) | ((byte2 >> 3) & 0b111))
							);
					}
				}
			}
		}

		// mov immediate to register mov instruction
		if ((byte >> 4) == 0b1011)
		{
			// initialise byte2 and byte3 to zero here so printf can use them as is if needed
			int byte2 = 0;
			int byte3 = 0;
			// check W(wide) bit
			if ((byte >> 3) & 0b1)
			{
				// if the wide bit is one, then a 16bit (2byte) displacement is needed
				byte2 = fgetc(asm_file);
				byte3 = fgetc(asm_file);
			} else {
				// if the wide bit is zero, then only an 8bit (1byte) displacement is needed
				byte2 = fgetc(asm_file);
			}
			// print out asm instruction
			printf(
				"mov %s, %i\n",
				// retrieve the correct register name
				get_reg(((byte >> 3) & 0b1) << 3 | (byte & 0b111)),
				// calculate the value from the displacement byte/s
				((byte3 << 8) | byte2)
				);
		}

		// add regmem with reg to either
		if ((byte>>2) == 0b000000)
		{
			// read in the secong byte for this instruction type
			int byte2 = fgetc(asm_file);
			// mod field check. mod 11 register mode, no displacement
			if ((byte2 >> 6) == 0b11)
			{
					// D bit is one, reg is the destination
					if (((byte >> 1) & 0b1) == 0b1)
					{
						printf
							(
							"add %s, %s\n",
							// add wide bit to reg to get the correct register
							get_reg(((byte&0b1)<<3) | ((byte2 >> 3) & 0b111)),
							// add wide bit to regmem to get the correct register
							get_reg(((byte&0b1)<<3) | (byte2 & 0b111))
							);
					}
					// D bit is zero, regmem is the destination
					else
					{
						printf
							(
							"add %s, %s\n",
							// add wide bit to regmem to get the correct register
							get_reg(((byte&0b1)<<3) | (byte2 & 0b111)),
							// add wide bit to reg to get the correct register
							get_reg(((byte&0b1)<<3) | ((byte2 >> 3) & 0b111))
							);
					}
			}
			// mod field check. mod 00. no displacement except special case
			if ((byte2 >> 6) == 0b00)
			{
				/*
				 * the special case. for this instruction where regmem is 110 has a 2 byte
				 * displacement.
				 */
				if ((byte2 & 0b111) == 0b110)
				{
					// read in 3rd and 4th byte for displacement
					int byte3 = fgetc(asm_file);
					int byte4 = fgetc(asm_file);
					// if d bit is one, reg is destination
					if (((byte >> 1) & 0b1) == 0b1)
					{
						printf
							(
							"add %s, [%i]\n",
							// add wide bit onto reg to retriece the correct register
							get_reg(((byte&0b1)<<3) | ((byte2 >> 3) & 0b111)),
							// bitwise OR byte 4 onto front of byte3 so the number is correct
							((byte4 << 8) | byte3)
							);
					}
					// if d bit is zero, regmem is destination
					else
					{
						printf
							(
							"add [%i], %s\n",
							((byte4 << 8) | byte3),
							get_reg(((byte&0b1)<<3) | ((byte2 >> 3) & 0b111))
							);
					}
				}
				// the not spicial case section, there is no dispacement needed
				else
				{
					// if D bit is one, reg is the destination
					if (((byte >> 1) & 0b1) == 0b1)
					{
						printf
							(
							"add %s, [%s]\n",
							get_reg(((byte&0b1)<<3) | ((byte2 >> 3) & 0b111)),
							get_effective_address(byte2 & 0b111)
							);
					}
					// if D bit is zero, regmem is the destination
					else
					{
						printf
							(
							"add [%s], %s\n",
							get_effective_address(byte2 & 0b111),
							get_reg(((byte&0b1)<<3) | ((byte2 >> 3) & 0b111))
							);
					}
				}
			}
			// mod field check. mod 01. displacement is 1 byte.
			if ((byte2 >> 6) == 0b01)
			{
				// read in 3rd byte for displacement
				int byte3 = fgetc(asm_file);
				// ternary condition to check if D bit is 1
				if (((byte >> 1) & 0b1) == 0b1)
				{
				// if D is 1 reg is destination
					// if byte3 is not zero add it into printf
					if (byte3 != 0)
					{
						printf
							(
							"add %s, [%s + %i]\n",
							get_reg(((byte&0b1)<<3) | ((byte2 >> 3) & 0b111)),
							get_effective_address(byte2 & 0b111),
							byte3
							);
					} 
					// if byte3 is zero then don't add it into printf
					else
					{
						printf
							(
							"add %s, [%s]\n",
							get_reg(((byte&0b1)<<3) | ((byte2 >> 3) & 0b111)),
							get_effective_address(byte2 & 0b111)
							);
					}
				}
				else
				{
				// if D is 0 regmen is destination
					// if byte3 is not zero add it into printf
					if (byte3 != 0)
					{
						printf
							(
							"add %s, [%s + %i]\n",
							get_effective_address(byte2 & 0b111),
							get_reg(((byte&0b1)<<3) | ((byte2 >> 3) & 0b111)),
							byte3
							);
					}
					// if byte3 is zero then don't add it into printf
					else
					{
						printf
							(
							"add [%s], %s\n",
							get_effective_address(byte2 & 0b111),
							get_reg(((byte&0b1)<<3) | ((byte2 >> 3) & 0b111))
							);
					}
				}
			}
		}

		// FIXME add immediate to accumulator
		if ((byte>>1) == 0b0000010)
		{
			// initialise byte2 and byte3 to zero here so printf can use them as is if needed
			int byte2 = 0;
			int byte3 = 0;
			// check W(wide) bit
			if ((byte & 0b1) == 0b1)
			{
				// if the wide bit is one, then a 16bit (2byte) displacement is needed
				byte2 = fgetc(asm_file);
				byte3 = fgetc(asm_file);
			} else {
				// if the wide bit is zero, then only an 8bit (1byte) displacement is needed
				byte2 = fgetc(asm_file);
			}
			// print out asm instruction
			printf(
				"add immediate to accumulator %s, %i\n",
				// retrieve the correct register name
				get_reg(((byte >> 3) & 0b1) << 3 | (byte & 0b111)),
				// calculate the value from the displacement byte/s
				((byte3 << 8) | byte2)
				);
		}
	}

	// close the file when done(to release the resources it was using)
	fclose(asm_file);

	// exit with success code
	return(0);
}

// function definitions
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
		"bx + si",
		"bx + di",
		"bp + si",
		"bp + di",
		"si",
		"di",
		"bp",
		"bx"
	};
	return effective_address_calculation[regmem];
}
