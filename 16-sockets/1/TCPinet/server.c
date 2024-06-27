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

void handle_error(const char *msg, int sock1, int sock2) {
    printf("%s %d\n", msg, errno);
    if (sock1 != -1) {
        close(sock1);
    }
    if (sock2 != -1) {
        close(sock2);
    }
    exit(EXIT_FAILURE);
}

int main() {
    int passive_sock, active_sock;
    struct sockaddr_in server_addr, client_addr;
    char buffer[15] = {0};
    socklen_t addr_size = sizeof(struct sockaddr_in);

    memset(&server_addr, 0, sizeof(server_addr));
    memset(&client_addr, 0, sizeof(client_addr));

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(2002);

    passive_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (passive_sock == -1) {
        handle_error("error socket", -1, -1);
    }

    int bind_status =
        bind(passive_sock, (struct sockaddr *)&server_addr, addr_size);
    if (bind_status == -1) {
        handle_error("error bind", passive_sock, -1);
    }

    if (listen(passive_sock, 2) == -1) {
        handle_error("error listen", passive_sock, -1);
    }

    active_sock =
        accept(passive_sock, (struct sockaddr *)&client_addr, &addr_size);
    if (active_sock == -1) {
        handle_error("error accept", passive_sock, -1);
    }

    int recv_status = recv(active_sock, buffer, 10, MSG_WAITALL);
    if (recv_status == -1) {
        handle_error("error recv", passive_sock, active_sock);
    }

    printf("%s\n", buffer);

    memset(buffer, 0, sizeof(buffer));
    strcat(buffer, "Hello");

    int send_status = send(active_sock, buffer, 10, 0);
    if (send_status == -1) {
        handle_error("error send", passive_sock, active_sock);
    }

    close(active_sock);
    close(passive_sock);
    return 0;
}
