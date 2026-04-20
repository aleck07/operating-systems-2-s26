#include <stdio.h>
#include <stdlib.h>

// Output
// Stack address (depth 5): 0x7ffcad3350cc
// Stack address (depth 4): 0x7ffcad33509c
// Stack address (depth 3): 0x7ffcad33506c
// Stack address (depth 2): 0x7ffcad33503c
// Stack address (depth 1): 0x7ffcad33500c

// Heap address (alloc 1): 0x1977b6b0
// Heap address (alloc 2): 0x1977b6d0
// Heap address (alloc 3): 0x1977b6f0
// Heap address (alloc 4): 0x1977b710
// Heap address (alloc 5): 0x1977b730

void grow_stack(int depth)
{
    if (depth == 0)
        return;

    int local = depth;
    printf("Stack address (depth %d): %p\n", depth, (void *)&local);
    grow_stack(depth - 1);
}

int main(void)
{
    grow_stack(5);

    printf("\n");

    for (int i = 1; i <= 5; i++)
    {
        int *heap = malloc(sizeof(int));
        printf("Heap address (alloc %d): %p\n", i, (void *)heap);
    }
}
