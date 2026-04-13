#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/mman.h>

const int SIZE=1024;  // bytes

struct firework {
    int shell_caliber;
    float lift_charge;
    float burst_charge;
    int fuse_delay_ms;
};

int main(void)
{
    // Allocate some space from the OS
    void *heap = mmap(NULL, SIZE, PROT_READ|PROT_WRITE,
                      MAP_ANON|MAP_PRIVATE, -1, 0);

    // Challenge: Store your favorite letter at the byte `heap` points
    // to, then print it with `%c` and also with `%d`.
    *(char *)heap = 'A';
    printf("Challenge #1: char = %c, ASCII = %d\n", *(char *)heap, (int)*(char *)heap);

    // Challenge: Store a string at the byte `heap` points to, then
    // print it.
    strcpy((char *)heap, "Hello, World!");
    printf("Challenge #2: %s\n", (char *)heap);

    // Challenge: Store an integer at the byte `heap` points to, then
    // print it.
    *(int *)heap = 20;
    printf("Challenge #3: %d\n", *(int *)heap);

    // Challenge: Store a float right *after* the int you just stored.
    // Print them both.
    *(float *)(heap+sizeof(int)) = 7.2;
    printf("Challenge #4: int = %d, float = %f\n", *(int *)heap, *(float *)(heap+sizeof(int)));

    // Challenge: Store a short (a smallish integer) at byte offset 512
    // in the `heap` area. Then print it with `%d`, along with the int
    // and float you already stored.
    *(short *)(heap+512) = 15;
    printf("Challenge #5: int1 = %d, float = %f, short = %d\n", *(int *)heap, *(float *)(heap+sizeof(int)), *(int *)(heap+512));

    // Challenge: Store a struct firework at byte offset 48 in the
    // `heap` area. Make a `struct firework *` initialized to the right
    // position, then fill it with data (with the `->` operator). Then
    // print out the data, and also print out the int, float, and short
    // from above.
    ((struct firework *)(heap+48))->shell_caliber = 5;
    ((struct firework *)(heap+48))->lift_charge = 3.5;
    ((struct firework *)(heap+48))->burst_charge = 4.5;
    ((struct firework *)(heap+48))->fuse_delay_ms = 1000;

    printf("Challenge #6: firework: caliber = %d, lift_charge = %f, burst_charge = %f, fuse_delay_ms = %d\n",
           ((struct firework *)(heap+48))->shell_caliber,
           ((struct firework *)(heap+48))->lift_charge,
           ((struct firework *)(heap+48))->burst_charge,
           ((struct firework *)(heap+48))->fuse_delay_ms);

    // Challenge: Store a second struct firework directly after the
    // first one. Initialize and print.
    ((struct firework *)(heap+48+sizeof(struct firework)))->shell_caliber = 10;
    ((struct firework *)(heap+48+sizeof(struct firework)))->lift_charge = 7.0;
    ((struct firework *)(heap+48+sizeof(struct firework)))->burst_charge = 9.0;
    ((struct firework *)(heap+48+sizeof(struct firework)))->fuse_delay_ms = 2000;

    printf("Challenge #7: firework: caliber = %d, lift_charge = %f, burst_charge = %f, fuse_delay_ms = %d\n",
           ((struct firework *)(heap+48+sizeof(struct firework)))->shell_caliber,
           ((struct firework *)(heap+48+sizeof(struct firework)))->lift_charge,
           ((struct firework *)(heap+48+sizeof(struct firework)))->burst_charge,
           ((struct firework *)(heap+48+sizeof(struct firework)))->fuse_delay_ms);
}