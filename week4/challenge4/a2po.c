#include <stdio.h>
#include <stdlib.h>


int main(int argc, char* argv[])
{
    if (argc != 2)
    {
        fprintf(stderr, "usage: %s num\n", argv[0]);
        return 1;
    }

    int num = atoi(argv[1]);
    int page = num >> 8;
    int offset = num & 0b0011111111;

    printf("Address %d = Page %d, offset %d\n", num, page, offset);

}