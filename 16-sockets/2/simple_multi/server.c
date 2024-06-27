#include <netinet/in.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#define BUFLEN 10
#define SERVER_PORT 12345
#define REPLY_MSG "Hello"

void* ClientHandler(void* arg) {
    int sockClient = *((int*)arg);
    char buf[BUFLEN];
    int msgLength;

    for (int i = 0; i < 10; i++) {
        if ((msgLength = recv(sockClient, buf, BUFLEN, 0)) <= 0) {
            perror("Error receiving message");
            close(sockClient);
            return NULL;
        }

        printf("Received message from client: %s\n", buf);

        if (send(sockClient, REPLY_MSG, strlen(REPLY_MSG), 0) < 0) {
            perror("Error sending message");
            close(sockClient);
            return NULL;
        }

        printf("Sent message to client: %s\n", REPLY_MSG);
        sleep(2);
    }

    close(sockClient);
    return NULL;
}

int main() {
    int sockMain, sockClient;
    struct sockaddr_in servAddr;

    if ((sockMain = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Error create main socket");
        exit(1);
    }

    bzero((char*)&servAddr, sizeof(servAddr));
    servAddr.sin_family = AF_INET;
    servAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servAddr.sin_port = htons(SERVER_PORT);

    if (bind(sockMain, (struct sockaddr*)&servAddr, sizeof(servAddr)) < 0) {
        perror("Error binding.");
        exit(1);
    }

    if (listen(sockMain, 5) < 0) {
        perror("Error listening");
        exit(1);
    }

    pthread_t tid;

    for (;;) {
        if ((sockClient = accept(sockMain, NULL, NULL)) < 0) {
            perror("Error conecting");
            exit(1);
        }

        pthread_create(&tid, NULL, ClientHandler, (void*)&sockClient);
        pthread_detach(tid);
    }

    close(sockMain);
    return 0;
}
