#include <stdio.h>
#include <stdlib.h>

#include "directory.h"

int menu();

int main() {
    menu();
    return 0;
}

int menu() {
    struct directory dir;
    initialize_directory(&dir);

    while (1) {
        printf(
            "\n1 - Добавить абонента\n2 - Удалить абонента\n3 - Поиск абонента "
            "по имени\n4 - Вывести все записи\n5 - Выйти\n");
        int choice;
        scanf("%d", &choice);
        getchar();  // Удаление символа новой строки, оставленного scanf

        system("clear");

        switch (choice) {
            case 1: {
                char name[NAME_LENGTH], second_name[NAME_LENGTH],
                    tel[TELEPHONE_LENGTH];
                printf("Введите имя (до 32 символов): ");
                fgets(name, NAME_LENGTH, stdin);
                if (name[strlen(name) - 1] != '\n') {
                    while (getchar() != '\n')
                        ;  // Сбрасываем остаток ввода
                } else {
                    name[strcspn(name, "\n")] =
                        '\0';  // Удаление символа новой строки
                }

                printf("Введите фамилию (до 32 символов): ");
                fgets(second_name, NAME_LENGTH, stdin);
                if (second_name[strlen(second_name) - 1] != '\n') {
                    while (getchar() != '\n')
                        ;
                } else {
                    second_name[strcspn(second_name, "\n")] = '\0';
                }

                printf("Введите номер телефона (до 16 символов): ");
                fgets(tel, TELEPHONE_LENGTH, stdin);
                if (tel[strlen(tel) - 1] != '\n') {
                    while (getchar() != '\n')
                        ;
                } else {
                    tel[strcspn(tel, "\n")] = '\0';
                }

                add_subscr(&dir, name, second_name, tel);
                printf("Абонент добавлен.\n");
                break;
            }
            case 2: {
                char name[NAME_LENGTH];
                printf("Введите имя абонента для удаления: ");
                fgets(name, NAME_LENGTH, stdin);
                name[strcspn(name, "\n")] =
                    '\0';
                delete_subscr(&dir, name);
                break;
            }
            case 3: {
                char name[NAME_LENGTH];
                printf("Введите имя абонента для поиска: ");
                fgets(name, NAME_LENGTH, stdin);
                name[strcspn(name, "\n")] =
                    '\0';
                find_subscr(&dir, name);
                break;
            }
            case 4:
                print_all(&dir);
                break;
            case 5:
                free_directory(&dir);
                free(&dir);
                return 0;
            default:
                printf("Неверный выбор. Пожалуйста, попробуйте снова.\n");
        }
    }
}