#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

#define MAX_COMMAND_LEN 1024
#define MAX_ARGS 64

void execute_command(char *command) {
    char *args[MAX_ARGS];
    char *token = strtok(command, " ");
    int arg_count = 0;

    while (token != NULL && arg_count < MAX_ARGS - 1) {
        args[arg_count++] = token;
        token = strtok(NULL, " ");
    }
    args[arg_count] = NULL;

    execvp(args[0], args);
    perror("execvp");
    exit(1);
}

void handle_pipeline(char *commands[], int command_count) {
    int pipefds[2 * (command_count - 1)];
    pid_t pid;
    int i, j;

    // Создаем необходимые каналы (pipes)
    for (i = 0; i < command_count - 1; ++i) {
        if (pipe(pipefds + i * 2) == -1) {
            perror("pipe");
            exit(1);
        }
    }

    for (i = 0; i < command_count; ++i) {
        pid = fork();

        if (pid == 0) {
            // Настройка ввода из предыдущего канала
            if (i > 0) {
                if (dup2(pipefds[(i - 1) * 2], 0) == -1) {
                    perror("dup2");
                    exit(1);
                }
            }

            // Настройка вывода в следующий канал
            if (i < command_count - 1) {
                if (dup2(pipefds[i * 2 + 1], 1) == -1) {
                    perror("dup2");
                    exit(1);
                }
            }

            // Закрытие всех файловых дескрипторов каналов
            for (j = 0; j < 2 * (command_count - 1); ++j) {
                close(pipefds[j]);
            }

            execute_command(commands[i]);
        } else if (pid < 0) {
            perror("fork");
            exit(1);
        }
    }

    // Закрытие всех файловых дескрипторов каналов в родительском процессе
    for (i = 0; i < 2 * (command_count - 1); ++i) {
        close(pipefds[i]);
    }

    // Ожидание завершения всех дочерних процессов
    for (i = 0; i < command_count; ++i) {
        wait(NULL);
    }
}

int main() {
    char input[MAX_COMMAND_LEN];
    char *commands[MAX_ARGS];
    int command_count;

    while (1) {
        printf("$ ");
        fflush(stdout);

        if (fgets(input, sizeof(input), stdin) == NULL) {
            break;
        }

        // Удаляем символ новой строки
        input[strcspn(input, "\n")] = '\0';

        // Проверяем на команду выхода
        if (strcmp(input, "exit") == 0) {
            break;
        }

        // Разделяем ввод на команды по символу конвейера (|)
        command_count = 0;
        char *token = strtok(input, "|");
        while (token != NULL && command_count < MAX_ARGS - 1) {
            commands[command_count++] = token;
            token = strtok(NULL, "|");
        }
        commands[command_count] = NULL;

        if (command_count > 1) {
            handle_pipeline(commands, command_count);
        } else if (command_count == 1) {
            pid_t pid = fork();
            if (pid == 0) {
                execute_command(commands[0]);
            } else if (pid > 0) {
                wait(NULL);
            } else {
                perror("fork");
            }
        }
    }

    printf("Выход из программы\n");
    return 0;
}
