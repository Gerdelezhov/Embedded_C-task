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
    int count = 0;

    while (len > 0) {
        bit = (number >> (len - 1)) & 1;
        bit_sum += bit;
        count++;

        len--;
    }

    printf("Количество единиц в двоичном представлении числа: %d\n", bit_sum);

    return 0;
}