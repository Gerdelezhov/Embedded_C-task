#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#define MAX_INPUT_SIZE 100

int main() {
    char input[MAX_INPUT_SIZE];

    while (1) {
        printf("Введите имя программы и опции (или 'exit' для выхода): ");
        fgets(input, MAX_INPUT_SIZE, stdin);
        input[strcspn(input, "\n")] = '\0';

        if (strcmp(input, "exit") == 0) {
            break;
        }

        pid_t pid = fork();

        if (pid < 0) {
            perror("fork");
            exit(EXIT_FAILURE);
        } else if (pid == 0) {
            char *args[MAX_INPUT_SIZE];
            char *token = strtok(input, " ");
            int i = 0;
            while (token != NULL) {
                args[i++] = token;
                token = strtok(NULL, " ");
            }
            args[i] = NULL;

            execvp(args[0], args);
            perror("execvp");
            exit(EXIT_FAILURE);
        } else {
            int status;
            waitpid(pid, &status, 0);
            /*if (WIFEXITED(status)) {
                printf("Дочерний процесс завершился с кодом: %d\n",
                       WEXITSTATUS(status));
            } else {
                printf("Дочерний процесс завершился нормально\n");
            }*/
        }
    }

    return 0;
}
