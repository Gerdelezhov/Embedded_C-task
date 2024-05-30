#include <stdio.h>

#include "libcalc.h"

int main() {
    printf("a = 8, b = 2");
    printf("\na + b = %d", add(8, 2));
    printf("\na - b = %d", sub(8, 2));
    printf("\na * b = %d", mul(8, 2));
    printf("\na / b = %d\n", div(8, 2));

    return 0;
}