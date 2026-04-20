#include <stdio.h>
#include <stdlib.h>

int global_var = 70;

// Output:
// Stack address:  0x7fff785c54b4
// Heap address:   0x3e3a26b0
// Global address: 0x40301c

int main(void)
{
    // Stack variable
    int stack_var = 10;
    printf("Stack address:  %p\n", (void *)&stack_var);

    // Heap variable
    int *heap_var = malloc(sizeof(int));
    *heap_var = 20;
    printf("Heap address:   %p\n", (void *)heap_var);

    // Global variable
    printf("Global address: %p\n", (void *)&global_var);

    free(heap_var);
}
