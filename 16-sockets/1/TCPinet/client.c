#include <errno.h>
#include <netdb.h>
#include <netinet/in.h>
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
    exit(1);
}

int main() {
    int sock;
    struct sockaddr_in server_addr;
    char buffer[100] = {0};

    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == -1) {
        handle_error("error socket", -1);
    }

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(2002);

    socklen_t addr_size = sizeof(struct sockaddr_in);

    int connection_status =
        connect(sock, (const struct sockaddr *)&server_addr, addr_size);
    if (connection_status == -1) {
        handle_error("error connect", sock);
    }

    strcat(buffer, "Hi");
    int send_status = send(sock, buffer, 10, 0);
    if (send_status == -1) {
        handle_error("error send", sock);
    }

    memset(buffer, 0, sizeof(buffer));
    int recv_status = recv(sock, buffer, 10, 0);
    if (recv_status == -1) {
        handle_error("error recv", sock);
    }

    printf("%s\n", buffer);
    close(sock);
    return 0;
}
