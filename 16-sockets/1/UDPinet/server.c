#include <errno.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

void handle_error(const char *msg, int sock) {
    printf("%s %d\n", msg, errno);
    if (sock != -1) {
        close(sock);
    }
    exit(EXIT_FAILURE);
}

void setup_address(struct sockaddr_in *address, int port) {
    memset(address, 0, sizeof(struct sockaddr_in));
    address->sin_family = AF_INET;
    address->sin_addr.s_addr = INADDR_ANY;
    address->sin_port = htons(port);
}

int main() {
    int server_socket;
    struct sockaddr_in server_addr, client_addr;
    char buffer[100] = {0};
    socklen_t addr_size = sizeof(struct sockaddr_in);

    server_socket = socket(AF_INET, SOCK_DGRAM, 0);
    if (server_socket == -1) {
        handle_error("error socket", -1);
    }

    setup_address(&server_addr, 2003);

    if (bind(server_socket, (struct sockaddr *)&server_addr, addr_size) == -1) {
        handle_error("error BIND", server_socket);
    }

    if (recvfrom(server_socket, buffer, sizeof(buffer), 0,
                 (struct sockaddr *)&client_addr, &addr_size) == -1) {
        handle_error("error recv", server_socket);
    }

    printf("%s\n", buffer);

    memset(buffer, 0, sizeof(buffer));
    strcat(buffer, "Hello");

    if (sendto(server_socket, buffer, strlen(buffer) + 1, 0,
               (struct sockaddr *)&client_addr, addr_size) == -1) {
        handle_error("error send", server_socket);
    }

    close(server_socket);
    return 0;
}
