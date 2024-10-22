#include <stdio.h>
#include <string.h>
#include <stdlib.h>


char buffer[2000] = {0};
char *w_one_reg_returns[] = {"ax", "cx", "dx", "bx", "sp", "bp", "si", "di"};
char *w_zero_reg_returns[] = {"al", "cl", "dl", "bl", "ah", "ch", "dh", "bh"};
char *eff_addr_calc[] = {"[bx + si]", "[bx + di]", "[bp + si]", "[bp + di]", "[si]", "[di]", "[bp]", "[bx]"};

int main(void)
{
    FILE* open_file_holding_variable = fopen("listing_0039_more_movs", "r");
    if (open_file_holding_variable == NULL) 
    {
        printf("file NULL");
        exit(EXIT_FAILURE);
    }

    int read_loop = fread(buffer, 1, 1, open_file_holding_variable);
    while (read_loop > 0)
    {
        unsigned char firstbyte = buffer[0];
        char data_transfer_type[7] = "";
        char reg[7] = "";
        char regmem[7] = "";
        char source[7] = "";
        char destination[7] = "";


        if ((firstbyte >> 2) == 0b100010)  // reg/mem to/from reg/mem mov
        {
            fread(buffer, 1, 1, open_file_holding_variable);
            unsigned char secondbyte = buffer[0];
            strcat(data_transfer_type, "mov");
            // printf("        first byte = %d\n", firstbyte);
            // printf("        second byte = %d\n", secondbyte);

            if ((secondbyte >> 6 & 0b11) == 0b00) //mode = 00
            {
                strcat(regmem, eff_addr_calc[(secondbyte & 0b111)]);
                if ((firstbyte & 0b1) == 1) // checking W bit
                {
                    strcat(destination, w_one_reg_returns[(secondbyte >> 3) & 0b111]);
                } else {
                    strcat(destination, w_zero_reg_returns[(secondbyte >> 3) & 0b111]);
                }
                strcat(source, regmem);

            }

            if ((secondbyte >> 6 & 0b11) == 0b01) //mode = 01
            {
                printf("mod 01");
                fread(buffer, 1, 1, open_file_holding_variable);
                strcat(regmem, eff_addr_calc[(secondbyte & 0b111)]);

                if ((firstbyte & 0b1) == 1) // checking W bit
                {
                    strcat(destination, w_one_reg_returns[(secondbyte >> 3) & 0b111]);
                } else {
                    strcat(destination, w_zero_reg_returns[(secondbyte >> 3) & 0b111]);
                }
                strcat(source, regmem);


            }

            if ((secondbyte >> 6 & 0b11) == 0b10) //mode = 10
            {
                printf("mod 10");
                fread(buffer, 1, 1, open_file_holding_variable);
                fread(buffer, 1, 1, open_file_holding_variable);
                strcat(regmem, eff_addr_calc[(secondbyte & 0b111)]);

                if ((firstbyte & 0b1) == 1) // checking W bit
                {
                    strcat(destination, w_one_reg_returns[(secondbyte >> 3) & 0b111]);
                } else {
                    strcat(destination, w_zero_reg_returns[(secondbyte >> 3) & 0b111]);
                }
                strcat(source, regmem);

            }

            if ((secondbyte >> 6 & 0b11) == 0b11) //mode = 11(register mode(no displacement))
            {
                if ((firstbyte & 0b1) == 1) // checking W bit
                {
                    // wide mode(16bit)
                    strcat(reg, w_one_reg_returns[(secondbyte >> 3 & 0b111)]);
                    strcat(regmem, w_one_reg_returns[(secondbyte & 0b111)]);
                } else {
                    // not wide mode(8 bit)
                    strcat(reg, w_zero_reg_returns[(secondbyte >> 3 & 0b111)]);
                    strcat(regmem, w_zero_reg_returns[(secondbyte & 0b111)]);
                }



                if ((firstbyte >> 1 & 0b1) == 0b1) // checking D bit
                {
                    // D bit set to one, reg field is destination
                    strcat(destination, reg);
                    strcat(source, regmem);
                } else {
                    // D bit set to zero, reg field is source
                    strcat(source, reg);
                    strcat(destination, regmem);
                }
            }


        }

        if ((firstbyte >> 4) == 0b1011) // immediate to register mov
        {
            fread(buffer, 1, 1, open_file_holding_variable);
            unsigned char secondbyte = buffer[0];
            strcat(data_transfer_type, "mov");

            if ((firstbyte >> 3 & 0b1) == 0b1) // w bit set to one, wide mode (16 bit)
            {
                fread(buffer, 1, 1, open_file_holding_variable);
                unsigned char thirdbyte = buffer[0];
                strcat(reg, w_one_reg_returns[(firstbyte & 0b111)]);
                snprintf(source, 7, "%d", ( thirdbyte << 8 | secondbyte ));
                strcat(destination, reg);

            }
            if ((firstbyte >> 3 & 0b1) == 0b0) { // w bit set to zero, not wide mode (8 bit)
                strcat(reg, w_zero_reg_returns[(firstbyte & 0b111)]);
                snprintf(source, 7, "%d", secondbyte);
                strcat(destination, reg);

            }
        }
        printf("%s %s, %s\n", data_transfer_type, destination, source);
        read_loop = fread(buffer, 1, 1, open_file_holding_variable);


    }
}