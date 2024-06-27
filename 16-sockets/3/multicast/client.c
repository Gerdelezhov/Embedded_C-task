#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <pthread.h>

#define MAXBUF 1024
#define PORT 12345
#define GROUP "239.0.0.1"

void* receive_messages(void* arg) {
    int sockfd = *((int*)arg);
    struct sockaddr_in addr;
    socklen_t addrlen;
    char msg[MAXBUF];
    int nbytes;
    struct ip_mreq mreq;

    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    addr.sin_port = htons(PORT);

    if (bind(sockfd, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        perror("bind");
        exit(1);
    }

    mreq.imr_multiaddr.s_addr = inet_addr(GROUP);
    mreq.imr_interface.s_addr = htonl(INADDR_ANY);

    if (setsockopt(sockfd, IPPROTO_IP, IP_ADD_MEMBERSHIP, &mreq, sizeof(mreq)) < 0) {
        perror("setsockopt");
        exit(1);
    }

    while (1) {
        addrlen = sizeof(addr);
        if ((nbytes = recvfrom(sockfd, msg, MAXBUF, 0, (struct sockaddr*)&addr, &addrlen)) < 0) {
            perror("recvfrom");
            exit(1);
        }
        msg[nbytes] = '\0';
        printf("Получено сообщение: %s", msg);
    }
    return NULL;
}

int main() {
    struct sockaddr_in addr;
    int sockfd, nbytes;
    char msg[MAXBUF];
    pthread_t recv_thread;

    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("socket");
        exit(1);
    }

    if (pthread_create(&recv_thread, NULL, receive_messages, &sockfd) != 0) {
        perror("pthread_create");
        exit(1);
    }

    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr(GROUP);
    addr.sin_port = htons(PORT);

    printf("Введите сообщение для отправки (exit для выхода):\n");

    while (1) {
        fgets(msg, MAXBUF, stdin);

        if (strncmp(msg, "exit", 4) == 0)
            break;

        nbytes = sendto(sockfd, msg, strlen(msg), 0, (struct sockaddr*)&addr, sizeof(addr));
        if (nbytes < 0) {
            perror("sendto");
            exit(1);
        }
    }

    pthread_cancel(recv_thread);
    pthread_join(recv_thread, NULL);
    close(sockfd);
    return 0;
}
