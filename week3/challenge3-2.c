#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/mman.h>

const int SIZE = 1024; // bytes

struct bicycle
{
    int wheel_count;
    char name[32];
};

int main(void)
{
    // Allocate some space from the OS
    void *heap = mmap(NULL, SIZE, PROT_READ | PROT_WRITE,
                      MAP_ANON | MAP_PRIVATE, -1, 0);

    // Challenge 1:
    //
    // Treat heap as an array of chars
    // Store a string there with strcpy() and print it
    strcpy((char *)heap, "Hello, World!");
    printf("Challenge #1: %s\n", (char *)heap);
    // Challenge 2:
    //
    // Treat heap as an array of ints
    // Use a loop to store the values 0, 10, 20, 30, 40 in it
    // Use a loop to retrieve the value and print them
    printf("Challenge #2: ");
    for (int i = 0; i < 5; i++)
    {
        *(int *)(heap + (i * sizeof(int))) = (i * 10);
    }
    for (int i = 0; i < 5; i++)
    {
        if (i == 4)
        {
            printf("%d.\n", *(int *)(heap + (i * sizeof(int))));
        }
        else
        {
            printf("%d,", *(int *)(heap + (i * sizeof(int))));
        }
    }

    // Challenge 3:
    //
    // Treat heap as an array of struct bicycles
    // Store 3 bicycles
    // Print out the bicycle data in a loop
    struct bicycle *bikes = (struct bicycle *)heap;
    const char *names[] = {"Silly Slacker", "Road Rage Randy", "Hardtail Heresy"};

    for (int i = 0; i < 3; i++)
    {
        bikes[i].wheel_count = 2;
        strcpy(bikes[i].name, names[i]);
    }

    printf("Challenge #3:\n");
    for (int i = 0; i < 3; i++)
    {
        printf("Bike %d: wheel_count = %d, name = %s\n",
               i + 1, bikes[i].wheel_count, bikes[i].name);
    }

    // Challenge 4:
    //
    // If you used pointer notation, redo challenges 1-3 with array
    // notation, and vice versa.

    // Challenge 1 redo (array notation — same as pointer notation for strcpy/printf):
    strcpy((char *)heap, "Hello, World!");
    printf("Challenge #4 (1): %s\n", (char *)heap);

    // Challenge 2 redo (array notation):
    printf("Challenge #4 (2): ");
    for (int i = 0; i < 5; i++)
    {
        ((int *)heap)[i] = (i * 10);
    }
    for (int i = 0; i < 5; i++)
    {
        if (i == 4)
        {
            printf("%d.\n", ((int *)heap)[i]);
        }
        else
        {
            printf("%d,", ((int *)heap)[i]);
        }
    }

    // Challenge 3 redo (pointer notation):
    struct bicycle *bikes4 = (struct bicycle *)heap;

    for (int i = 0; i < 3; i++)
    {
        (bikes4 + i)->wheel_count = 2;
        strcpy((bikes4 + i)->name, names[i]);
    }

    printf("Challenge #4 (3):\n");
    for (int i = 0; i < 3; i++)
    {
        printf("Bike %d: wheel_count = %d, name = %s\n",
               i + 1, (bikes4 + i)->wheel_count, (bikes4 + i)->name);
    }

    // Challenge 5:
    //
    // Make the first 32 bytes of the heap a string (array of chars),
    // and follow that directly with an array of ints.
    //
    // Store a short string in the string area of the heap
    // Use a loop to store the values 0, 10, 20, 30, 40 in the int array
    // Use a loop to retrieve the value and print them
    // Print the string

    for (int i = 0; i < 32; i++)
    {
        ((char *)heap)[i] = 'A' + i;
    }

    int *int_heap = (int *)(heap + 32);
    strcpy((char *)heap, "Hello heap!");
    printf("Challenge #5:\n");

    for (int i = 0; i < 5; i++)
    {
        int_heap[i] = (i * 10);
    }
    for (int i = 0; i < 5; i++)
    {
        if (i == 4)
        {
            printf("%d.\n", int_heap[i]);
        }
        else
        {
            printf("%d,", int_heap[i]);
        }
    }
    printf("String: %s\n", (char *)heap);

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

    int struct_size = sizeof(struct bicycle) + sizeof(int);
    int num_entries = SIZE / struct_size;
    printf("Challenge #6: num_entries = %d\n", num_entries);

    for (int i = 0; i < num_entries; i++)
    {
        struct bicycle *bike = (struct bicycle *)(heap + (i * struct_size));
        int *num = (int *)(heap + (i * struct_size) + sizeof(struct bicycle));

        bike->wheel_count = 2;
        sprintf(bike->name, "Bike %d", i + 1);
        *num = i + 1;
    }

    for (int i = 0; i < num_entries; i++)
    {
        struct bicycle *bike = (struct bicycle *)(heap + (i * struct_size));
        int *num = (int *)(heap + (i * struct_size) + sizeof(struct bicycle));

        printf("Entry %d: wheel_count = %d, name = %s, num = %d\n",
               i + 1, bike->wheel_count, bike->name, *num);
    }

    // Free it up
    munmap(heap, SIZE);
}
