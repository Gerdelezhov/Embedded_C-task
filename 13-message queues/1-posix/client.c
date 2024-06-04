// client_posix.c
#include <fcntl.h>
#include <mqueue.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#define QUEUE_NAME "/test_queue"
#define MAX_SIZE 1024

int main() {
    mqd_t mq;
    char buffer[MAX_SIZE];

    mq = mq_open(QUEUE_NAME, O_RDONLY);
    if (mq == (mqd_t)-1) {
        perror("mq_open");
        exit(1);
    }

    ssize_t bytes_read = mq_receive(mq, buffer, MAX_SIZE, NULL);
    if (bytes_read >= 0) {
        buffer[bytes_read] = '\0';
        printf("Client: Received message: %s\n", buffer);
    } else {
        perror("mq_receive");
    }

    mq_close(mq);

    mq = mq_open(QUEUE_NAME, O_WRONLY);
    if (mq == (mqd_t)-1) {
        perror("mq_open");
        exit(1);
    }

    strcpy(buffer, "Hello!");
    if (mq_send(mq, buffer, MAX_SIZE, 0) == -1) {
        perror("mq_send");
        exit(1);
    }
    printf("Client: Sent message: %s\n", buffer);

    mq_close(mq);

    return 0;
}
