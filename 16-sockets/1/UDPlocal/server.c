#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/un.h>
#include <unistd.h>

void handle_error(const char *msg, int sock) {
    printf("%s %d\n", msg, errno);
    if (sock != -1) {
        close(sock);
    }
    exit(EXIT_FAILURE);
}

void setup_local_address(struct sockaddr_un *address, const char *path) {
    memset(address, 0, sizeof(struct sockaddr_un));
    address->sun_family = AF_UNIX;
    strncpy(address->sun_path, path, sizeof(address->sun_path) - 1);
}

int main() {
    int server_socket;
    struct sockaddr_un server_addr, client_addr;
    char buffer[100] = {0};
    socklen_t addr_size = sizeof(struct sockaddr_un);

    server_socket = socket(AF_UNIX, SOCK_DGRAM, 0);
    if (server_socket == -1) {
        handle_error("error socket", -1);
    }

    setup_local_address(&server_addr, "/tmp/udp_local_socket");

    unlink("/tmp/udp_local_socket");

    if (bind(server_socket, (struct sockaddr *)&server_addr, addr_size) == -1) {
        handle_error("error bind", server_socket);
    }

    if (recvfrom(server_socket, buffer, sizeof(buffer), 0,
                 (struct sockaddr *)&client_addr, &addr_size) == -1) {
        handle_error("error recv", server_socket);
    }

    printf("%s\n", buffer);

    memset(buffer, 0, sizeof(buffer));
    strcpy(buffer, "Hello");

    if (sendto(server_socket, buffer, strlen(buffer) + 1, 0,
               (struct sockaddr *)&client_addr, addr_size) == -1) {
        handle_error("error send", server_socket);
    }

    close(server_socket);
    unlink("/tmp/udp_local_socket");
    return 0;
}
