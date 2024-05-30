#include <stdio.h>

#include "libcalc.h"

int main() {
    int a, b;
    printf("\e[2J\e[H");

    while (1) {
        printf(
            "\n1 - Сложение\n2 - Вычитание\n3 - Умножение\n4 - Деление\n5 - "
            "Выйти\n");
        int choice;
        scanf("%d", &choice);
        getchar();  // Удаление символа новой строки, оставленного scanf
        printf("\e[2J\e[H");

        switch (choice) {
            case 1: {
                printf("Ведите целое число 'a', где ans = a + b: ");
                scanf("%d", &a);
                getchar();
                printf("Ведите целое число 'b', где ans = a + b: ");
                scanf("%d", &b);
                getchar();

                printf("%d + %d = %d\n", a, b, add(a, b));
                break;
            }
            case 2: {
                printf("Ведите целое число 'a', где ans = a - b: ");
                scanf("%d", &a);
                getchar();
                printf("Ведите целое число 'b', где ans = a - b: ");
                scanf("%d", &b);
                getchar();

                printf("%d - %d = %d\n", a, b, sub(a, b));
                break;
            }
            case 3: {
                printf("Ведите целое число 'a', где ans = a * b: ");
                scanf("%d", &a);
                getchar();
                printf("Ведите целое число 'b', где ans = a * b: ");
                scanf("%d", &b);
                getchar();

                printf("%d * %d = %d\n", a, b, mul(a, b));
                break;
            }
            case 4: {
                printf("Ведите целое число 'a', где ans = a / b: ");
                scanf("%d", &a);
                getchar();
                printf("Ведите целое число 'b', где ans = a / b: ");
                scanf("%d", &b);
                getchar();

                printf("%d / %d = %d\n", a, b, div(a, b));
                break;
            }
            case 5:
                return 0;
            default:
                printf("Неверный выбор. Пожалуйста, попробуйте снова.\n");
        }
    }
}