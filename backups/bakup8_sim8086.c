#include <stdio.h>
#include <stdlib.h> // so i can use exit(EXIT_FAILURE)
#include <stdint.h> // so i can use int8_t
#include <string.h> // so i can use strcmp

// FUNCTION PROTOTYPES
char* get_reg_string(int reg);
char* get_effective_address(int regmem);
void regmem_tofrom_regmem(FILE* asm_file, int byte, char* instruction);
void immediate_to_regmem(FILE* asm_file, int byte, char* instruction);
void immediate_to_register(FILE* asm_file, int byte, char* instruction);
void immediate_to_accumulator(FILE* asm_file, int byte, char* instruction);
void memory_with_accumulator(FILE* asm_file, int byte, char* instruction);
void mod00(FILE* asm_file, int byte, int byte2, char* instruction, int sbit);
void mod01(FILE* asm_file, int byte, int byte2, char* instruction, int sbit);
void mod10(FILE* asm_file, int byte, int byte2, char* instruction, int sbit);
void mod11(FILE* asm_file, int byte, int byte2, char* instruction, int sbit);
void jump_instruction(FILE* asm_file, int byte);
void print_registers(uint16_t* registers, size_t array_size);
void write_to_reg(char* reg, uint16_t value);
uint16_t get_reg_value(char* reg);

// GLOBALS
// execute flag
int execute_flag = 0;
// flags array
const char *flags[2] = {" "," "};
// sign flag
const char **sign_flag = &flags[0];
// zero flag
const char **zero_flag = &flags[1];
// make registers for the simulator
uint16_t regs[8] = {0};
// register names
const char *reg_names[8] = {"ax", "cx", "dx", "bx", "sp", "bp", "si", "di"};
// set variables for the individual registers
uint16_t *ax = &regs[0];
uint16_t *cx = &regs[1];
uint16_t *dx = &regs[2];
uint16_t *bx = &regs[3];
uint16_t *sp = &regs[4];
uint16_t *bp = &regs[5];
uint16_t *si = &regs[6];
uint16_t *di = &regs[7];


/* START MAIN FUNCTION */
int main(int argc, char *argv[])
{

	// check if no arguments to the program are given
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

	// process commandline flags
	for (int i = 0; i < argc; i++)
	{
		if (strcmp(argv[i], "--execute") == 0 || strcmp(argv[i], "-e") == 0)
		{
			execute_flag = 1;
			//printf("execute flag is set\n");
		}
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

	// start processing the asm instruction stream
	int byte;
	while ((byte = fgetc(asm_file)) != EOF)
	{
		// copy registers into a new array to compare at end
		uint16_t regs_orig[8];
		memcpy(regs_orig, regs, sizeof(regs));

		// copy flags array into new array for end comparison
		const char *flags_orig[2];
		memcpy(flags_orig, flags, sizeof(flags));
		//print_registers(regs_orig, (sizeof(regs_orig) / sizeof(regs_orig[0])));

		switch (byte)
		{

			// add regmem to/from reg
			case 0b00000000: // fall through
			case 0b00000001: // fall through
			case 0b00000010: // fall through
			case 0b00000011:
				regmem_tofrom_regmem(asm_file, byte, "add");
				break;

			// add immediate to accumulator
			case 0b00000100: // fall through
			case 0b00000101:
				immediate_to_accumulator(asm_file, byte, "add");
				break;

			case 0b00000110: printf("not implemented\n"); break;
			case 0b00000111: printf("not implemented\n"); break;
			case 0b00001000: printf("not implemented\n"); break;
			case 0b00001001: printf("not implemented\n"); break;
			case 0b00001010: printf("not implemented\n"); break;
			case 0b00001011: printf("not implemented\n"); break;
			case 0b00001100: printf("not implemented\n"); break;
			case 0b00001101: printf("not implemented\n"); break;
			case 0b00001110: printf("not implemented\n"); break;
			case 0b00001111: printf("not implemented\n"); break;
			case 0b00010000: printf("not implemented\n"); break;
			case 0b00010001: printf("not implemented\n"); break;
			case 0b00010010: printf("not implemented\n"); break;
			case 0b00010011: printf("not implemented\n"); break;
			case 0b00010100: printf("not implemented\n"); break;
			case 0b00010101: printf("not implemented\n"); break;
			case 0b00010110: printf("not implemented\n"); break;
			case 0b00010111: printf("not implemented\n"); break;
			case 0b00011000: printf("not implemented\n"); break;
			case 0b00011001: printf("not implemented\n"); break;
			case 0b00011010: printf("not implemented\n"); break;
			case 0b00011011: printf("not implemented\n"); break;
			case 0b00011100: printf("not implemented\n"); break;
			case 0b00011101: printf("not implemented\n"); break;
			case 0b00011110: printf("not implemented\n"); break;
			case 0b00011111: printf("not implemented\n"); break;
			case 0b00100000: printf("not implemented\n"); break;
			case 0b00100001: printf("not implemented\n"); break;
			case 0b00100010: printf("not implemented\n"); break;
			case 0b00100011: printf("not implemented\n"); break;
			case 0b00100100: printf("not implemented\n"); break;
			case 0b00100101: printf("not implemented\n"); break;
			case 0b00100110: printf("not implemented\n"); break;
			case 0b00100111: printf("not implemented\n"); break;

			// sub regmem to/from reg
			case 0b00101000: // fall through
			case 0b00101001: // fall through
			case 0b00101010: // fall through
			case 0b00101011:
				regmem_tofrom_regmem(asm_file, byte, "sub");
				break;

			// sub immediate to accumulator
			case 0b00101100: // fall through
			case 0b00101101: // fall through
			case 0b00101110: // fall through
			case 0b00101111:
				immediate_to_accumulator(asm_file, byte, "sub");
				break;

			case 0b00110000: printf("not implemented\n"); break;
			case 0b00110001: printf("not implemented\n"); break;
			case 0b00110010: printf("not implemented\n"); break;
			case 0b00110011: printf("not implemented\n"); break;
			case 0b00110100: printf("not implemented\n"); break;
			case 0b00110101: printf("not implemented\n"); break;
			case 0b00110110: printf("not implemented\n"); break;
			case 0b00110111: printf("not implemented\n"); break;

			// cmp regmem to/from reg
			case 0b00111000: // fall through
			case 0b00111001: // fall through
			case 0b00111010: // fall through
			case 0b00111011:
				regmem_tofrom_regmem(asm_file, byte, "cmp");
				break;

			// cmp immediate to accumulator
			case 0b00111100: // fall through
			case 0b00111101:
				immediate_to_accumulator(asm_file, byte, "cmp");
				break;

			case 0b00111110: printf("not implemented\n"); break;
			case 0b00111111: printf("not implemented\n"); break;
			case 0b01000000: printf("not implemented\n"); break;
			case 0b01000001: printf("not implemented\n"); break;
			case 0b01000010: printf("not implemented\n"); break;
			case 0b01000011: printf("not implemented\n"); break;
			case 0b01000100: printf("not implemented\n"); break;
			case 0b01000101: printf("not implemented\n"); break;
			case 0b01000110: printf("not implemented\n"); break;
			case 0b01000111: printf("not implemented\n"); break;
			case 0b01001000: printf("not implemented\n"); break;
			case 0b01001001: printf("not implemented\n"); break;
			case 0b01001010: printf("not implemented\n"); break;
			case 0b01001011: printf("not implemented\n"); break;
			case 0b01001100: printf("not implemented\n"); break;
			case 0b01001101: printf("not implemented\n"); break;
			case 0b01001110: printf("not implemented\n"); break;
			case 0b01001111: printf("not implemented\n"); break;
			case 0b01010000: printf("not implemented\n"); break;
			case 0b01010001: printf("not implemented\n"); break;
			case 0b01010010: printf("not implemented\n"); break;
			case 0b01010011: printf("not implemented\n"); break;
			case 0b01010100: printf("not implemented\n"); break;
			case 0b01010101: printf("not implemented\n"); break;
			case 0b01010110: printf("not implemented\n"); break;
			case 0b01010111: printf("not implemented\n"); break;
			case 0b01011000: printf("not implemented\n"); break;
			case 0b01011001: printf("not implemented\n"); break;
			case 0b01011010: printf("not implemented\n"); break;
			case 0b01011011: printf("not implemented\n"); break;
			case 0b01011100: printf("not implemented\n"); break;
			case 0b01011101: printf("not implemented\n"); break;
			case 0b01011110: printf("not implemented\n"); break;
			case 0b01011111: printf("not implemented\n"); break;
			case 0b01100000: printf("not implemented\n"); break;
			case 0b01100001: printf("not implemented\n"); break;
			case 0b01100010: printf("not implemented\n"); break;
			case 0b01100011: printf("not implemented\n"); break;
			case 0b01100100: printf("not implemented\n"); break;
			case 0b01100101: printf("not implemented\n"); break;
			case 0b01100110: printf("not implemented\n"); break;
			case 0b01100111: printf("not implemented\n"); break;
			case 0b01101000: printf("not implemented\n"); break;
			case 0b01101001: printf("not implemented\n"); break;
			case 0b01101010: printf("not implemented\n"); break;
			case 0b01101011: printf("not implemented\n"); break;
			case 0b01101100: printf("not implemented\n"); break;
			case 0b01101101: printf("not implemented\n"); break;
			case 0b01101110: printf("not implemented\n"); break;
			case 0b01101111: printf("not implemented\n"); break;

			// jump instructions
			case 0b01110000: // fall through
			case 0b01110001: // fall through
			case 0b01110010: // fall through
			case 0b01110011: // fall through
			case 0b01110100: // fall through
			case 0b01110101: // fall through
			case 0b01110110: // fall through
			case 0b01110111: // fall through
			case 0b01111000: // fall through
			case 0b01111001: // fall through
			case 0b01111010: // fall through
			case 0b01111011: // fall through
			case 0b01111100: // fall through
			case 0b01111101: // fall through
			case 0b01111110: // fall through
			case 0b01111111:
				jump_instruction(asm_file, byte);
				break;

			// add, sub, cmp immediate to regmem
			case 0b10000000: // fall through
			case 0b10000001: // fall through
			case 0b10000010: // fall through
			case 0b10000011:
				immediate_to_regmem(asm_file, byte, "xxx");
				break;

			case 0b10000100: printf("not implemented\n"); break;
			case 0b10000101: printf("not implemented\n"); break;
			case 0b10000110: printf("not implemented\n"); break;
			case 0b10000111: printf("not implemented\n"); break;

			// mov regmem to/from reg
			case 0b10001000: //fall through
			case 0b10001001: //fall through
			case 0b10001010: //fall through
			case 0b10001011:
				regmem_tofrom_regmem(asm_file, byte, "mov");
				break;

			case 0b10001100: printf("not implemented\n"); break;
			case 0b10001101: printf("not implemented\n"); break;
			case 0b10001110: printf("not implemented\n"); break;
			case 0b10001111: printf("not implemented\n"); break;
			case 0b10010000: printf("not implemented\n"); break;
			case 0b10010001: printf("not implemented\n"); break;
			case 0b10010010: printf("not implemented\n"); break;
			case 0b10010011: printf("not implemented\n"); break;
			case 0b10010100: printf("not implemented\n"); break;
			case 0b10010101: printf("not implemented\n"); break;
			case 0b10010110: printf("not implemented\n"); break;
			case 0b10010111: printf("not implemented\n"); break;
			case 0b10011000: printf("not implemented\n"); break;
			case 0b10011001: printf("not implemented\n"); break;
			case 0b10011010: printf("not implemented\n"); break;
			case 0b10011011: printf("not implemented\n"); break;
			case 0b10011100: printf("not implemented\n"); break;
			case 0b10011101: printf("not implemented\n"); break;
			case 0b10011110: printf("not implemented\n"); break;
			case 0b10011111: printf("not implemented\n"); break;

			// memory to accumulator
			case 0b10100000: // fall through
			case 0b10100001: // fall through
			// accumulator to memory
			case 0b10100010: // fall through
			case 0b10100011:
				memory_with_accumulator(asm_file, byte, "mov");
				break;

			case 0b10100100: printf("not implemented\n"); break;
			case 0b10100101: printf("not implemented\n"); break;
			case 0b10100110: printf("not implemented\n"); break;
			case 0b10100111: printf("not implemented\n"); break;
			case 0b10101000: printf("not implemented\n"); break;
			case 0b10101001: printf("not implemented\n"); break;
			case 0b10101010: printf("not implemented\n"); break;
			case 0b10101011: printf("not implemented\n"); break;
			case 0b10101100: printf("not implemented\n"); break;
			case 0b10101101: printf("not implemented\n"); break;
			case 0b10101110: printf("not implemented\n"); break;
			case 0b10101111: printf("not implemented\n"); break;

			// mov immediate to register
			case 0b10110000: // fall through
			case 0b10110001: // fall through
			case 0b10110010: // fall through
			case 0b10110011: // fall through
			case 0b10110100: // fall through
			case 0b10110101: // fall through
			case 0b10110110: // fall through
			case 0b10110111: // fall through
			case 0b10111000: // fall through
			case 0b10111001: // fall through
			case 0b10111010: // fall through
			case 0b10111011: // fall through
			case 0b10111100: // fall through
			case 0b10111101: // fall through
			case 0b10111110: // fall through
			case 0b10111111:
				immediate_to_register(asm_file, byte, "mov");
				break;

			case 0b11000000: printf("not implemented\n"); break;
			case 0b11000001: printf("not implemented\n"); break;
			case 0b11000010: printf("not implemented\n"); break;
			case 0b11000011: printf("not implemented\n"); break;
			case 0b11000100: printf("not implemented\n"); break;
			case 0b11000101: printf("not implemented\n"); break;

			// mov immediate to regmem
			case 0b11000110:
			case 0b11000111:
				immediate_to_regmem(asm_file, byte, "mov");
				break;

			case 0b11001000: printf("not implemented\n"); break;
			case 0b11001001: printf("not implemented\n"); break;
			case 0b11001010: printf("not implemented\n"); break;
			case 0b11001011: printf("not implemented\n"); break;
			case 0b11001100: printf("not implemented\n"); break;
			case 0b11001101: printf("not implemented\n"); break;
			case 0b11001110: printf("not implemented\n"); break;
			case 0b11001111: printf("not implemented\n"); break;
			case 0b11010000: printf("not implemented\n"); break;
			case 0b11010001: printf("not implemented\n"); break;
			case 0b11010010: printf("not implemented\n"); break;
			case 0b11010011: printf("not implemented\n"); break;
			case 0b11010100: printf("not implemented\n"); break;
			case 0b11010101: printf("not implemented\n"); break;
			case 0b11010110: printf("not implemented\n"); break;
			case 0b11010111: printf("not implemented\n"); break;
			case 0b11011000: printf("not implemented\n"); break;
			case 0b11011001: printf("not implemented\n"); break;
			case 0b11011010: printf("not implemented\n"); break;
			case 0b11011011: printf("not implemented\n"); break;
			case 0b11011100: printf("not implemented\n"); break;
			case 0b11011101: printf("not implemented\n"); break;
			case 0b11011110: printf("not implemented\n"); break;
			case 0b11011111: printf("not implemented\n"); break;
			case 0b11100000: printf("not implemented\n"); break;
			case 0b11100001: printf("not implemented\n"); break;
			case 0b11100010: printf("not implemented\n"); break;
			case 0b11100011: printf("not implemented\n"); break;
			case 0b11100100: printf("not implemented\n"); break;
			case 0b11100101: printf("not implemented\n"); break;
			case 0b11100110: printf("not implemented\n"); break;
			case 0b11100111: printf("not implemented\n"); break;
			case 0b11101000: printf("not implemented\n"); break;
			case 0b11101001: printf("not implemented\n"); break;
			case 0b11101010: printf("not implemented\n"); break;
			case 0b11101011: printf("not implemented\n"); break;
			case 0b11101100: printf("not implemented\n"); break;
			case 0b11101101: printf("not implemented\n"); break;
			case 0b11101110: printf("not implemented\n"); break;
			case 0b11101111: printf("not implemented\n"); break;
			case 0b11110000: printf("not implemented\n"); break;
			case 0b11110001: printf("not implemented\n"); break;
			case 0b11110010: printf("not implemented\n"); break;
			case 0b11110011: printf("not implemented\n"); break;
			case 0b11110100: printf("not implemented\n"); break;
			case 0b11110101: printf("not implemented\n"); break;
			case 0b11110110: printf("not implemented\n"); break;
			case 0b11110111: printf("not implemented\n"); break;
			case 0b11111000: printf("not implemented\n"); break;
			case 0b11111001: printf("not implemented\n"); break;
			case 0b11111010: printf("not implemented\n"); break;
			case 0b11111011: printf("not implemented\n"); break;
			case 0b11111100: printf("not implemented\n"); break;
			case 0b11111101: printf("not implemented\n"); break;
			case 0b11111110: printf("not implemented\n"); break;
			case 0b11111111: printf("not implemented\n"); break;
		}

		//if execute flag set, print outs
		if (execute_flag)
		{
			printf(" ; ");
			// check if a register has change and print it if it has
			for (int i = 0; i < (sizeof(regs) / sizeof(regs[0])); i++)
			{
				if (regs[i] != regs_orig[i])
				{
					printf("%s:0x%x->0x%x",
							reg_names[i],
							regs_orig[i],
							regs[i]
							);
				}
			}

			// if a flag has changed print the change out
			for (int i = 0; i < (sizeof(flags) / sizeof(flags[0])); i++)
			{
				if (strcmp(flags[i], flags_orig[i]) != 0)
				{
					printf("flags:%s->%s",
							flags_orig[i],
							flags[i]
							);
				}
			}
		}

		// printf newlinw at the end of instruction
		printf("\n");
	}

	//if execute flag set, print out registers before exiting
	if (execute_flag)
	{
		printf("\nFinal registers:\n");
		print_registers(regs, (sizeof(regs) / sizeof(regs[0])));
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

	// get register string
	char* reg_str = get_reg_string(((byte&0b1)<<3) | ((byte2 >> 3) & 0b111));

	// get register/memory string
	char* regmem_str = get_reg_string(((byte&0b1)<<3) | (byte2 & 0b111));

	// get effective address string
	char* effaddr_str = get_effective_address(byte2 & 0b111);

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
				printf("%s, [%i]", reg_str, disp_val)
				: // if d bit is zero, reg is source
				printf("[%i], %s", disp_val, reg_str);
		}

		// normal case of mod00(reg is not 110). no dispacement.
		else
		{
			dbit ? // if d bit is one, reg is destination
				printf("%s, [%s]", reg_str, effaddr_str)
				: // if d bit is zero, reg is source
				printf("[%s], %s", effaddr_str, reg_str);
		}

	} // mod01. displacement one byte.
	if ((byte2 >> 6) == 0b01)
	{
		// read in 3rd byte for displacement
		int8_t byte3 = fgetc(asm_file);

		// check if D bit is 1
		dbit ? // if D is 1 reg is destination
			(byte3 != 0) ? // if byte3 is not zero add it into printf
						printf("%s, [%s %s %i]", reg_str, effaddr_str,
						// string for the sign
						byte3 < 0 ? "-" : "+",
						// displacement value(with remove sign hack)
						byte3 < 0 ? -byte3 : byte3)
						: // if byte3 is zero then don't add it into printf
						printf("%s, [%s]", reg_str, effaddr_str)
		: // if D is 0 reg is source
			(byte3 != 0) ? // if byte3 is not zero add it into printf
						printf("[%s %s %i], %s",
						// effective address
						effaddr_str,
						// string for the sign
						byte3 < 0 ? "-" : "+",
						// displacement value
						byte3 < 0 ? -byte3 : byte3,
						// register
						reg_str
						)
						: // if byte3 is zero then don't add it into printf
						printf("[%s], %s", effaddr_str, reg_str);
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
				? printf("%s, [%s + %i]", reg_str, effaddr_str, disp_val)
				// if byte3 and byte4 are 0, don't add their value into printf
				: printf("%s, [%s]", reg_str, effaddr_str)

			: // if D is 0 reg is source
			// if byte3 and byte4 are not 0, add there value into printf
			(byte3 != 0) && (byte4 !=0)
				? printf("[%s %s %i], %s", effaddr_str,
					// print displacemt sign string
					disp_val < 0 ? "-" : "+",
					// displacement value(displacement sign hack)
					disp_val < 0 ? -disp_val : disp_val,
					// register
					reg_str)
				// if byte3 and byte4 are 0, don't add their value into printf
				:
					// if byte3 is 0 then don't add it into printf
					printf("[%s], %s\n", effaddr_str, reg_str);
	}

	// mod11 register mode, no displacement
	if ((byte2 >> 6) == 0b11)
	{
		if (dbit) // D bit is one, reg is destination
		{
			printf("%s, %s", reg_str, regmem_str);
			// if execute flag set, execute the instruction
			if (execute_flag)
			{
				// get regmem value
				int regmem_value = get_reg_value(regmem_str);

				// get reg value
				int reg_value = get_reg_value(reg_str);

				if (strcmp(instruction, "mov") == 0)
				{
					// write regmem value into reg
					write_to_reg(reg_str, regmem_value);
				}
				if (strcmp(instruction, "add") == 0)
				{
					printf("add not implemented");
				}
				if (strcmp(instruction, "sub") == 0)
				{
					int new_value = reg_value - regmem_value;
					if (new_value >> 7)
					{
						*sign_flag = "S";
					}
					else
					{
						*sign_flag = " ";
					}
					// write regmem value into reg
					write_to_reg(reg_str, new_value);
				}
				if (strcmp(instruction, "cmp") == 0)
				{
					// compare reg and regmem by subtraction
					int new_value = reg_value - regmem_value;

					// sign flag gets checked for cmp
					if (new_value >> 7)
					{
						*sign_flag = "S";
					}
					else
					{
						*sign_flag = " ";
					}

					// if cmp if zero set flag
					if (new_value == 0)
					{
						*zero_flag = "Z";
					}
					else
					{
						*zero_flag = " ";
					}
				}
			}
		}
		else // D bit is zero, regmem is destination
		{
			printf("%s, %s", regmem_str, reg_str);
			// if execute flag set, execute the instruction
			if (execute_flag)
			{
				// get reg value
				int reg_value = get_reg_value(reg_str);

				// get regmem value
				int regmem_value = get_reg_value(regmem_str);

				if (strcmp(instruction, "mov") == 0)
				{
					// write reg value into regmem
					write_to_reg(regmem_str, reg_value);
				}
				if (strcmp(instruction, "add") == 0)
				{
					printf("add not implemented");
				}
				if (strcmp(instruction, "sub") == 0)
				{
					int new_value = regmem_value - reg_value;
					if (new_value >> 7)
					{
						*sign_flag = "S";
					}
					else
					{
						*sign_flag = " ";
					}
					write_to_reg(regmem_str, new_value);
				}
				if (strcmp(instruction, "cmp") == 0)
				{
					// compare reg and regmem by subtraction
					int new_value = regmem_value - reg_value;

					// sign flag gets checked for cmp
					if (new_value >> 7)
					{
						*sign_flag = "S";
					}
					else
					{
						*sign_flag = " ";
					}

					// if cmp if zero set flag
					if (new_value == 0)
					{
						*zero_flag = "Z";
					}
					else
					{
						*zero_flag = " ";
					}
				}
			}
		}
	}

}

void immediate_to_regmem(FILE* asm_file, int byte, char* instruction)
{
	// get second instruction byte
	int byte2 = fgetc(asm_file);
	int sbit = 0;

	// set instructions for add, sub, cmp
	if ((byte >> 2) == 0b100000)
	{
		switch ((byte2 >> 3) & 0b111)
		{
			case 0b000:
				instruction = "add";
				sbit = ((byte >> 1) & 0b1);
				break;
			case 0b101:
				instruction = "sub";
				sbit = ((byte >> 1) & 0b1);
				break;
			case 0b111:
				instruction = "cmp";
				sbit = ((byte >> 1) & 0b1);
				break;
		}
	}

	// check mod bits and call appropriate function
	switch (byte2 >> 6)
	{
		case 0b00:
			mod00(asm_file, byte, byte2, instruction, sbit);
			break;
		case 0b01:
			mod01(asm_file, byte, byte2, instruction, sbit);
			break;
		case 0b10:
			mod10(asm_file, byte, byte2, instruction, sbit);
			break;
		case 0b11:
			mod11(asm_file, byte, byte2, instruction, sbit);
			break;
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

	// execute if flag is set, modify registeres and print out instruction
	if (execute_flag == 1)
	{
		// write the new value to the register
		write_to_reg(reg_string, value);

		// print out asm instruction with register modifications
		printf(
			"%s %s, %i",
			instruction,
			// retrieve the correct register name
			reg_string,
			// value from the displacement byte/s
			value
			);
	}
	else
	{
	// print out asm instruction
	printf(
		"%s %s, %i",
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

void memory_with_accumulator(FILE* asm_file, int byte, char* instruction)
{
	int dbit = (byte >> 1) & 0b1;

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
		dbit ? 
			printf
				(
				"mov [%i], %s\n",
				// print memory address
				memory_address_value,
				// print the accumulator register
				"ax"
				)
			:
			printf
				(
				"mov %s, [%i]\n",
				// print the accumulator register
				"ax",
				// print memory address
				memory_address_value
				)
			;
	}
	// w bit is 0
	else
	{
		printf("mem to accu, w bit is 0");
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

// mod00, no displacement *except special case
void mod00(FILE* asm_file, int byte, int byte2, char* instruction, int sbit)
{
	// immediate to direct address. special case 110 is 16bit displacement
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

	// not special case of 110, no displacement
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

// mod01, memory mode, 8bit displacement follows
// TODO: finish this
void mod01(FILE* asm_file, int byte, int byte2, char* instruction, int sbit)
{
	// get displacement byte
	int byte3 = fgetc(asm_file);
	printf("mod01 - needs fixing, byte 3 is %02x", byte3);
}

// mod10, memory mode, 16bit displacement follows
void mod10(FILE* asm_file, int byte, int byte2, char* instruction, int sbit)
{
	// get displacement bytes
	int byte3 = fgetc(asm_file);
	int byte4 = fgetc(asm_file);

	// bitwise OR together displacement bytes to make displacement value
	int16_t displacement_value = ((byte4 << 8) | byte3);

	// w bit 1, wide mode
	if ((byte & 0b1) == 0b1)
	{
		int value = 0;

		// get data byte/s
		if (sbit)
		{
			int8_t byte5 = fgetc(asm_file);
			value = byte5;
		}
		else
		{
			int8_t byte5 = fgetc(asm_file);
			int8_t byte6 = fgetc(asm_file);
			value = (byte6<<8 | byte5);
		}

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
			value
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
void mod11(FILE* asm_file, int byte, int byte2, char* instruction, int sbit)
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

void print_registers(uint16_t* registers, size_t array_size)
{
	/*for (size_t i = 0; i < array_size; i++)
	{
		printf("      %s: 0x%04x (%i)\n",
				reg_names[i],
				regs[i],
				regs[i]
				);
	}*/
	printf("      %s: 0x%04x (%i)\n",
			"ax", get_reg_value("ax"), get_reg_value("ax"));
	printf("      %s: 0x%04x (%i)\n",
			"bx", get_reg_value("bx"), get_reg_value("bx"));
	printf("      %s: 0x%04x (%i)\n",
			"cx", get_reg_value("cx"), get_reg_value("cx"));
	printf("      %s: 0x%04x (%i)\n",
			"dx", get_reg_value("dx"), get_reg_value("dx"));
	printf("      %s: 0x%04x (%i)\n",
			"sp", get_reg_value("sp"), get_reg_value("sp"));
	printf("      %s: 0x%04x (%i)\n",
			"bp", get_reg_value("bp"), get_reg_value("bp"));
	printf("      %s: 0x%04x (%i)\n",
			"si", get_reg_value("si"), get_reg_value("si"));
	printf("      %s: 0x%04x (%i)\n",
			"di", get_reg_value("di"), get_reg_value("di"));
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



