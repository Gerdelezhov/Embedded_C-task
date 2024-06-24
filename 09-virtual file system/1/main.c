#include <stdio.h>
#include <stdlib.h>

int main() {
    FILE *file = fopen("output.txt", "w");
    if (file == NULL) {
        printf("Ошибка открытия файла");
        return 1;
    }

    if (fputs("Съешь ещё этих мягких французских булок, да выпей чаю.", file) ==
        EOF) {
        printf("Не удалось записать в файл");
        return 1;
    }

    fclose(file);
    file = fopen("output.txt", "r");
    if (file == NULL) {
        printf("Ошибка открытия файла");
        return 1;
    }

    fseek(file, 0, SEEK_END);
    int length = ftell(file);
    fseek(file, 0, SEEK_SET);

    char *buffer = (char *)malloc(length + 1);

    fread(buffer, 1, length, file);
    buffer[length] = '\0';

    printf("Строка из файла в обратном порядке: ");
    for (int i = length - 1; i >= 0; i--) {
        putchar(buffer[i]);
    }
    printf("\n");

    fclose(file);
    free(buffer);

    return EXIT_SUCCESS;
}
