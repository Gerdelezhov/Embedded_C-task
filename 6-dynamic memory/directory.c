#include "directory.h"

void initialize_directory(struct directory *dir) {
    dir->size = 0;
    dir->capacity = 0;
    dir->entries = NULL;
}

void resize_directory(struct directory *dir) {
    dir->capacity = (dir->capacity == 0) ? 1 : dir->capacity * 2;
    dir->entries = (struct abonent *)realloc(
        dir->entries, sizeof(struct abonent) * dir->capacity);
}

void add_subscr(struct directory *dir, const char *name,
                const char *second_name, const char *tel) {
    if (dir->capacity == 0) {
        resize_directory(dir);
    } else if (dir->size >= dir->capacity) {
        resize_directory(dir);
    }

    strcpy(dir->entries[dir->size].name, name);
    strcpy(dir->entries[dir->size].second_name, second_name);
    strcpy(dir->entries[dir->size].tel, tel);

    dir->size++;
}

void delete_subscr(struct directory *dir, const char *name) {
    int i;
    for (i = 0; i < dir->size; i++) {
        if (strcmp(dir->entries[i].name, name) == 0) {
            // Перемещение последней записи в позицию удаленной
            dir->entries[i] = dir->entries[dir->size - 1];
            dir->size--;
            printf("Абонент удален.\n");
            return;
        }
    }
    printf("Абонент с именем %s не найден.\n", name);
}

void find_subscr(const struct directory *dir, const char *name) {
    int found = 0;

    for (int i = 0; i < dir->size; ++i) {
        if (strcmp(dir->entries[i].name, name) == 0) {
            found = 1;
            printf("Найден абонент по запросу:\n");
            printf("Имя: %s, Фамилия: %s, Телефон: %s\n", dir->entries[i].name,
                   dir->entries[i].second_name, dir->entries[i].tel);
        }
    }

    if (!found) {
        printf("Абонент с именем %s не найден.\n", name);
    }
}

void print_all(const struct directory *dir) {
    printf("Все записи в справочнике:\n");
    for (int i = 0; i < dir->size; ++i) {
        printf("Имя: %s, Фамилия: %s, Телефон: %s\n", dir->entries[i].name,
               dir->entries[i].second_name, dir->entries[i].tel);
    }
}

void free_directory(struct directory *dir) { free(dir->entries); }
