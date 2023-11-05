#include <stdio.h>

int main() {
    int number;

    printf("Введите целое положительное число: ");
    if (scanf("%d", &number) != 1 || number <= 0) {
        printf("Ошибка: Число должно быть целым положительным!\n");
        return 1;
    }

    int len = sizeof(number) * 8;

    int bit_sum = 0, bit;

    printf("Двоичное представление чисала %d: ", number);
    while (len > 0) {
        bit = (number >> (len - 1)) & 1;

        if ((bit_sum += bit) != 0) {
            printf("%d", bit);
        }
        len--;
    }

    printf("\n");

    return 0;
}