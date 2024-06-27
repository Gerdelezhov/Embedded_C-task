#include <errno.h>
#include <netinet/in.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/msg.h>
#include <sys/socket.h>
#include <unistd.h>

#define BUFLEN 10
#define SERVER_PORT 12345
#define SMSG "Hello"
#define NTHREAD 5

pthread_t threadPool[NTHREAD];
int msgQueue;
int freeThread[NTHREAD];
pthread_mutex_t queueMutex = PTHREAD_MUTEX_INITIALIZER;

struct msgbuf {
    long mtype;
    int sockClient;
};

void *ClientHandler(void *arg) {
    int threadIndex = *((int *)arg);
    free(arg);

    struct msgbuf msg;
    msg.mtype = threadIndex + 1;

    while (1) {
        int read_check = 0;

        while (read_check <= 0) {
            read_check = msgrcv(msgQueue, &msg, sizeof(int), msg.mtype, 0);
            if (read_check < 0) {
                fprintf(stderr, "Thread %d: Error reading: %s\n", threadIndex,
                        strerror(errno));
                sleep(1);
            }
        }

        int sockClient = msg.sockClient;

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
            printf("Thread %d received message from client %d: %s\n",
                   threadIndex, sockClient, buf);

            if (send(sockClient, SMSG, strlen(SMSG), 0) < 0) {
                perror("Error sending message");
                close(sockClient);
                break;
            }

            printf("Thread %d sent message to client %d: %s\n", threadIndex,
                   sockClient, SMSG);
            sleep(2);
        }

        close(sockClient);

        pthread_mutex_lock(&queueMutex);
        msg.mtype = NTHREAD + 1;
        msg.sockClient = threadIndex;
        if (msgsnd(msgQueue, &msg, sizeof(int), 0) < 0) {
            perror("Error writing to queue");
        }
        pthread_mutex_unlock(&queueMutex);
    }

    return NULL;
}

int main() {
    int sockMain, sockClient;
    struct sockaddr_in servAddr;

    msgQueue = msgget(IPC_PRIVATE, IPC_CREAT | 0666);
    if (msgQueue < 0) {
        perror("Error creating message queue");
        exit(1);
    }

    for (int i = 0; i < NTHREAD; i++) {
        freeThread[i] = 1;
    }

    for (int i = 0; i < NTHREAD; i++) {
        int *arg = malloc(sizeof(*arg));
        *arg = i;
        if (pthread_create(&threadPool[i], NULL, ClientHandler, arg) != 0) {
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

        int found = 0;
        while (!found) {
            for (int i = 0; i < NTHREAD; i++) {
                if (freeThread[i]) {
                    struct msgbuf msg;
                    msg.mtype = i + 1;
                    msg.sockClient = sockClient;

                    pthread_mutex_lock(&queueMutex);
                    if (msgsnd(msgQueue, &msg, sizeof(int), 0) < 0) {
                        perror("Error writing to queue");
                    }
                    pthread_mutex_unlock(&queueMutex);

                    freeThread[i] = 0;
                    found = 1;
                    break;
                }
            }

            if (!found) {
                struct msgbuf msg;
                msgrcv(msgQueue, &msg, sizeof(int), NTHREAD + 1, 0);
                freeThread[msg.sockClient] = 1;
            }
        }
    }

    close(sockMain);
    msgctl(msgQueue, IPC_RMID, NULL);
    return 0;
}
