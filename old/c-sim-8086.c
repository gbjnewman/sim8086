#include <stdio.h>
#include <string.h>
#include <stdlib.h>


char buffer[2000] = {0};
char *w_one_reg_returns[] = {"ax", "cx", "dx", "bx", "sp", "bp", "si", "di"};
char *w_zero_reg_returns[] = {"al", "cl", "dl", "bl", "ah", "ch", "dh", "bh"};

int main(void)
{
    FILE* open_file_holding_variable = fopen("listing_0038_many_register_mov", "r");
    if (open_file_holding_variable == NULL) 
    {
        printf("file NULL");
        exit(EXIT_FAILURE);
    }

    int read_loop = fread(buffer, 2, 1, open_file_holding_variable);
    while (read_loop > 0)
    {
        char reg[3] = "";
        char regmem[3] = "";
        unsigned char firstbyte = buffer[0];
        unsigned char secondbyte = buffer[1];


        if (firstbyte >> 2 == 0b100010)  // reg to reg mov mode
        {
            // printf("mov mode\n");

            if ((secondbyte >> 6 & 0b11) == 0b11)
            {
                // printf("Mod 11, register to register mov mode\n");
                //regmem = get the regmem add
                if ((firstbyte & 0b1) == 1)
                {
                    strcat(reg, w_one_reg_returns[(secondbyte >> 3 & 0b111)]);
                    strcat(regmem, w_one_reg_returns[(secondbyte & 0b111)]);
                } else {
                    strcat(reg, w_zero_reg_returns[(secondbyte >> 3 & 0b111)]);
                    strcat(regmem, w_zero_reg_returns[(secondbyte & 0b111)]);
                }



                if ((firstbyte >> 1 & 0b1) == 0b1)
                {
                    //printf("D bit set to one, reg field is destination\n");
                    printf("mov %s, %s\n", reg, regmem);
                } else {
                    //printf("D bit set to zero, reg field is source\n");
                    printf("mov %s, %s\n", regmem, reg);
                }
            }

        }
        // printf("tester = %s\n", w_one_reg_returns[(secondbyte & 0b111)]);
        // printf("reg = %s\n", reg);
        // printf("regmem = %s\n", regmem);

    read_loop = fread(buffer, 2, 1, open_file_holding_variable);
    }
}