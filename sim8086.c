#include <stdio.h>
#include <stdlib.h> // library with exit() and EXIT_FAILURE

int main(int argc, char **argv)
{
    // check if no arguments to main program are given
    if (argc == 1)
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
        perror("Error opening file"); // perror to print system error message
        exit(EXIT_FAILURE);
    }


    // close the file when done(to release the resources it was using)
    fclose(asm_file);

    // exit with success code
    return(0);
}
