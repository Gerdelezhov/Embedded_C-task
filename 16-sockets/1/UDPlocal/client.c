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
    int client_socket;
    struct sockaddr_un server_addr, client_addr;
    char buffer[100] = {0};
    socklen_t addr_size = sizeof(struct sockaddr_un);

    client_socket = socket(AF_UNIX, SOCK_DGRAM, 0);
    if (client_socket == -1) {
        handle_error("error socket", -1);
    }

    // Bind the client socket to a unique address
    setup_local_address(&client_addr, "/tmp/udp_client_socket");
    unlink("/tmp/udp_client_socket");
    if (bind(client_socket, (struct sockaddr *)&client_addr, addr_size) == -1) {
        handle_error("error bind", client_socket);
    }

    setup_local_address(&server_addr, "/tmp/udp_local_socket");

    strcpy(buffer, "Hi");
    if (sendto(client_socket, buffer, strlen(buffer) + 1, 0,
               (const struct sockaddr *)&server_addr,
               sizeof(server_addr)) == -1) {
        handle_error("error send", client_socket);
    }

    memset(buffer, 0, sizeof(buffer));
    if (recvfrom(client_socket, buffer, sizeof(buffer), 0, NULL, NULL) == -1) {
        handle_error("error recv", client_socket);
    }

    printf("%s\n", buffer);
    close(client_socket);
    unlink("/tmp/udp_client_socket");
    return 0;
}
