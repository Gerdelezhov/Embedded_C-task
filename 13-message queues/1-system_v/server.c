// server_sysv.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/msg.h>

#define QUEUE_KEY 1234
#define MAX_SIZE 1024

struct message {
    long msg_type;
    char msg_text[MAX_SIZE];
};

int main() {
    int msgid;
    struct message msg;

    msgid = msgget(QUEUE_KEY, IPC_CREAT | 0666);
    if (msgid == -1) {
        perror("msgget");
        exit(1);
    }

    msg.msg_type = 1;
    strcpy(msg.msg_text, "Hi!");

    if (msgsnd(msgid, &msg, sizeof(msg.msg_text), 0) == -1) {
        perror("msgsnd");
        exit(1);
    }
    printf("Server: Sent message: %s\n", msg.msg_text);

    if (msgrcv(msgid, &msg, sizeof(msg.msg_text), 2, 0) == -1) {
        perror("msgrcv");
        exit(1);
    }
    printf("Server: Received message: %s\n", msg.msg_text);

    if (msgctl(msgid, IPC_RMID, NULL) == -1) {
        perror("msgctl");
        exit(1);
    }

    return 0;
}
