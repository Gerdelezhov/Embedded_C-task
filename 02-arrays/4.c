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
        int top = 0, right = N - 1, bottom = N - 1, left = 0, val = 0;

        while (val < N * N) {
            for (int j = left; j <= right; j++) {
                val++;
                matrix[top][j] = val;
            }
            top++;

            for (int i = top; i <= bottom; i++) {
                val++;
                matrix[i][right] = val;
            }
            right--;

            for (int j = right; j >= left; j--) {
                val++;
                matrix[bottom][j] = val;
            }
            bottom--;

            for (int i = bottom; i >= top; i--) {
                val++;
                matrix[i][left] = val;
            }
            left++;
        }

        for (int i = 0; i < N; i++) {
            for (int j = 0; j < N; j++) {
                printf("%-6d ", matrix[i][j]);
            }
            printf("\n");
        }
    }
    return 0;
}