#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define PORT 8080
#define BUFSIZE 1024

int main() {
    int sock;
    struct sockaddr_in server_addr;
    char buffer[BUFSIZE];
    int bytes_read;
    socklen_t addr_len = sizeof(server_addr);

    sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock == -1) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    if (inet_pton(AF_INET, "127.0.0.1", &server_addr.sin_addr) <= 0) {
        perror("Invalid address");
        close(sock);
        exit(EXIT_FAILURE);
    }

    strcpy(buffer, "Hi");
    sendto(sock, buffer, strlen(buffer), 0, (struct sockaddr*)&server_addr,
           addr_len);

    bytes_read = recvfrom(sock, buffer, BUFSIZE, 0,
                          (struct sockaddr*)&server_addr, &addr_len);
    if (bytes_read > 0) {
        buffer[bytes_read] = '\0';
        printf("Server: %s\n", buffer);
    }

    close(sock);
    return 0;
}
