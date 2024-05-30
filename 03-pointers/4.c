#include <stdio.h>
#include <string.h>

#define MAX_LENGTH 100

char* sub_str(const char* str, const char* substr) {
    int len = strlen(substr);
    int str_len = strlen(str);

    for (int i = 0; i <= str_len - len; i++) {
        int j;
        for (j = 0; j < len; j++) {
            if (str[i + j] != substr[j]) {
                break;
            }
        }

        if (j == len) {
            return (char*)&str[i];
        }
    }

    return NULL;
}

int main() {
    char str1[MAX_LENGTH];
    char str2[MAX_LENGTH];

    printf("Введите строку: ");
    scanf("%99s", str1);
    printf("Адрес введенной строки: %p\n", &str1);

    printf("Введите подстроку для поиска: ");
    scanf("%99s", str2);

    char* result = sub_str(str1, str2);

    if (result != NULL) {
        printf("Подстрока найдена по адресу: %p\n", (void*)result);
    } else {
        printf("Подстрока не найдена %p\n", (void*)result);
    }

    return 0;
}
