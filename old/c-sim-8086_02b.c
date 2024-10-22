#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define one_time 1
#define one_byte 1
#define two_bytes 2
#define twenty_bytes 20 
#define two_thousand_bytes 2000
#define index_zero 0
#define index_one 1
#define mov_regmem_tofrom_regmem 0b100010 // reg/mem to/from reg/mem mov
#define mov_immediate_to_reg 0b1011 // immediate to reg mov
#define mode00 0b00 // memory mode, no displacement follows*
#define mode01 0b01 // memory mode, 8bit displacement follows
#define mode10 0b10 // memory mode, 16bit displacement follows
#define mode11 0b11 // register mode, no displacement follows
#define d_bit_zero 0b0
#define d_bit_one 0b1
#define w_bit_zero 0b0
#define w_bit_one 0b1

int main(void)
{
    char fread_buffer[two_thousand_bytes] = {0};

    char *register_name[] = {   "al", 
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
                                "di"};

    char *effective_address_calculation[] = {   "bx + si", 
                                                "bx + di", 
                                                "bp + si", 
                                                "bp + di", 
                                                "si", 
                                                "di", 
                                                "bp", 
                                                "bx"};

    FILE* file_pointer = fopen("listing_0039_more_movs", "r"); // open ASM file

    if (file_pointer == NULL) // exit if file_pointer is empty 
    {
        printf("file NULL, could not open file");
        exit(EXIT_FAILURE);
    }

    int read_loop = fread(fread_buffer, one_byte, one_time, file_pointer);
    while (read_loop > 0)  // while there are still bytes to read 
    {
        unsigned char firstbyte = fread_buffer[index_zero];
        unsigned char secondbyte;
        unsigned char thirdbyte;
        unsigned char fourthbyte;
        char data_transfer_type[twenty_bytes] = "";
        char reg[twenty_bytes] = "";
        char regmem[twenty_bytes] = "";
        char source[twenty_bytes] = "";
        char destination[twenty_bytes] = "";


        if ((firstbyte >> 2) == mov_regmem_tofrom_regmem)  // reg/mem to/from reg/mem mov
        {
            fread(fread_buffer, one_byte, one_time, file_pointer);
            secondbyte = fread_buffer[index_zero];
            strcat(data_transfer_type, "mov");

            if (((secondbyte >> 6) & 0b11) == mode00) // mod set to 00
            {
                if (secondbyte & 0b111 == 0b110) //*special case, 16bit displacements follows
                {
                    printf("mod 00 special case\n");
                    fread(fread_buffer, two_bytes, one_time, file_pointer);
                    thirdbyte = fread_buffer[index_zero];
                    fourthbyte = fread_buffer[index_one];

                }
                strcat(reg, register_name[((firstbyte & 0b1) << 3) | ((secondbyte >> 3) & 0b111)]);
                strcat(regmem, "[");
                strcat(regmem, effective_address_calculation[secondbyte & 0b111]);
                strcat(regmem, "]");

            }
            if (((secondbyte >> 6) & 0b11) == mode01) // mod set to 01
            {
            fread(fread_buffer, one_byte, one_time, file_pointer);
            thirdbyte = fread_buffer[index_zero];
                strcat(reg, register_name[((firstbyte & 0b1) << 3) | ((secondbyte >> 3) & 0b111)]);
                strcat(regmem, "[");
                strcat(regmem, effective_address_calculation[secondbyte & 0b111]);
                if (thirdbyte > 0)
                    {
                        char regmem_plus_component[twenty_bytes] = "";
                        snprintf(regmem_plus_component, twenty_bytes,  " + %d", thirdbyte);
                        strcat(regmem, regmem_plus_component);
                    }
                strcat(regmem, "]");
            }
            if (((secondbyte >> 6) & 0b11) == mode10) // mod set to 10
            {
                fread(fread_buffer, two_bytes, one_time, file_pointer);
                thirdbyte = fread_buffer[index_zero];
                fourthbyte = fread_buffer[index_one];
                strcat(reg, register_name[((firstbyte & 0b1) << 3) | ((secondbyte >> 3) & 0b111)]);
                strcat(regmem, "[");
                strcat(regmem, effective_address_calculation[secondbyte & 0b111]);
                if ((thirdbyte > 0) || (fourthbyte > 0))
                    {
                        char regmem_plus_component[twenty_bytes] = "";
                        snprintf(   
                                regmem_plus_component, twenty_bytes,  
                                " + %d", ((fourthbyte << 8) | thirdbyte)
                                );
                        strcat(regmem, regmem_plus_component);
                    }
                strcat(regmem, "]");
            }
            if (((secondbyte >> 6) & 0b11) == mode11) // mod set to 11
            {
                strcat(reg, register_name[((firstbyte & 0b1) << 3 | (secondbyte >> 3) & 0b111)]);
                strcat(regmem, register_name[((firstbyte & 0b1) << 3 | secondbyte & 0b111)]);

            }

            /* d bit check */
            if (((firstbyte >> 1) & 0b1) == d_bit_zero) // if d bit zero, reg is source
            {
                strcat(source, reg);
                strcat(destination, regmem);
            }
            if (((firstbyte >> 1) & 0b1) == d_bit_one) // if d bit one, reg is destination
            {
                strcat(source, regmem);
                strcat(destination, reg);
            }

        }

        if ((firstbyte >> 4) == mov_immediate_to_reg) // immediate to register mov
        {
            fread(fread_buffer, one_byte, one_time, file_pointer);
            secondbyte = fread_buffer[index_zero];
            strcat(data_transfer_type, "mov");
            strcat(reg, register_name[(((firstbyte >> 3) & 0b1) << 3 | firstbyte & 0b111)]);

            if (((firstbyte >> 3) & 0b1) == w_bit_zero) // w bit set to zero, not wide mode (8 bit)
            { 
                snprintf(source, twenty_bytes, "%d", secondbyte);
            }
            if (((firstbyte >> 3) & 0b1) == w_bit_one) // w bit set to one, wide mode (16 bit)
            {
                fread(fread_buffer, one_byte, one_time, file_pointer);
                thirdbyte = fread_buffer[index_zero];
                snprintf(source, twenty_bytes, "%d", ( thirdbyte << 8 | secondbyte ));
            }

            strcat(destination, reg);
        }

        printf("%s %s, %s\n", data_transfer_type, destination, source);
        read_loop = fread(fread_buffer, one_byte, one_time, file_pointer);
    }
}