// server_posix.c
#include <fcntl.h>
#include <mqueue.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

#define QUEUE_NAME "/test_queue"
#define MAX_SIZE 1024

int main() {
    mqd_t mq;
    struct mq_attr attr;
    char buffer[MAX_SIZE];

    attr.mq_flags = 0;
    attr.mq_maxmsg = 10;
    attr.mq_msgsize = MAX_SIZE;
    attr.mq_curmsgs = 0;

    mq = mq_open(QUEUE_NAME, O_CREAT | O_WRONLY, 0644, &attr);
    if (mq == (mqd_t)-1) {
        perror("mq_open");
        exit(1);
    }

    strcpy(buffer, "Hi!");
    if (mq_send(mq, buffer, MAX_SIZE, 0) == -1) {
        perror("mq_send");
        exit(1);
    }
    printf("Server: Sent message: %s\n", buffer);

    mq_close(mq);

    mq = mq_open(QUEUE_NAME, O_RDONLY);
    if (mq == (mqd_t)-1) {
        perror("mq_open");
        exit(1);
    }

    ssize_t bytes_read = mq_receive(mq, buffer, MAX_SIZE, NULL);
    if (bytes_read >= 0) {
        buffer[bytes_read] = '\0';
        printf("Server: Received message: %s\n", buffer);
    } else {
        perror("mq_receive");
    }

    mq_close(mq);
    mq_unlink(QUEUE_NAME);

    // Добавляем задержку перед завершением процесса (в секундах)
    sleep(5);

    return 0;
}
