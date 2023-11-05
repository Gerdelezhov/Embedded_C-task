#include <stdio.h>

int main() {
    int number;

    printf("Введите целое отрицательное число: ");
    if (scanf("%d", &number) != 1 || number >= 0) {
        printf("Ошибка: Число должно быть целым отрицательным!\n");
        return 1;
    }

    int len = sizeof(number) * 8;

    printf("Двоичное представление чисала %d: ", number);
    while (len > 0) {
        printf("%d", (number >> (len - 1)) & 1);
        len--;
    }

    printf("\n");

    return 0;
}