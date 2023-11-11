#include <stdio.h>
#define S 1000

int main() {
    int N;
    printf("Введите размерность матрицы от 1 до 1000: ");
    scanf("%d", &N);

    if (N < 1 || N > 1000) {
        printf("Ошибка! Чиcло должно быть в промежутке: [1:1000]\n");
    } else {
        int matrix[S][S];

        for (int i = 0; i < N; i++) {
            for (int j = 0; j < N; j++) {
                if (i < N - j - 1) {
                    matrix[i][j] = 0;
                } else {
                    matrix[i][j] = 1;
                }
            }
        }

        for (int i = 0; i < N; i++) {
            for (int j = 0; j < N; j++) {
                printf("%d ", matrix[i][j]);
            }
            printf("\n");
        }
    }
    return 0;
}