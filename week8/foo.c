#include <stdio.h>

struct foo { // 8 bytes somehow?
    char a; // 1 byte
    // compiler puts 3 padding bytes here to align the int
    int b; // 4 bytes
};

int main(void) {
    printf("%zu\n", sizeof(char));
    printf("%zu\n", sizeof(struct foo));
}