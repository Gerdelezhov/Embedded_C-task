#include <errno.h>
#include <netinet/in.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#define BUFLEN 10
#define SERVER_PORT 12345
#define SMSG "Hello  "
#define NTHREAD 5
#define QUEUE_SIZE 50

pthread_t threadPool[NTHREAD];
int clientQueue[QUEUE_SIZE];
int queueStart = 0;
int queueEnd = 0;
pthread_mutex_t queueMutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t queueCond = PTHREAD_COND_INITIALIZER;

void enqueue(int sockClient) {
    clientQueue[queueEnd] = sockClient;
    queueEnd = (queueEnd + 1) % QUEUE_SIZE;
}

int dequeue() {
    int sockClient = clientQueue[queueStart];
    queueStart = (queueStart + 1) % QUEUE_SIZE;
    return sockClient;
}

int isQueueEmpty() { return queueStart == queueEnd; }

void *ClientHandler(void *arg) {
    while (1) {
        pthread_mutex_lock(&queueMutex);
        while (isQueueEmpty()) {
            pthread_cond_wait(&queueCond, &queueMutex);
        }

        int sockClient = dequeue();
        pthread_mutex_unlock(&queueMutex);

        char buf[BUFLEN];
        int msgLength;

        for (int i = 0; i < 10; i++) {
            if ((msgLength = recv(sockClient, buf, BUFLEN, 0)) <= 0) {
                if (msgLength == 0) {
                    printf("Client %d disconnected\n", sockClient);
                }

                close(sockClient);
                break;
            }

            buf[msgLength] = '\0';
            printf("Received message from client %d: %s\n", sockClient, buf);

            if (send(sockClient, SMSG, strlen(SMSG), 0) < 0) {
                perror("Error sending message");
                close(sockClient);
                break;
            }

            printf("Sended message to client %d: %s\n", sockClient, SMSG);
            sleep(2);
        }

        close(sockClient);
    }

    return NULL;
}

int main() {
    int sockMain, sockClient;
    struct sockaddr_in servAddr;

    for (int i = 0; i < NTHREAD; i++) {
        if (pthread_create(&threadPool[i], NULL, ClientHandler, NULL) != 0) {
            perror("Error creating thread");
            exit(1);
        }
    }

    if ((sockMain = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Error creating main socket");
        exit(1);
    }

    memset(&servAddr, 0, sizeof(servAddr));
    servAddr.sin_family = AF_INET;
    servAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servAddr.sin_port = htons(SERVER_PORT);

    if (bind(sockMain, (struct sockaddr *)&servAddr, sizeof(servAddr)) < 0) {
        perror("Error binding");
        exit(1);
    }

    if (listen(sockMain, 5) < 0) {
        perror("Error listening");
        exit(1);
    }

    while (1) {
        if ((sockClient = accept(sockMain, NULL, NULL)) < 0) {
            perror("Error accepting connection");
            exit(1);
        }

        pthread_mutex_lock(&queueMutex);
        enqueue(sockClient);
        pthread_cond_signal(&queueCond);
        pthread_mutex_unlock(&queueMutex);
    }

    close(sockMain);
    return 0;
}
