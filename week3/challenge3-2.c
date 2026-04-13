#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/mman.h>

const int SIZE=1024;  // bytes

struct bicycle {
    int wheel_count;
    char name[32];
};

int main(void)
{
    // Allocate some space from the OS
    void *heap = mmap(NULL, SIZE, PROT_READ|PROT_WRITE,
                  MAP_ANON|MAP_PRIVATE, -1, 0);

    // Challenge 1:
    //
    // Treat heap as an array of chars
    // Store a string there with strcpy() and print it

    // Challenge 2:
    //
    // Treat heap as an array of ints
    // Use a loop to store the values 0, 10, 20, 30, 40 in it
    // Use a loop to retrieve the value and print them

    // Challenge 3:
    //
    // Treat heap as an array of struct bicycles
    // Store 3 bicycles
    // Print out the bicycle data in a loop

    // Challenge 4:
    //
    // If you used pointer notation, redo challenges 1-3 with array
    // notation, and vice versa.

    // Challenge 5:
    //
    // Make the first 32 bytes of the heap a string (array of chars),
    // and follow that directly with an array of ints.
    //
    // Store a short string in the string area of the heap
    // Use a loop to store the values 0, 10, 20, 30, 40 in the int array
    // Use a loop to retrieve the value and print them
    // Print the string

    // Challenge 6:
    //
    // Make an array of interleaved `struct bicycle` and `int` data
    // types.
    //
    // The first entry in the heap will be a `struct bicycle`, then
    // right after that, an `int`, then right after that a `struct
    // bicycle`, then an `int`, etc.
    //
    // Store as many of these as can fit in SIZE bytes. Loop through,
    // filling them up with programmatically-generated data. Then loop
    // through again and print out the elements.

    // Free it up
    munmap(heap, SIZE);
}
