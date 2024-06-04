#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>

#define FIFO_NAME "/tmp/my_fifo"
#define BUFFER_SIZE 128

int main() {
    char buffer[BUFFER_SIZE];

    printf("Клиент: Ожидание данных от сервера...\n");

    // Открытие канала на чтение
    int fd = open(FIFO_NAME, O_RDONLY);
    if (fd == -1) {
        perror("open");
        exit(EXIT_FAILURE);
    }

    // Чтение сообщения из канала
    ssize_t num_bytes = read(fd, buffer, BUFFER_SIZE);
    if (num_bytes == -1) {
        perror("read");
        close(fd);
        exit(EXIT_FAILURE);
    }

    // Закрытие дескриптора канала
    close(fd);

    // Завершение строки нулевым байтом и вывод сообщения
    buffer[num_bytes] = '\0';
    printf("Клиент: Получено сообщение: %s\n", buffer);

    printf("Клиент: Завершение работы.\n");

    return 0;
}
