#include <stdio.h>
#include <stdlib.h> // so i can use exit(EXIT_FAILURE)
#include <stdint.h> // so i can use int8_t

// function prototypes
char* get_reg(int reg);
char* get_effective_address(int regmem);

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

		//printf("byte1 is %B: ", byte);

		// mov regmem to/from reg
		if ((byte >> 2) == 0b100010)
		{
			// read in the secong byte for this instruction type
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
					// read in 3rd and 4th byte for displacement
					int byte3 = fgetc(asm_file);
					int byte4 = fgetc(asm_file);
					// if d bit is one, reg is destination
					if (((byte >> 1) & 0b1) == 0b1)
					{
						printf
							(
							"mov %s, [%i]\n",
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
					} } } // mod field check. mod 01. displacement is 1 byte.
			if ((byte2 >> 6) == 0b01)
			{
				// read in 3rd byte for displacement
				int8_t byte3 = fgetc(asm_file);
				// check if D bit is 1
				if (((byte >> 1) & 0b1) == 0b1)
				{
				// if D is 1 reg is destination
					// if byte3 is not zero add it into printf
					if (byte3 != 0)
					{
						printf
							(
							"mov %s, [%s %s %i]\n",
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
							"mov %s, [%s %s %i]\n",
							// effective address
							get_effective_address(byte2 & 0b111),
							// register
							get_reg(((byte&0b1)<<3) | ((byte2 >> 3) & 0b111)),
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
							"mov %s, [%s + %i]\n",
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
							"mov [%s %s %i], %s\n",
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
							"mov [%s], %s\n",
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
		}

		// mov immediate to regmem // TODO: not complete yet
		if ((byte >> 1) == 0b1100011)
		{
			// get instruction byte
			int byte2 = fgetc(asm_file);
			// mod00, no displacement *except special case
			if ((byte2 >> 6) == 0b00)
			{
				// special case regmem equals 110. 16bit displacement to follow
				if ((byte2 & 0b111) == 0b110)
				{
					printf("    *mov immediate to regmem, mod00 specail case. \n");
					// get displacement byte
					int byte3 = fgetc(asm_file);
					// get displacement byte
					int byte4 = fgetc(asm_file);
					// w bit check
					// w bit 1. wide instruction
					if ((byte & 0b1) == 0b1)// FIXME: needs completing
					{
						// get data byte
						int byte5 = fgetc(asm_file);
						// get data byte
						int byte6 = fgetc(asm_file);
					}
					// w bit 0
					else// FIXME: needs completing
					{
						// get data byte
						int byte5 = fgetc(asm_file);
					}
				}
				// not special case, no displacement
				else
				{
					// w bit 1
					if (byte & 0b1)
					{
						printf("wide\n");
						// get data byte
						int byte3 = fgetc(asm_file);
						// get data byte
						int byte4 = fgetc(asm_file);
						// bitwise OR together data bytes to make immediate value
						int16_t immediate_value = ((byte4 << 8) | byte3);
						//print out instruction
						printf
							(
							"mov [%s], %s %i\n",
							// retrieve effective address
							get_effective_address(byte2 & 0b111),
							// immediate size
							"word",
							// immediate value
							immediate_value 
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
							"mov [%s], %s %i\n",
							// retrieve effective address
							get_effective_address(byte2 & 0b111),
							// immediate size
							"byte",
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
					int byte5 = fgetc(asm_file);
					// get data byte
					int byte6 = fgetc(asm_file);
					// bitwise OR together data bytes to make immediate value
					int16_t immediate_value = ((byte6 << 8) | byte5);
					printf
						(
						"mov %s + %i, %s %i\n",
						// retrieve effective address
						get_effective_address(byte2 & 0b111),
						// print displacement value
						displacement_value,
						// print immediate value bit width
						"word",
						// print out the immediate value
						immediate_value
						);
				}
				// w bit 0
				else
				{
					// get data byte
					int byte5 = fgetc(asm_file);
					printf
						(
						"mov %s + %i, %s %i\n",
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
			}
		}

		// mov immediate to register
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

		// add regmem with reg to either FIXME: some stuff is busted
		if ((byte >> 2) == 0b000000)
		{
			// read in the secong byte for this instruction type
			int byte2 = fgetc(asm_file);
			//printf("add regmem to regmem: byte2 is %B : \n", byte2);
			// mod00 memory mode no displacement *except special case
			if ((byte2 >> 6) == 0b00)
			{
				/*
				 * the special case. for this instruction where regmem is 110 has a 2 byte
				 * displacement.
				 */
				if ((byte2 & 0b111) == 0b110)
				{
					printf("special case working");
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
			// mod01. memory mode displacement is 1byte.
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
							"add [%s + %i], %s\n",
							get_effective_address(byte2 & 0b111),
							byte3,
							get_reg(((byte&0b1)<<3) | ((byte2 >> 3) & 0b111))
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
			// mod10 memory mode 16bit displacement to follow
			if ((byte2 >> 6) == 0b10)
			{
				// get displacement byte
				int byte3 = fgetc(asm_file);
				// get displacement byte
				int byte4 = fgetc(asm_file);
				// bitwise OR together displacement bytes to make displacement value
				int displacement_value = ((byte4 << 8) | byte3);
				// w bit 1, wide mode
				if ((byte & 0b1) == 0b1)
				{
					printf
						(
						"add [%s], %i", 
						// retrieve effective address
						get_effective_address(byte2 & 0b111),
						// print displacement value
						displacement_value
						);
				}
				// w bit 0
				else
				{
					printf
						(
						"fix this: add %s, %i",
						// retrieve effective address
						get_effective_address(byte2 & 0b111),
						// print displacement value
						displacement_value
						);
				}
			}
			// mod11 register mode no displacement
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
		}

		// add immediate to regmem //TODO: fix this(mov import)
		if ((byte >> 2) == 0b100000)
		{
			// get second instruction byte
			int byte2 = fgetc(asm_file);
			//printf("add imm to regmem: byte2 is %B : \n", byte2);
			if (((byte2 >> 3) & 0b111) != 0b000)
			{
				fseek(asm_file, -1, SEEK_CUR);
				goto end_add;
			}
			// mod00, memory mode no displacement *except special case
			if ((byte2 >> 6) == 0b00)
			{
				// special case regmem equals 110. 16bit displacement to follow
				if ((byte2 & 0b111) == 0b110)
				{
					printf("    *add immediate to regmem, mod00 specail case. \n");
					// get displacement byte
					int byte3 = fgetc(asm_file);
					// get displacement byte
					int byte4 = fgetc(asm_file);
					// w bit check
					// w bit 1. wide instruction
					if (byte & 0b1)// FIXME: needs completing
					{
						// get data byte
						int byte5 = fgetc(asm_file);
						// get data byte
						int byte6 = fgetc(asm_file);
						printf("add mod 00 regmem 110 wide");
					}
					// w bit 0
					else// FIXME: needs completing
					{
						// get data byte
						int byte5 = fgetc(asm_file);
						printf("add mod 00 regmem 110 wide");
					}
				}
				// not special case, no displacement
				else
				{
					// FIXME this probably needs fixing,ie if s is one, sign extend byte3
					// w bit 1
					if (byte & 0b1)
					{
						printf("wide\n");
						// get data byte
						int byte3 = fgetc(asm_file);
						// get data byte
						int byte4 = fgetc(asm_file);
						// bitwise OR together data bytes to make immediate value
						int16_t immediate_value = ((byte4 << 8) | byte3);
						//print out instruction
						printf
							(
							"add [%s], %s %i\n",
							// retrieve effective address
							get_effective_address(byte2 & 0b111),
							// immediate size
							"word",
							// immediate value
							immediate_value 
							);
					}
					// w bit 0
					else
					{
						// get data byte (signed int 8bits)
						int8_t byte3 = fgetc(asm_file);
						//print out instruction
						printf
							(
							"add %s [%s], %i\n",
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
			// mod01, memory mode 8bit displacement follows// TODO: finish making this
			if ((byte2 >> 6) == 0b01)// TODO: finish making this
			{
				printf("the fuck is this");
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
					// FIXME s mode not incorporated properly
					// get data byte
					int byte5 = fgetc(asm_file);
					// bitwise OR together data bytes to make immediate value
					// if s and w is 1 then sign extend 8bits to 16bits
					/*if (byte >> 1 & 0b1)
					{
						int8_t immediate_value = byte5;
						printf("s is 1");
					}
					else
					{
						int16_t immediate_value = ((byte6 << 8) | byte5);
						printf("s is 0");
					}*/
					int8_t immediate_value = byte5;
					printf
						(
						"add %s [%s + %i], %i\n",
						// print immediate value bit width
						"word",
						// retrieve effective address
						get_effective_address(byte2 & 0b111),
						// print displacement value
						displacement_value,
						// print out the immediate value
						immediate_value
						);
				}
				// w bit 0
				else
				{
					// get data byte
					int byte5 = fgetc(asm_file);
					printf
						(
						"add %s + %i, %s %i\n",
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
			// mod11, add immediate to register, mode no displacement
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
							"add %s, %i\n",
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
						"debug add %s, %i\n",
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
						"add %s, %i\n : this immediate value needs checking",
						// get register
						get_reg((byte & 0b1) << 3 | (byte2 & 0b111)),
						// immediate value
						byte3
						);
				}
			}
		}

		// add immediate to accumulator
		if ((byte>>1) == 0b0000010)
		{
			// check W(wide) bit
			if ((byte & 0b1) == 0b1)
			{
				// if the wide bit is one, then a 16bit (2byte) data
				int byte2 = fgetc(asm_file);
				int byte3 = fgetc(asm_file);
				// print out asm instruction
				printf(
					"add %s, %i\n",
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
					"add %s, %i\n",
					// accumulator register
					"al",
					// value to add to accumulator
					byte2
					);
			}
		}

end_add:

		// sub regmem with reg to either FIXME: some stuff is busted
		if ((byte >> 2) == 0b001010)
		{
			// read in the secong byte for this instruction type
			int byte2 = fgetc(asm_file);
			//printf("sub regmem to regmem: byte2 is %B : \n", byte2);
			// mod00 memory mode no displacement *except special case
			if ((byte2 >> 6) == 0b00)
			{
				/*
				 * the special case. for this instruction where regmem is 110 has a 2 byte
				 * displacement.
				 */
				if ((byte2 & 0b111) == 0b110)
				{
					printf("special case working");
					// read in 3rd and 4th byte for displacement
					int byte3 = fgetc(asm_file);
					int byte4 = fgetc(asm_file);
					// if d bit is one, reg is destination
					if (((byte >> 1) & 0b1) == 0b1)
					{
						printf
							(
							"sub %s, [%i]\n",
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
							"sub [%i], %s\n",
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
							"sub %s, [%s]\n",
							get_reg(((byte&0b1)<<3) | ((byte2 >> 3) & 0b111)),
							get_effective_address(byte2 & 0b111)
							);
					}
					// if D bit is zero, regmem is the destination
					else
					{
						printf
							(
							"sub [%s], %s\n",
							get_effective_address(byte2 & 0b111),
							get_reg(((byte&0b1)<<3) | ((byte2 >> 3) & 0b111))
							);
					}
				}
			}
			// mod01. memory mode displacement is 1byte.
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
							"sub %s, [%s + %i]\n",
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
							"sub %s, [%s]\n",
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
							"sub [%s + %i], %s\n",
							get_effective_address(byte2 & 0b111),
							byte3,
							get_reg(((byte&0b1)<<3) | ((byte2 >> 3) & 0b111))
							);
					}
					// if byte3 is zero then don't add it into printf
					else
					{
						printf
							(
							"sub [%s], %s\n",
							get_effective_address(byte2 & 0b111),
							get_reg(((byte&0b1)<<3) | ((byte2 >> 3) & 0b111))
							);
					}
				}
			}
			// mod10 memory mode 16bit displacement to follow
			if ((byte2 >> 6) == 0b10)
			{
				// get displacement byte
				int byte3 = fgetc(asm_file);
				// get displacement byte
				int byte4 = fgetc(asm_file);
				// bitwise OR together displacement bytes to make displacement value
				int displacement_value = ((byte4 << 8) | byte3);
				// w bit 1, wide mode
				if ((byte & 0b1) == 0b1)
				{
					printf
						(
						"sub [%s], %i", 
						// retrieve effective address
						get_effective_address(byte2 & 0b111),
						// print displacement value
						displacement_value
						);
				}
				// w bit 0
				else
				{
					printf
						(
						"fix this: sub %s, %i",
						// retrieve effective address
						get_effective_address(byte2 & 0b111),
						// print displacement value
						displacement_value
						);
				}
			}
			// mod11 register mode no displacement
			if ((byte2 >> 6) == 0b11)
			{
					// D bit is one, reg is the destination
					if (((byte >> 1) & 0b1) == 0b1)
					{
						printf
							(
							"sub %s, %s\n",
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
							"sub %s, %s\n",
							// add wide bit to regmem to get the correct register
							get_reg(((byte&0b1)<<3) | (byte2 & 0b111)),
							// add wide bit to reg to get the correct register
							get_reg(((byte&0b1)<<3) | ((byte2 >> 3) & 0b111))
							);
					}
			}
		}

		// sub immediate to regmem
		if ((byte >> 2) == 0b100000)
		{
			// get second instruction byte
			int byte2 = fgetc(asm_file);
			//printf("sub imm to regmem: byte2 is %B : \n", byte2);
			if (((byte2 >> 3) & 0b111) != 0b101)
			{
				//printf("goto end_sub\n");
				fseek(asm_file, -1, SEEK_CUR);
				goto end_sub;
			}
			// mod00, memory mode no displacement *except special case
			if ((byte2 >> 6) == 0b00)
			{
				// special case regmem equals 110. 16bit displacement to follow
				if ((byte2 & 0b111) == 0b110)
				{
					printf("    *sub immediate to regmem, mod00 specail case. \n");
					// get displacement byte
					int byte3 = fgetc(asm_file);
					// get displacement byte
					int byte4 = fgetc(asm_file);
					// w bit check
					// w bit 1. wide instruction
					if (byte & 0b1)// FIXME: needs completing
					{
						// get data byte
						int byte5 = fgetc(asm_file);
						// get data byte
						int byte6 = fgetc(asm_file);
						printf("sub mod 00 regmem 110 wide");
					}
					// w bit 0
					else// FIXME: needs completing
					{
						// get data byte
						int byte5 = fgetc(asm_file);
						printf("sub mod 00 regmem 110 wide");
					}
				}
				// not special case, no displacement
				else
				{
					// w bit 1
					if (byte & 0b1)
					{
						// if w is 1 and s is 1, sign extend byte3
						if (((byte>1) & 0b1) == 0b1)
						{
							// get data byte
							int byte3 = fgetc(asm_file);
							//print out instruction
							printf
								(
								"sub %s [%s], %i\n",
								// immediate size
								"word",
								// retrieve effective address
								get_effective_address(byte2 & 0b111),
								// immediate value
								byte3
								);
						}
					}
					// w bit 0
					else
					{
						// get data byte (signed int 8bits)
						int8_t byte3 = fgetc(asm_file);
						//print out instruction
						printf
							(
							"sub %s [%s], %i\n",
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
			// mod01, memory mode 8bit displacement follows// TODO: finish making this
			if ((byte2 >> 6) == 0b01)// TODO: finish making this
			{
				printf("the fuck is this");
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
					// FIXME s mode not incorporated properly
					// get data byte
					int byte5 = fgetc(asm_file);
					// bitwise OR together data bytes to make immediate value
					// if s and w is 1 then sign extend 8bits to 16bits
					/*if (byte >> 1 & 0b1)
					{
						int8_t immediate_value = byte5;
						printf("s is 1");
					}
					else
					{
						int16_t immediate_value = ((byte6 << 8) | byte5);
						printf("s is 0");
					}*/
					int8_t immediate_value = byte5;
					printf
						(
						"sub %s [%s + %i], %i\n",
						// print immediate value bit width
						"word",
						// retrieve effective address
						get_effective_address(byte2 & 0b111),
						// print displacement value
						displacement_value,
						// print out the immediate value
						immediate_value
						);
				}
				// w bit 0
				else
				{
					// get data byte
					int byte5 = fgetc(asm_file);
					printf
						(
						"sub %s + %i, %s %i\n",
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
			// mod11, add immediate to register, mode no displacement
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
							"sub %s, %i\n",
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
						"debug sub %s, %i\n",
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
						"sub %s, %i\n : this immediate value needs checking",
						// get register
						get_reg((byte & 0b1) << 3 | (byte2 & 0b111)),
						// immediate value
						byte3
						);
				}
			}
		}

		// sub immediate to accumulator
		if ((byte>>1) == 0b0010110)
		{
			// check W(wide) bit
			if ((byte & 0b1) == 0b1)
			{
				// if the wide bit is one, then a 16bit (2byte) data
				int byte2 = fgetc(asm_file);
				int byte3 = fgetc(asm_file);
				// print out asm instruction
				printf(
					"sub %s, %i\n",
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
					"sub %s, %i\n",
					// accumulator register
					"al",
					// value to add to accumulator
					byte2
					);
			}
		}

end_sub:

		// cmp regmem with reg to either FIXME: some stuff is busted
		if ((byte >> 2) == 0b001110)
		{
			// read in the secong byte for this instruction type
			int byte2 = fgetc(asm_file);
			//printf("cmp regmem to regmem: byte2 is %B : \n", byte2);
			// mod00 memory mode no displacement *except special case
			if ((byte2 >> 6) == 0b00)
			{
				/*
				 * the special case. for this instruction where regmem is 110 has a 2 byte
				 * displacement.
				 */
				if ((byte2 & 0b111) == 0b110)
				{
					printf("special case working");
					// read in 3rd and 4th byte for displacement
					int byte3 = fgetc(asm_file);
					int byte4 = fgetc(asm_file);
					// if d bit is one, reg is destination
					if (((byte >> 1) & 0b1) == 0b1)
					{
						printf
							(
							"cmp %s, [%i]\n",
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
							"cmp [%i], %s\n",
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
							"cmp %s, [%s]\n",
							get_reg(((byte&0b1)<<3) | ((byte2 >> 3) & 0b111)),
							get_effective_address(byte2 & 0b111)
							);
					}
					// if D bit is zero, regmem is the destination
					else
					{
						printf
							(
							"cmp [%s], %s\n",
							get_effective_address(byte2 & 0b111),
							get_reg(((byte&0b1)<<3) | ((byte2 >> 3) & 0b111))
							);
					}
				}
			}
			// mod01. memory mode displacement is 1byte.
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
							"cmp %s, [%s + %i]\n",
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
							"cmp %s, [%s]\n",
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
							"cmp [%s + %i], %s\n",
							get_effective_address(byte2 & 0b111),
							byte3,
							get_reg(((byte&0b1)<<3) | ((byte2 >> 3) & 0b111))
							);
					}
					// if byte3 is zero then don't add it into printf
					else
					{
						printf
							(
							"cmp [%s], %s\n",
							get_effective_address(byte2 & 0b111),
							get_reg(((byte&0b1)<<3) | ((byte2 >> 3) & 0b111))
							);
					}
				}
			}
			// mod10 memory mode 16bit displacement to follow
			if ((byte2 >> 6) == 0b10)
			{
				// get displacement byte
				int byte3 = fgetc(asm_file);
				// get displacement byte
				int byte4 = fgetc(asm_file);
				// bitwise OR together displacement bytes to make displacement value
				int displacement_value = ((byte4 << 8) | byte3);
				// w bit 1, wide mode
				if ((byte & 0b1) == 0b1)
				{
					printf
						(
						"cmp [%s], %i", 
						// retrieve effective address
						get_effective_address(byte2 & 0b111),
						// print displacement value
						displacement_value
						);
				}
				// w bit 0
				else
				{
					printf
						(
						"fix this: cmp %s, %i",
						// retrieve effective address
						get_effective_address(byte2 & 0b111),
						// print displacement value
						displacement_value
						);
				}
			}
			// mod11 register mode no displacement
			if ((byte2 >> 6) == 0b11)
			{
					// D bit is one, reg is the destination
					if (((byte >> 1) & 0b1) == 0b1)
					{
						printf
							(
							"cmp %s, %s\n",
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
							"cmp %s, %s\n",
							// add wide bit to regmem to get the correct register
							get_reg(((byte&0b1)<<3) | (byte2 & 0b111)),
							// add wide bit to reg to get the correct register
							get_reg(((byte&0b1)<<3) | ((byte2 >> 3) & 0b111))
							);
					}
			}
		}

		// cmp immediate to regmem
		if ((byte >> 2) == 0b100000)
		{
			// get second instruction byte
			int byte2 = fgetc(asm_file);
			//printf("cmp imm to regmem: byte2 is %B : \n", byte2);
			if (((byte2 >> 3) & 0b111) != 0b111)
			{
				//printf("goto end_cmp\n");
				fseek(asm_file, -1, SEEK_CUR);
				goto end_cmp;
			}
			// mod00, memory mode no displacement *except special case
			if ((byte2 >> 6) == 0b00)
			{
				// special case regmem equals 110. 16bit displacement to follow
				if ((byte2 & 0b111) == 0b110)
				{
					printf("    *cmp immediate to regmem, mod00 specail case. \n");
					// get displacement byte
					int byte3 = fgetc(asm_file);
					// get displacement byte
					int byte4 = fgetc(asm_file);
					// w bit check
					// w bit 1. wide instruction
					if (byte & 0b1)// FIXME: needs completing
					{
						// get data byte
						int byte5 = fgetc(asm_file);
						// get data byte
						int byte6 = fgetc(asm_file);
						printf("cmp mod 00 regmem 110 wide");
					}
					// w bit 0
					else// FIXME: needs completing
					{
						// get data byte
						int byte5 = fgetc(asm_file);
						printf("cmp mod 00 regmem 110 wide");
					}
				}
				// not special case, no displacement
				else
				{
					// w bit 1
					if (byte & 0b1)
					{
						// if w is 1 and s is 1, sign extend byte3
						if (((byte>1) & 0b1) == 0b1)
						{
							// get data byte
							int byte3 = fgetc(asm_file);
							//print out instruction
							printf
								(
								"cmp %s [%s], %i\n",
								// immediate size
								"word",
								// retrieve effective address
								get_effective_address(byte2 & 0b111),
								// immediate value
								byte3
								);
						}
					}
					// w bit 0
					else
					{
						// get data byte (signed int 8bits)
						int8_t byte3 = fgetc(asm_file);
						//print out instruction
						printf
							(
							"cmp %s [%s], %i\n",
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
			// mod01, memory mode 8bit displacement follows// TODO: finish making this
			if ((byte2 >> 6) == 0b01)// TODO: finish making this
			{
				printf("the fuck is this");
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
					// FIXME s mode not incorporated properly
					// get data byte
					int byte5 = fgetc(asm_file);
					// bitwise OR together data bytes to make immediate value
					// if s and w is 1 then sign extend 8bits to 16bits
					/*if (byte >> 1 & 0b1)
					{
						int8_t immediate_value = byte5;
						printf("s is 1");
					}
					else
					{
						int16_t immediate_value = ((byte6 << 8) | byte5);
						printf("s is 0");
					}*/
					int8_t immediate_value = byte5;
					printf
						(
						"cmp %s [%s + %i], %i\n",
						// print immediate value bit width
						"word",
						// retrieve effective address
						get_effective_address(byte2 & 0b111),
						// print displacement value
						displacement_value,
						// print out the immediate value
						immediate_value
						);
				}
				// w bit 0
				else
				{
					// get data byte
					int byte5 = fgetc(asm_file);
					printf
						(
						"cmp %s + %i, %s %i\n",
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
			// mod11, add immediate to register, mode no displacement
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
							"cmp %s, %i\n",
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
						"debug cmp %s, %i\n",
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
						"cmp %s, %i\n : this immediate value needs checking",
						// get register
						get_reg((byte & 0b1) << 3 | (byte2 & 0b111)),
						// immediate value
						byte3
						);
				}
			}
		}

		// cmp immediate to accumulator
		if ((byte>>1) == 0b0010110)
		{
			// check W(wide) bit
			if ((byte & 0b1) == 0b1)
			{
				// if the wide bit is one, then a 16bit (2byte) data
				int byte2 = fgetc(asm_file);
				int byte3 = fgetc(asm_file);
				// print out asm instruction
				printf(
					"cmp %s, %i\n",
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
					"cmp %s, %i\n",
					// accumulator register
					"al",
					// value to add to accumulator
					byte2
					);
			}
		}

end_cmp:

	}

	// close the file when done(to release the resources it was using)
	fclose(asm_file);

	// exit with success code
	return(0);
}

/* function definitions */

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
