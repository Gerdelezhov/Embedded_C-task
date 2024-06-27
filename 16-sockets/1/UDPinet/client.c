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
    int client_socket;
    struct sockaddr_in server_addr, client_addr;
    char buffer[100] = {0};
    socklen_t addr_size = sizeof(struct sockaddr_in);

    client_socket = socket(AF_INET, SOCK_DGRAM, 0);
    if (client_socket == -1) {
        handle_error("error socket", -1);
    }

    setup_address(&server_addr, 2003);
    setup_address(&client_addr, 2004);

    if (bind(client_socket, (struct sockaddr *)&client_addr, addr_size) == -1) {
        handle_error("error BIND", client_socket);
    }

    strcpy(buffer, "Hi");
    if (sendto(client_socket, buffer, strlen(buffer) + 1, 0,
               (const struct sockaddr *)&server_addr, addr_size) == -1) {
        handle_error("error send", client_socket);
    }

    memset(buffer, 0, sizeof(buffer));
    if (recvfrom(client_socket, buffer, sizeof(buffer), 0, NULL, NULL) == -1) {
        handle_error("error recv", client_socket);
    }

    printf("%s\n", buffer);
    close(client_socket);
    return 0;
}
