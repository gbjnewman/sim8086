#include <stdio.h>
#include <stdlib.h> // so i can use exit(EXIT_FAILURE)
#include <stdint.h> // so i can use int8_t

// function prototypes
char* get_reg(int reg);
char* get_effective_address(int regmem);
void regmem_tofrom_regmem(FILE* asm_file, int byte, char* instruction);
void immediate_to_regmem(FILE* asm_file, int byte, char* instruction);
void immediate_to_register(FILE* asm_file, int byte, char* instruction);
void immediate_to_accumulator(FILE* asm_file, int byte, char* instruction);

/* START MAIN FUNCTION */
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
		// print system error message
		fprintf(stderr, "error opening file '%s': ", argv[1]);
		// prints the system error after the first custom message
		perror("");
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
			char instruction[] = "mov";
			regmem_tofrom_regmem(asm_file, byte, instruction);
		}

		// mov immediate to regmem // TODO: not complete yet
		if ((byte >> 1) == 0b1100011)
		{
			char instruction[] = "mov";
			immediate_to_regmem(asm_file, byte, instruction);
		}

		// mov immediate to register
		if ((byte >> 4) == 0b1011)
		{
			char instruction[] = "mov";
			immediate_to_register(asm_file, byte, instruction);
		}

		// mov memory to accumulator
		if ((byte >> 1) == 0b1010000)
		{
			// w bit is 1
			if (byte & 0b1)
			{
				// get addr-lo
				int byte2 = fgetc(asm_file);
				// get addr-hi
				int byte3 = fgetc(asm_file);
				// add bytes togther to get memory address value
				int16_t memory_address_value = ((byte3 << 8) | byte2);
				//print out intruction
				printf
					(
					"mov %s, [%i]\n",
					// print the accumulator register
					"ax",
					// print memory address
					memory_address_value
					);
			}
			// w bit is 0
			else
			{
				printf("mem to accu, w bit is 0");
			}
		}

		// mov accumulator to memory
		if ((byte >> 1) == 0b1010001)
		{
			// w bit is 1
			if (byte & 0b1)
			{
				// get addr-lo
				int byte2 = fgetc(asm_file);
				// get addr-hi
				int byte3 = fgetc(asm_file);
				// add bytes togther to get memory address value
				int16_t memory_address_value = ((byte3 << 8) | byte2);
				//print out intruction
				printf
					(
					"mov [%i], %s\n",
					// print memory address
					memory_address_value,
					// print the accumulator register
					"ax"
					);
			}
			// w bit is 0
			else
			{
				printf("mem to accu, w bit is 0");
			}
		}

		// add, sub, cmp immediate to regmem
		if ((byte >> 2) == 0b100000)
		{
			char instruction[4];
			immediate_to_regmem(asm_file, byte, instruction);
		}

		// add regmem to/from reg
		if ((byte >> 2) == 0b000000)
		{
			char instruction[] = "add";
			regmem_tofrom_regmem(asm_file, byte, instruction);
		}

		// add immediate to accumulator
		if ((byte>>1) == 0b0000010)
		{
			char instruction[] = "add";
			immediate_to_accumulator(asm_file, byte, instruction);
		}

		// sub regmem to/from reg
		if ((byte >> 2) == 0b001010)
		{
			char instruction[] = "sub";
			regmem_tofrom_regmem(asm_file, byte, instruction);
		}

		// sub immediate to accumulator
		if ((byte>>1) == 0b0010110)
		{
			char instruction[] = "sub";
			immediate_to_accumulator(asm_file, byte, instruction);
		}

		// cmp regmem to/from reg
		if ((byte >> 2) == 0b001110)
		{
			char instruction[] = "cmp";
			regmem_tofrom_regmem(asm_file, byte, instruction);
		}

		// cmp immediate to accumulator
		if ((byte>>1) == 0b0011110)
		{
			char instruction[] = "cmp";
			immediate_to_accumulator(asm_file, byte, instruction);
		}

		if (byte == 0b01110101)
		{
			int byte2 = fgetc(asm_file);
			printf("jnz something\n");
		}

	}

	// close the file when done(to release the resources it was using)
	fclose(asm_file);

	// exit with success code
	return(0);
}

/* function definitions */

void regmem_tofrom_regmem(FILE* asm_file, int byte, char* instruction)
{
	// read in the secong byte for this instruction
	int byte2 = fgetc(asm_file);
	// mod field check. mod 00. no displacement except special case
	if ((byte2 >> 6) == 0b00)
	{
		/*
		 * the special case. for this instruction where regmem is 110 has a 2 byte
		 * displacement.
		 */
		if ((byte2 & 0b111) == 0b110)
		{
			printf("special case: ");
			// read in 3rd and 4th byte for displacement
			int byte3 = fgetc(asm_file);
			int byte4 = fgetc(asm_file);
			// if d bit is one, reg is destination
			if (((byte >> 1) & 0b1) == 0b1)
			{
				printf
					(
					"%s %s, [%i]\n",
					instruction,
					// register. add wide bit onto reg to retrieve the correct register
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
					"%s [%i], %s\n",
					instruction,
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
					"%s %s, [%s]\n",
					instruction,
					get_reg(((byte&0b1)<<3) | ((byte2 >> 3) & 0b111)),
					get_effective_address(byte2 & 0b111)
					);
			}
			// if D bit is zero, regmem is the destination
			else
			{
				printf
					(
					"%s [%s], %s\n",
					instruction,
					get_effective_address(byte2 & 0b111),
					get_reg(((byte&0b1)<<3) | ((byte2 >> 3) & 0b111))
					);
			}
		}
	} // mod 01. memory mode displacement is 1 byte.
	if ((byte2 >> 6) == 0b01)
	{
		// read in 3rd byte for displacement
		int byte3 = fgetc(asm_file);
		// check if D bit is 1
		if (((byte >> 1) & 0b1) == 0b1)
		{
		// if D is 1 reg is destination
			// if byte3 is not zero add it into printf
			if (byte3 != 0)
			{
				printf
					(
					"%s %s, [%s %s %i]\n",
					instruction,
					// register
					get_reg(((byte&0b1)<<3) | ((byte2 >> 3) & 0b111)),
					// effective address
					get_effective_address(byte2 & 0b111),
					// string for the sign
					byte3 < 0 ? "-" : "+",
					// displacement value
					byte3 < 0 ? -byte3 : byte3
					);
			}
			// if byte3 is zero then don't add it into printf
			else
			{
				printf
					(
					"%s %s, [%s]\n",
					instruction,
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
					"%s [%s %s %i], %s\n",
					instruction,
					// effective address
					get_effective_address(byte2 & 0b111),
					// string for the sign
					byte3 < 0 ? "-" : "+",
					// displacement value
					byte3 < 0 ? -byte3 : byte3,
					// register
					get_reg(((byte&0b1)<<3) | ((byte2 >> 3) & 0b111))
					);
			}
			// if byte3 is zero then don't add it into printf
			else
			{
				printf
					(
					"%s [%s], %s\n",
					instruction,
					get_effective_address(byte2 & 0b111),
					get_reg(((byte&0b1)<<3) | ((byte2 >> 3) & 0b111))
					);
			}
		}
	}
	// mod 10. memory mode displacement is 2 byte.
	if ((byte2 >> 6) == 0b10)
	{
		// read in 3rd byte for displacement
		int byte3 = fgetc(asm_file);
		// read in 4th byte for displacement
		int byte4 = fgetc(asm_file);
		// displacemt value
		int16_t displacement_value = ((byte4 << 8) | byte3);
		// if D is 1 reg is destination
		if (((byte >> 1) & 0b1) == 0b1)
		{
			// if byte3 and byte4 are not zero, add there value into printf
			if ((byte3 != 0) && (byte4 !=0))
			{
				printf
					(
					"%s %s, [%s + %i]\n",
					instruction,
					// get register
					get_reg(((byte&0b1)<<3) | ((byte2 >> 3) & 0b111)),
					// get effective address
					get_effective_address(byte2 & 0b111),
					// displacent for effective address
					displacement_value
					);
			}
			// if byte3 and byte4 are zero, don't add their value into printf
			else
			{
				// if byte 3 is 0 then don't add it into printf
				printf
					(
					"%s %s, [%s]\n",
					instruction,
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
					"%s [%s %s %i], %s\n",
					instruction,
					// effective address
					get_effective_address(byte2 & 0b111),
					// displacemt sign
					displacement_value < 0 ? "-" : "+",
					// displacement value
					displacement_value < 0 ? -displacement_value : displacement_value,
					// register
					get_reg(((byte&0b1)<<3) | ((byte2 >> 3) & 0b111))
					);
			}
			// if byte3 and byte4 are zero, don't add their value into printf
			else
			{
				// if byte 3 is 0 then don't add it into printf
				printf
					(
					"%s [%s], %s\n",
					instruction,
					get_effective_address(byte2 & 0b111),
					get_reg(((byte&0b1)<<3) | ((byte2 >> 3) & 0b111))
					);
			}
		}
	}
	// mod field check. mod 11 register mode, no displacement
	if ((byte2 >> 6) == 0b11)
	{
			// D bit is one, reg is the destination
			if (((byte >> 1) & 0b1) == 0b1)
			{
				printf
					(
					"%s %s, %s\n",
					instruction,
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
					"%s %s, %s\n",
					instruction,
					// add wide bit to regmem to get the correct register
					get_reg(((byte&0b1)<<3) | (byte2 & 0b111)),
					// add wide bit to reg to get the correct register
					get_reg(((byte&0b1)<<3) | ((byte2 >> 3) & 0b111))
					);
			}
	}
}

void immediate_to_regmem(FILE* asm_file, int byte, char* instruction)
{
	// get instruction byte
	int byte2 = fgetc(asm_file);

	if ((byte >> 2) == 0b100000)
	{
		if (((byte2 >> 3) & 0b111) == 0b000)
		{
			instruction = "add";
		}
		if (((byte2 >> 3) & 0b111) == 0b101)
		{
			instruction = "sub";
		}
		if (((byte2 >> 3) & 0b111) == 0b111)
		{
			instruction = "cmp";
		}
	}

	if ((byte2 >> 6) == 0b00)
	// mod00, no displacement *except special case
	{
		// immediate to direct address. special case regmem equals 110. 16bit displacement
		if ((byte2 & 0b111) == 0b110)
		{
			//printf("    *immediate to direct address, mod00 specail case. \n");
			// get displacement byte
			int byte3 = fgetc(asm_file);
			// get displacement byte
			int byte4 = fgetc(asm_file);
			// w bit check
			// w bit 1. wide instruction
			if ((byte & 0b1) == 0b1)// FIXME: needs completing
			{
				// get data byte
				int8_t byte5 = fgetc(asm_file);
				//printf("spec case·mod·00·regmem·110·wide");
				printf
					(
					"%s [%i], %i\n",
					instruction,
					((byte4 << 8) | byte3),
					byte5
					);
			}
			// w bit 0
			else// FIXME: needs completing
			{
				// get data byte
				int byte5 = fgetc(asm_file);
				printf("fix this imm to direct\n");
			}
		}
		// not special case, no displacement
		else
		{
			// w bit 1
			if (byte & 0b1)
			{
				// get data byte
				int byte3 = fgetc(asm_file);
				//print out instruction
				printf
					(
					"%s %s [%s], %i\n",
					instruction,
					// immediate size
					"word",
					// retrieve effective address
					get_effective_address(byte2 & 0b111),
					// immediate value
					byte3
					);
			}
			// w bit 0
			else
			{
				// get data byte(signed int 8bits)
				int8_t byte3 = fgetc(asm_file);
				//print out instruction
				printf
					(
					"%s %s [%s], %i\n",
					instruction,
					// immediate size
					"byte",
					// retrieve effective address
					get_effective_address(byte2 & 0b111),
					// immediate value
					byte3
					);
			}
		}
	}
	// mod01, memory mode, 8bit displacement follows// TODO: finish making this
	if ((byte2 >> 6) == 0b01)// TODO: finish making this
	{
		// get displacement byte
		int byte3 = fgetc(asm_file);
		printf("    mod01");
	}
	// mod10, memory mode, 16bit displacement follows
	if ((byte2 >> 6) == 0b10)
	{
		// get displacement byte
		int byte3 = fgetc(asm_file);
		// get displacement byte
		int byte4 = fgetc(asm_file);
		// bitwise OR together displacement bytes to make displacement value
		int16_t displacement_value = ((byte4 << 8) | byte3);
		// w bit 1, wide mode
		if ((byte & 0b1) == 0b1)
		{
			// get data byte
			int8_t byte5 = fgetc(asm_file);
			// print out instruction
			printf
				(
				"%s %s [%s + %i], %i\n",
				instruction,
				// print immediate value bit width
				"word",
				// retrieve effective address
				get_effective_address(byte2 & 0b111),
				// print displacement value
				displacement_value,
				// print out the immediate value
				byte5
				);
		}
		// w bit 0
		else
		{
			// get data byte
			int byte5 = fgetc(asm_file);
			printf
				(
				"%s %s + %i, %s %i\n",
				instruction,
				// retrieve effective address
				get_effective_address(byte2 & 0b111),
				// print displacement value
				displacement_value,
				// print immediate value bit width
				"byte",
				// print out the immediate value
				byte5
				);
		}
	}
	// mod11, register mode, no displacement
	if ((byte2 >> 6) == 0b11)
	{
		// w bit 1
		if ((byte & 0b1) == 0b1)
		{
			// if s is one, sign extend byte3
			if (byte>>1)
			{
				// get data byte
				int8_t byte3 = fgetc(asm_file);
				printf
					(
					"%s %s, %i\n",
					instruction,
					// get register
					get_reg((byte & 0b1) << 3 | (byte2 & 0b111)),
					// immediate value
					byte3
					);
			}
			// s is zero, don't sign extend
			else
			{
			// get data byte
			int byte3 = fgetc(asm_file);
			// get data byte
			int byte4 = fgetc(asm_file);
			printf
				(
				"debug %s %s, %i\n",
				instruction,
				// get register
				get_reg((byte & 0b1) << 3 | (byte2 & 0b111)),
				// immediate value
				// bitwise OR together data bytes to get immediate value
				((byte4<<8) | byte3)
				);
			}
		}
		// w bit 0
		else// TODO: finish making this
		{
			// get data byte
			int byte3 = fgetc(asm_file);
			// print out instruction
			printf
				(
				"%s %s, %i\n : this immediate value needs checking",
				instruction,
				// get register
				get_reg((byte & 0b1) << 3 | (byte2 & 0b111)),
				// immediate value
				byte3
				);
		}
		/*//old
		printf("    mod11");
		// w bit 1
		if ((byte & 0b1) == 0b1)// TODO: finish making this
		{
			// get data byte
			int byte3 = fgetc(asm_file);
			// get data byte
			int byte4 = fgetc(asm_file);
		}
		// w bit 0
		else// TODO: finish making this
		{
			// get data byte
			int byte3 = fgetc(asm_file);
		}
		*/
	}
}

void immediate_to_register(FILE* asm_file, int byte, char* instruction)
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
		"%s %s, %i\n",
		instruction,
		// retrieve the correct register name
		get_reg(((byte >> 3) & 0b1) << 3 | (byte & 0b111)),
		// calculate the value from the displacement byte/s
		((byte3 << 8) | byte2)
		);
}

void immediate_to_accumulator(FILE* asm_file, int byte, char* instruction)
{
	// check W(wide) bit
	if ((byte & 0b1) == 0b1)
	{
		// if the wide bit is one, then a 16bit (2byte) data
		int byte2 = fgetc(asm_file);
		int byte3 = fgetc(asm_file);
		// print out asm instruction
		printf(
			"%s %s, %i\n",
			instruction,
			// accumulator register
			"ax",
			// value to add to accumulator
			((byte3 << 8) | byte2)
			);
	} else {
		// if the wide bit is zero, then only an 8bit (1byte) displacement is needed
		int8_t byte2 = fgetc(asm_file);
		// print out asm instruction
		printf(
			"%s %s, %i\n",
			instruction,
			// accumulator register
			"al",
			// value to add to accumulator
			byte2
			);
	}
}

// register array retrieval
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

// effective address array retrieval
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
