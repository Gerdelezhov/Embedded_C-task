#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

#define FIFO_NAME "/tmp/my_fifo"
#define MESSAGE "Hi!\n"

int main() {
    // Создание именованного канала (FIFO)
    if (mkfifo(FIFO_NAME, 0666) == -1) {
        perror("mkfifo");
        exit(EXIT_FAILURE);
    }

    printf("Сервер: Ожидание подключения клиента...\n");

    // Открытие канала на запись
    int fd = open(FIFO_NAME, O_WRONLY);
    if (fd == -1) {
        perror("open");
        exit(EXIT_FAILURE);
    }

    printf("Сервер: Подключение установлено. Отправка данных...\n");

    // Запись сообщения в канал
    write(fd, MESSAGE, strlen(MESSAGE));
    close(fd);

    printf("Сервер: Данные отправлены. Завершение работы.\n");

    // Удаление канала
    if (unlink(FIFO_NAME) == -1) {
        perror("unlink");
        exit(EXIT_FAILURE);
    }

    printf("Сервер: Канал удален.\n");

    return 0;
}
