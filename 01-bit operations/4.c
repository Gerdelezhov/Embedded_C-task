#include <stdio.h>

int main() {
    int number, bit_to_set;

    printf("Введите целое положительное число: ");
    if (scanf("%d", &number) != 1 || number <= 0) {
        printf("Ошибка: Число должно быть целым положительным!\n");
        return 1;
    }

    printf("Введите бит (0 или 1), на который вы хотите заменить третий бит: ");
    if (scanf("%d", &bit_to_set) != 1 || (bit_to_set != 0 && bit_to_set != 1)) {
        printf("Ошибка: Бит должен быть 0 или 1.\n");
        return 1;
    }

    number &= ~(1 << 2);
    number |= (bit_to_set << 2);

    printf("Результат: %d\n", number);

    return 0;
}
