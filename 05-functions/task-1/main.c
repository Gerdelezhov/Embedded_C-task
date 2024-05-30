#include <stdio.h>
#include <stdlib.h>

#include "directory.h"

int start() {
    int input;
    char name[10], second_name[10], tel[10];

    while (1) {
        printf(
            "\n1 - Добавить абонента\n2 - Удалить абонента\n3 - Поиск "
            "абонентов "
            "по имени\n4 - Вывод всех записей\n5 - Выход\n");
        scanf("%d", &input);

        system("clear");

        switch (input) {
            case 1:
                printf("Ведите имя пользователя (10 симв): ");
                scanf("%s", name);

                printf("Ведите фамилию пользователя (10 симв): ");
                scanf("%s", second_name);

                printf("Ведите телефон пользователя (10 симв): ");
                scanf("%s", tel);

                add_subscr(name, second_name, tel);
                printf("\nАбонент добавлен\n");
                break;
            case 2:
                printf("Ведите имя пользователя для удаления (10 симв): ");
                scanf("%s", name);

                delete_subscr(name);
                break;
            case 3:
                printf("Ведите имя пользователя для удаления (10 симв): ");
                scanf("%s", name);

                find_subscr(name);
                break;
            case 4:
                print_all();
                break;
            case 5:
                return 0;
        }
    }
}

int main() {
    system("clear");
    start();
}