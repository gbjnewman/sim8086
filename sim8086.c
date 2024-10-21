#include <stdio.h>
#include <string.h>
#include <stdlib.h>

// fread read amounts(how many times the bytes get read)
#define one_time 1
#define two_times 2

// byte amounts
#define one_byte 1
#define two_bytes 2
#define sixteen_bytes 16
#define fread_buffer_size 2000 //number of bytes allocated to the fread buffer

// yes no
#define yes 1
#define no 0

// array index name
#define index_zero 0
#define index_one 1
#define index_two 2

// mode types
#define mod00 0b00 // memory mode, no displacement follows*
#define mod01 0b01 // memory mode, 8bit displacement follows
#define mod10 0b10 // memory mode, 16bit displacement follows
#define mod11 0b11 // register mode, no displacement follows

// function prototypes
void decode_firstbyte(int firstbyte, char* opcode, int* d_bit, int* w_bit, int* reg_number, int* secondbyte_needed);
void decode_secondbyte(int secondbyte, int w_bit, int* mod, int* reg_number, int* regmem_number);
char* get_reg(int reg);
char* get_regmem(int mod, int regmem, int thirdbyte, int fourthbyte);
char* get_effective_address(int regmem);
int get_displacement(int firstbyte, int mod, int regmem_number);


int main(int argc, char **argv)
{
    // test for cli arguments, exit if user has provided too many, or none
    if (argc < 2)
    {
      printf ("no file to decode has been provided\n");
      exit(EXIT_FAILURE);
    }
    if (argc > 2)
    {
      printf ("too many arguments provided, please provide only one file to decode\n");
      exit(EXIT_FAILURE);
    }

    //setup fread buffer
    char fread_buffer[fread_buffer_size] = {0};

    // open file into file pointer
    FILE* file_pointer = fopen(argv[1], "r");

    // exit if file_pointer is empty 
    if (file_pointer == NULL)
    {
        printf("file NULL, could not open file");
        exit(EXIT_FAILURE);
    }

    // start read loop
    int read_loop = fread(fread_buffer, one_byte, one_time, file_pointer);
    while (read_loop > 0)  // while there are still bytes to read 
    {
        unsigned char firstbyte = fread_buffer[index_zero];
        unsigned char secondbyte;
        unsigned char thirdbyte;
        unsigned char fourthbyte;

        char opcode[sixteen_bytes];
        char reg[sixteen_bytes];
        char regmem[sixteen_bytes];

        int d_bit;
        int w_bit;
        int secondbyte_needed;
        int mod;
        int reg_number;
        int regmem_number;
        int displacement;

        // decode firstbyte
        decode_firstbyte(firstbyte, opcode, &d_bit, &w_bit, &reg_number, &secondbyte_needed);

        // if needed, decode secondbyte
        if (secondbyte_needed == yes)
        {
          fread(fread_buffer, one_byte, secondbyte_needed, file_pointer);
          secondbyte = fread_buffer[index_zero];
          decode_secondbyte(secondbyte, w_bit, &mod, &reg_number, &regmem_number);
        }

        //displacement
        displacement = get_displacement(firstbyte, mod, reg_number);
        //if needed, get displacement freads and put into byte variables
        if (displacement > 0)
        {
          switch (displacement)
          {
            case 1:
              fread(fread_buffer, one_byte, one_time, file_pointer);
              thirdbyte = fread_buffer[index_zero];
              break;
            case 2:
              fread(fread_buffer, one_byte, two_times, file_pointer);
              thirdbyte = fread_buffer[index_zero];
              fourthbyte = fread_buffer[index_two];
              break;
          }
        }


        //set registers
        strcpy(reg, get_reg(reg_number));
        strcpy(regmem, get_regmem(mod, regmem_number, thirdbyte, fourthbyte));

        //print out instruction
        if (d_bit == 1)
        {
          printf("%s %s, %s\n", opcode, reg, regmem);
        } else {
          printf("%s %s, %s\n", opcode, regmem, reg);
        }


        //read loop for next while loop iteration
        read_loop = fread(fread_buffer, one_byte, one_time, file_pointer);
    }
}
/*end of main*/





/* FUNCTIONS */

//decode firstbyte
void decode_firstbyte(int firstbyte, char* opcode, int* d_bit, int* w_bit, int* reg_number, int* secondbyte_needed)
{
    if ((firstbyte >> 4) == 0b1011) // mov, immediate to reg
    {
      printf("print me daddy,opcode is 1011");
      strcpy(opcode, "mov");
      *w_bit = (firstbyte >> 3) & 0b1;
      *d_bit = 1;
      *reg_number = firstbyte & 0b111;
      *secondbyte_needed = no;
    }
    if ((firstbyte >> 2) == 0b100010) // mov, reg/mem to/from reg/mem
    {
      strcpy(opcode, "mov");
      *d_bit = (firstbyte >> 1) & 0b1;
      *w_bit = firstbyte & 0b1;
      *secondbyte_needed = yes;
    }

}

//decode secondbyte
void decode_secondbyte(int secondbyte, int w_bit, int* mod, int* reg_number, int* regmem_number)
{
  *mod = (secondbyte >> 6) & 0b11;
  *reg_number = w_bit << 3 & 0b1111 | (secondbyte >> 3) & 0b111;
  *regmem_number = w_bit << 3 | secondbyte & 0b111;

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
char* get_regmem(int mod, int regmem_number, int thirdbyte, int fourthbyte)
{
  switch (mod)
  {
    case 0b00:
    case 0b01:
    case 0b10:
    case 0b11:
      return get_reg(regmem_number);


  }

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
}

int get_displacement(int firstbyte, int mod, int regmem_number)
{
  if ((firstbyte >> 4) == 0b1011)
  {
    return ((firstbyte >> 3) & 0b1) == 0b1 ? 2 : 1;//if w bit is one, displacement is 2bytes, else 1byte
  }
  int displacement;
  switch (mod)
  {
    case mod00: // memory mode, no displacement follows*
      if (regmem_number == 0b110) // specal case
      {
        displacement = 2;
      } else {
        displacement = 0;
      }
      break;
    case mod01: // memory mode, 8bit displacement follows*
      displacement = 1;
      break;
    case mod10: // memory mode, 16bit displacement follows*
      displacement = 2;
      break;
    case mod11: // register mode, no displacement follows*
      displacement = 0;
      break;
  }
  return displacement;
}



