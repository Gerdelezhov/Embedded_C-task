#include <stdio.h>
#define S 1000

int main() {
    int N;

    printf("Введите колличество элементов массива от 1 до 1000: ");
    scanf("%d", &N);

    if (N < 1 || N > 1000) {
        printf("Ошибка! Чиcло должно быть в промежутке: [1:1000]\n");
    } else {
        int arr[S] = {0};

        for (int i = 0; i < N; i++) {
            scanf("%d", &arr[i]);
        }

        int buff;
        for (int i = 0; i < N / 2; i++) {
            buff = arr[N - 1 - i];
            arr[N - 1 - i] = arr[i];
            arr[i] = buff;
        }

        printf("Развернутый массив: ");
        for (int i = 0; i < N; i++) {
            printf("%d ", arr[i]);
        }
    }

    printf("\n");

    return 0;
}