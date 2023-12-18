#include "directory.h"

#include <stdio.h>
#include <string.h>

struct abonent handbook[DIRECTORY_LENGTH];
int number_of_users = 0;

int add_subscr(char name[10], char second_name[10], char tel[10]) {
    strcpy(handbook[number_of_users].name, name);
    strcpy(handbook[number_of_users].second_name, second_name);
    strcpy(handbook[number_of_users].tel, tel);

    number_of_users++;
    return 0;
}

int delete_subscr(char name[10]) {
    int found = 0;

    for (int i = 0; i < number_of_users; ++i) {
        if (strcmp(handbook[i].name, name) == 0) {
            memset(&handbook[i], 0, sizeof(struct abonent));
            found = 1;

            printf("Абонент удален.\n");
        }

        if (found &&
            i < number_of_users -
                    1) {  //сдвиг всех пользователей после удаленного на 1 влево
            handbook[i] = handbook[i + 1];
            memset(&handbook[i + 1], 0, sizeof(struct abonent));
        }
    }

    if (found) {
        (number_of_users)--;
    } else {
        printf("Абонент с таким именем не найден.\n");
    }

    return 0;
}

int find_subscr(char name[10]) {
    int found = 1;

    for (int i = 0; i < number_of_users; ++i) {
        if (strcmp(handbook[i].name, name) == 0) {
            found = 0;
            printf("Позаданным праметрам найден абонент:");
            printf("Имя: %s Фамилия: %s Телефон: %s\n", handbook[i].name,
                   handbook[i].second_name, handbook[i].tel);
        }
    }

    if (found) {
        printf("Абонент с таким именем не найден.\n");
    }

    return 0;
}

int print_all() {
    printf("Все записи справочника:\n");
    for (int i = 0; i < number_of_users; ++i) {
        if (handbook[i].name[0] != '\0') {
            printf("Имя: %s Фамилия: %s Телефон: %s\n", handbook[i].name,
                   handbook[i].second_name, handbook[i].tel);
        }
    }
    return 0;
}