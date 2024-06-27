#include <arpa/inet.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define PORT 8080
#define BUFSIZE 1024

void* handle_tcp_client(void* arg) {
    int client_sock = *((int*)arg);
    free(arg);

    char buffer[BUFSIZE];
    int bytes_read;
    struct sockaddr_in client_addr;
    socklen_t addr_len = sizeof(client_addr);
    getpeername(client_sock, (struct sockaddr*)&client_addr, &addr_len);

    while ((bytes_read = recv(client_sock, buffer, BUFSIZE, 0)) > 0) {
        buffer[bytes_read] = '\0';
        printf("TCP Client [%s:%d]: %s\n", inet_ntoa(client_addr.sin_addr),
               ntohs(client_addr.sin_port), buffer);
        if (strcmp(buffer, "Hi") == 0) {
            char response[] = "Hello";
            send(client_sock, response, strlen(response), 0);
        }
    }

    close(client_sock);
    return NULL;
}

void* handle_udp_client(void* arg) {
    int udp_sock = *((int*)arg);
    struct sockaddr_in client_addr;
    socklen_t addr_len = sizeof(client_addr);
    char buffer[BUFSIZE];
    int bytes_read;

    while (1) {
        bytes_read = recvfrom(udp_sock, buffer, BUFSIZE, 0,
                              (struct sockaddr*)&client_addr, &addr_len);
        if (bytes_read > 0) {
            buffer[bytes_read] = '\0';
            printf("UDP Client [%s:%d]: %s\n", inet_ntoa(client_addr.sin_addr),
                   ntohs(client_addr.sin_port), buffer);
            if (strcmp(buffer, "Hi") == 0) {
                char response[] = "Hello";
                sendto(udp_sock, response, strlen(response), 0,
                       (struct sockaddr*)&client_addr, addr_len);
            }
        }
    }

    close(udp_sock);
    return NULL;
}

int main() {
    int tcp_sock, udp_sock;
    struct sockaddr_in server_addr;

    tcp_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (tcp_sock == -1) {
        perror("TCP socket creation failed");
        exit(EXIT_FAILURE);
    }

    udp_sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (udp_sock == -1) {
        perror("UDP socket creation failed");
        exit(EXIT_FAILURE);
    }

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    if (bind(tcp_sock, (struct sockaddr*)&server_addr, sizeof(server_addr)) ==
        -1) {
        perror("TCP bind failed");
        close(tcp_sock);
        exit(EXIT_FAILURE);
    }

    if (bind(udp_sock, (struct sockaddr*)&server_addr, sizeof(server_addr)) ==
        -1) {
        perror("UDP bind failed");
        close(udp_sock);
        exit(EXIT_FAILURE);
    }

    if (listen(tcp_sock, 5) == -1) {
        perror("Listen failed");
        close(tcp_sock);
        exit(EXIT_FAILURE);
    }

    printf("Server is listening on port %d...\n", PORT);

    pthread_t udp_thread;
    pthread_create(&udp_thread, NULL, handle_udp_client, &udp_sock);

    while (1) {
        struct sockaddr_in client_addr;
        socklen_t addr_len = sizeof(client_addr);
        int* new_sock = malloc(sizeof(int));
        *new_sock = accept(tcp_sock, (struct sockaddr*)&client_addr, &addr_len);
        if (*new_sock == -1) {
            perror("Accept failed");
            free(new_sock);
            continue;
        }

        pthread_t tcp_thread;
        pthread_create(&tcp_thread, NULL, handle_tcp_client, new_sock);
        pthread_detach(tcp_thread);
    }

    close(tcp_sock);
    return 0;
}
