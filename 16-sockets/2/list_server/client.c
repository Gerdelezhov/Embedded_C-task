#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#define BUFLEN 10
#define SERVER_IP "127.0.0.1"
#define SERVER_PORT 12345
#define CLIENT_MSG "Hi"

int main() {
    int sock;
    struct sockaddr_in servAddr;
    char buf[BUFLEN];

    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Error opening socket");
        exit(1);
    }

    bzero((char *)&servAddr, sizeof(servAddr));
    servAddr.sin_family = AF_INET;
    servAddr.sin_addr.s_addr = inet_addr(SERVER_IP);
    servAddr.sin_port = htons(SERVER_PORT);

    if (connect(sock, (struct sockaddr *)&servAddr, sizeof(servAddr)) < 0) {
        perror("Error connecting");
        exit(1);
    }

    if (send(sock, CLIENT_MSG, strlen(CLIENT_MSG), 0) < 0) {
        perror("Error sending message");
        exit(1);
    }

    if (recv(sock, buf, BUFLEN, 0) <= 0) {
        perror("Error receiving message");
        exit(1);
    }

    printf("Received message from server: %s\n", buf);

    close(sock);

    return 0;
}
