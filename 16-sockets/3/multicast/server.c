#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>

#define MAXBUF 1024
#define PORT 12345
#define GROUP "239.0.0.1"

int main() {
    struct sockaddr_in addr;
    int sockfd, nbytes;
    socklen_t addrlen;
    struct ip_mreq mreq;
    char msg[MAXBUF];

    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("socket");
        exit(1);
    }

    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    addr.sin_port = htons(PORT);

    if (bind(sockfd, (struct sockaddr *) &addr, sizeof(addr)) < 0) {
        perror("bind");
        exit(1);
    }

    mreq.imr_multiaddr.s_addr = inet_addr(GROUP);
    mreq.imr_interface.s_addr = htonl(INADDR_ANY);

    if (setsockopt(sockfd, IPPROTO_IP, IP_ADD_MEMBERSHIP, &mreq, sizeof(mreq)) < 0) {
        perror("setsockopt");
        exit(1);
    }

    printf("Multicast сервер запущен...\n");

    while (1) {
        addrlen = sizeof(addr);
        if ((nbytes = recvfrom(sockfd, msg, MAXBUF, 0, (struct sockaddr *) &addr, &addrlen)) < 0) {
            perror("recvfrom");
            exit(1);
        }

        printf("Получено от клиента: %s", msg);
    }

    close(sockfd);
    return 0;
}
