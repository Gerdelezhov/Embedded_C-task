#include <stdio.h>

int main() {
    int number, byte_to_set;

    printf("Введите целое положительное число: ");
    if (scanf("%d", &number) != 1 || number <= 0) {
        printf("Ошибка: Число должно быть целым положительным!\n");
        return 1;
    }

    printf("Введите число для замены третьего байта: ");
    scanf("%d", &byte_to_set);

    unsigned char *bytePtr = (unsigned char *)&number + 2;
    *bytePtr = (unsigned char)byte_to_set;

    printf("Измененное число: %d\n", number);

    return 0;
}
