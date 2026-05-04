#include <stdio.h>
#include "adder.h"

int main() {
    int a = 5;
    int b = 10;
    int result = adder(a, b);
    printf("The sum of %d and %d is %d\n", a, b, result);
    return 0;
}