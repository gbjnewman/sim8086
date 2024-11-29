#include <stdio.h>
#include <stdlib.h> // so i can use exit(EXIT_FAILURE)
#include <stdint.h> // so i can use int8_t
#include <string.h> // so i can use strcmp

// function prototypes
char* get_reg_string(int reg);
char* get_effective_address(int regmem);
void regmem_tofrom_regmem(FILE* asm_file, int byte, char* instruction);
void immediate_to_regmem(FILE* asm_file, int byte, char* instruction);
void immediate_to_register(FILE* asm_file, int byte, char* instruction);
void immediate_to_accumulator(FILE* asm_file, int byte, char* instruction);
void jump_instruction(FILE* asm_file, int byte);
void print_registers();
void write_to_reg(char* reg, uint16_t value);
uint16_t get_reg_value(char* reg);

// GLOBALS
// execute flag
int execute_flag = 0;
// make registers for the simulator
uint16_t regs[8] = {0};
// register names
const char *reg_names[8] = {"ax", "bx", "cx", "dx", "sp", "bp", "si", "di"};
// set variables for the individual registers
uint16_t *ax = &regs[0];
uint16_t *bx = &regs[1];
uint16_t *cx = &regs[2];
uint16_t *dx = &regs[3];
uint16_t *sp = &regs[4];
uint16_t *bp = &regs[5];
uint16_t *si = &regs[6];
uint16_t *di = &regs[7];


/* START MAIN FUNCTION */
int main(int argc, char *argv[])
{

	// process commandline flags
	for (int i = 0; i < argc; i++)
	{
		if (strcmp(argv[i], "--execute") == 0 || strcmp(argv[i], "-e") == 0)
		{
			execute_flag = 1;
			printf("execute flag is set\n");
		};
	}

	// check if no arguments to main program are given
	if (argc < 2)
	{
		printf("No file given, please provide one ASM file to decode\n");
		exit(EXIT_FAILURE);
	}

	/* // not in use because flags(-e, --execute) have been implemented
	// check if too many arguments to main program are given
	if (argc > 2)
	{
		printf("Too many arguments given.\n");
		exit(EXIT_FAILURE);
	}
	*/

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

	// start processing the asm instruction stream
	int byte;
	while ((byte = fgetc(asm_file)) != EOF)
	{

		// mov regmem to/from reg
		if ((byte >> 2) == 0b100010)
		{
			regmem_tofrom_regmem(asm_file, byte, "mov");
		}

		// mov immediate to regmem
		else if ((byte >> 1) == 0b1100011)
		{
			immediate_to_regmem(asm_file, byte, "mov");
		}

		// mov immediate to register
		else if ((byte >> 4) == 0b1011)
		{
			immediate_to_register(asm_file, byte, "mov");
		}

		// mov memory to accumulator
		else if ((byte >> 1) == 0b1010000)
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
		else if ((byte >> 1) == 0b1010001)
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
		else if ((byte >> 2) == 0b100000)
		{
			char instruction[4];
			immediate_to_regmem(asm_file, byte, instruction);
		}

		// add regmem to/from reg
		else if ((byte >> 2) == 0b000000)
		{
			char instruction[] = "add";
			regmem_tofrom_regmem(asm_file, byte, instruction);
		}

		// add immediate to accumulator
		else if ((byte>>1) == 0b0000010)
		{
			char instruction[] = "add";
			immediate_to_accumulator(asm_file, byte, instruction);
		}

		// sub regmem to/from reg
		else if ((byte >> 2) == 0b001010)
		{
			char instruction[] = "sub";
			regmem_tofrom_regmem(asm_file, byte, instruction);
		}

		// sub immediate to accumulator
		else if ((byte>>1) == 0b0010110)
		{
			char instruction[] = "sub";
			immediate_to_accumulator(asm_file, byte, instruction);
		}

		// cmp regmem to/from reg
		else if ((byte >> 2) == 0b001110)
		{
			char instruction[] = "cmp";
			regmem_tofrom_regmem(asm_file, byte, instruction);
		}

		// cmp immediate to accumulator
		else if ((byte>>1) == 0b0011110)
		{
			char instruction[] = "cmp";
			immediate_to_accumulator(asm_file, byte, instruction);
		}

		// jump instructions
		else if ((byte >> 4) == 0b0111)
		{
			jump_instruction(asm_file, byte);
		}

		else {
			printf("not valid instruction\n");
					break;
		}

	}
	if (execute_flag)
	{
		//print out registers before exiting
		printf("Final registers:\n");
		print_registers();
	}

	// close the file when done(to release the resources it was using)
	fclose(asm_file);

	// exit with success code
	return(0);
}

/* function definitions */

void regmem_tofrom_regmem(FILE* asm_file, int byte, char* instruction)
{

	// read in the secong byte for this instruction set
	int byte2 = fgetc(asm_file);

	// get destination bit
	int dbit = ((byte >> 1) & 0b1);

	// get register
	char* reg = get_reg_string(((byte&0b1)<<3) | ((byte2 >> 3) & 0b111));

	// get register/memory
	char* regmem = get_reg_string(((byte&0b1)<<3) | (byte2 & 0b111));

	// get effective address
	char* effaddr =
		get_effective_address(byte2 & 0b111);

	// print out the instruction mnemonic
	printf("%s ", instruction);

	// mod00. no displacement, except special case
	if ((byte2 >> 6) == 0b00)
	{
		// special case of mod00(reg110 direct address). two byte displacement.
		if ((byte2 & 0b111) == 0b110)
		{
			// read in 3rd and 4th byte for displacement
			int byte3 = fgetc(asm_file);
			int byte4 = fgetc(asm_file);

			// bitwise OR together byte4 and byte3 for displacemt value
			int16_t disp_val = ((byte4 << 8) | byte3);

			dbit ? // if d bit is one, reg is destination
				printf("%s, [%i]", reg, disp_val)
				: // if d bit is zero, reg is source
				printf("[%i], %s", disp_val, reg);
		}

		// normal case of mod00(reg is not 110). no dispacement.
		else
		{
			dbit ? // if d bit is one, reg is destination
				printf("%s, [%s]", reg, effaddr)
				: // if d bit is zero, reg is source
				printf("[%s], %s", effaddr, reg);
		}

	} // mod01. displacement one byte.
	if ((byte2 >> 6) == 0b01)
	{
		// read in 3rd byte for displacement
		int8_t byte3 = fgetc(asm_file);

		// check if D bit is 1
		dbit ? // if D is 1 reg is destination
			(byte3 != 0) ? // if byte3 is not zero add it into printf
						printf("%s, [%s %s %i]", reg, effaddr,
						// string for the sign
						byte3 < 0 ? "-" : "+",
						// displacement value(with remove sign hack)
						byte3 < 0 ? -byte3 : byte3)
						: // if byte3 is zero then don't add it into printf
						printf("%s, [%s]", reg, effaddr)
		: // if D is 0 reg is source
			(byte3 != 0) ? // if byte3 is not zero add it into printf
						printf("[%s %s %i], %s",
						// effective address
						effaddr,
						// string for the sign
						byte3 < 0 ? "-" : "+",
						// displacement value
						byte3 < 0 ? -byte3 : byte3,
						// register
						reg
						)
						: // if byte3 is zero then don't add it into printf
						printf("[%s], %s", effaddr, reg);
	}

	// mod10. memory mode displacement is 2 byte.
	if ((byte2 >> 6) == 0b10)
	{

		// read in 3rd byte for displacement
		int byte3 = fgetc(asm_file);

		// read in 4th byte for displacement
		int byte4 = fgetc(asm_file);

		// displacemt value
		int16_t disp_val = ((byte4 << 8) | byte3);

		dbit ? // if D is 1 reg is destination
			// if byte3 and byte4 are not 0, add there value into printf
			((byte3 != 0) && (byte4 !=0))
				? printf("%s, [%s + %i]", reg, effaddr, disp_val)
				// if byte3 and byte4 are 0, don't add their value into printf
				: printf("%s, [%s]", reg, effaddr)

			: // if D is 0 reg is source
			// if byte3 and byte4 are not 0, add there value into printf
			(byte3 != 0) && (byte4 !=0)
				? printf("[%s %s %i], %s", effaddr,
					// print displacemt sign string
					disp_val < 0 ? "-" : "+",
					// displacement value(displacement sign hack)
					disp_val < 0 ? -disp_val : disp_val,
					// register
					reg)
				// if byte3 and byte4 are 0, don't add their value into printf
				:
					// if byte3 is 0 then don't add it into printf
					printf("[%s], %s\n", effaddr, reg);
	}

	// mod11 register mode, no displacement
	if ((byte2 >> 6) == 0b11)
	{
			dbit ? // D bit is one, reg is destination
				printf("%s, %s", reg, regmem)
				:// D bit is zero, regmem is destination
				printf("%s, %s", regmem, reg);
	}

	// make a newline at the end of the instruction
	printf("\n");
}

void immediate_to_regmem(FILE* asm_file, int byte, char* instruction)
{
	// get instruction byte
	int byte2 = fgetc(asm_file);
	int sbit = 0;

	if ((byte >> 2) == 0b100000)
	{
		sbit = (byte >> 1) &0b1;
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
		// immediate to direct address. speical case 110 is 16bit displacement
		if ((byte2 & 0b111) == 0b110)
		{
			//printf("*immediate to direct address, mod00 special case. \n");
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
				printf("fix this imm to direct, byte 5 is %i\n", byte5);
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
					"%s [%s], %s %i\n",
					instruction,
					// retrieve effective address
					get_effective_address(byte2 & 0b111),
					// immediate size
					"word",
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
					"%s [%s], %s %i\n",
					instruction,
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

	// mod01, memory mode, 8bit displacement follows// TODO: finish this
	if ((byte2 >> 6) == 0b01)// TODO: finish making this
	{
		// get displacement byte
		int byte3 = fgetc(asm_file);
		printf("    mod01, byte 3 is %i", byte3);
	}

	// mod10, memory mode, 16bit displacement follows
	if ((byte2 >> 6) == 0b10)
	{

		// get displacement bytes
		int byte3 = fgetc(asm_file);
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
				"%s [%s + %i], %s %i\n",
				instruction,
				// retrieve effective address
				get_effective_address(byte2 & 0b111),
				// print displacement value
				displacement_value,
				// print immediate value bit width
				"word",
				// print out the immediate value
				byte5
				);
		}
		// w bit 0
		else
		{
			// get data byte
			int8_t byte5 = fgetc(asm_file);
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
					get_reg_string((byte & 0b1) << 3 | (byte2 & 0b111)),
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
				get_reg_string((byte & 0b1) << 3 | (byte2 & 0b111)),
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
				get_reg_string((byte & 0b1) << 3 | (byte2 & 0b111)),
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
	int16_t value = 0;
	char* reg_string = get_reg_string(((byte >> 3) & 0b1) << 3 | (byte & 0b111));

	// check W(wide) bit
	// if W is one
	if ((byte >> 3) & 0b1)
	{
		// if the wide bit is one, then a 16bit (2byte) displacement is needed
		int16_t byte2 = fgetc(asm_file);
		int16_t byte3 = fgetc(asm_file);
		value = ((byte3 << 8) | byte2);
	// if wide is zero
	} else {
		// if the wide bit is zero, then only an 8bit displacement is needed
		int8_t byte2 = fgetc(asm_file);
		value = byte2;
	}

	// execute if flag is set
	if (execute_flag == 1)
	{
		//get original value to add to printf
		int orig_reg_value = get_reg_value(reg_string);
		// write the new value to the register
		write_to_reg(reg_string, value);
		// get new value to add to printf
		int new_reg_value = get_reg_value(reg_string);
		// print out asm instruction
		printf(
			"%s %s, %i ; %s:0x%i->0x%i\n",
			instruction,
			// retrieve the correct register name
			reg_string,
			// value from the displacement byte/s
			value,
			reg_string,
			orig_reg_value,
			new_reg_value
			);
	}
	else
	{
	// print out asm instruction
	printf(
		"%s %s, %i\n",
		instruction,
		// retrieve the correct register name
		//get_reg_string(((byte >> 3) & 0b1) << 3 | (byte & 0b111)),
		reg_string,
		// calculate the value from the displacement byte/s
		//((byte3 << 8) | byte2)
		value
		);
	}
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
		// if the wide bit is zero, then only an 8bit displacement is needed
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

void jump_instruction(FILE* asm_file, int byte)
{
	int8_t byte2 = fgetc(asm_file);
	char* instruction = NULL;

	switch (byte & 0b1111)
	{
		case 0b0100:
			instruction = "je";
			break;
		case 0b1100:
			instruction = "jl";
			break;
		case 0b1110:
			instruction = "jle";
			break;
		case 0b0010:
			instruction = "jb";
			break;
		case 0b0110:
			instruction = "jbe";
			break;
		case 0b1010:
			instruction = "jp";
			break;
		case 0b0000:
			instruction = "jo";
			break;
		case 0b1000:
			instruction = "js";
			break;
		case 0b0101:
			instruction = "jne";
			break;
		case 0b1101:
			instruction = "jnl";
			break;
		case 0b1111:
			instruction = "jg";
			break;
		case 0b0011:
			instruction = "jnb";
			break;
		case 0b0111:
			instruction = "ja";
			break;
		case 0b1011:
			instruction = "jnp";
			break;
		case 0b0001:
			instruction = "jno";
			break;
		case 0b1001:
			instruction = "jns";
			break;
	}
	printf("%s %i\n", instruction, byte2);
}

// register array retrieval
char* get_reg_string(int reg)
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

void print_registers()
{
	for (size_t i = 0; i < (sizeof(regs) / sizeof(regs[0])); i++)
	{
		printf("      %s: 0x%04x\n", get_reg_string(i+8), regs[i]);
	}
}

void write_to_reg(char* reg, uint16_t value)
{
	if (strcmp(reg, "ax") == 0)
	{
		*ax = value;
	}
	if (strcmp(reg, "bx") == 0)
	{
		*bx = value;
	}
	if (strcmp(reg, "cx") == 0)
	{
		*cx = value;
	}
	if (strcmp(reg, "dx") == 0)
	{
		*dx = value;
	}
	if (strcmp(reg, "sp") == 0)
	{
		*sp = value;
	}
	if (strcmp(reg, "bp") == 0)
	{
		*bp = value;
	}
	if (strcmp(reg, "si") == 0)
	{
		*si = value;
	}
	if (strcmp(reg, "di") == 0)
	{
		*di = value;
	}
}

uint16_t get_reg_value(char* reg)
{
	if (strcmp(reg, "ax") == 0)
	{
		return *ax;
	}
	if (strcmp(reg, "bx") == 0)
	{
		return *bx;
	}
	if (strcmp(reg, "cx") == 0)
	{
		return *cx;
	}
	if (strcmp(reg, "dx") == 0)
	{
		return *dx;
	}
	if (strcmp(reg, "sp") == 0)
	{
		return *sp;
	}
	if (strcmp(reg, "bp") == 0)
	{
		return *bp;
	}
	if (strcmp(reg, "si") == 0)
	{
		return *si;
	}
	if (strcmp(reg, "di") == 0)
	{
		return *di;
	}
	return 0;
}



