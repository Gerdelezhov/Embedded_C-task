#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <unistd.h>

#define SHM_SIZE 4096
#define MAX_CLIENTS 10
#define MAX_NAME_LEN 32
#define MAX_MSG_LEN 256

typedef struct {
    char name[MAX_NAME_LEN];
    int active;
} Client;

typedef struct {
    char message[MAX_MSG_LEN];
} Message;

typedef struct {
    Client clients[MAX_CLIENTS];
    Message messages[100];
    int msg_count;
} ChatRoom;

int shm_id;
ChatRoom *chat_room;

void broadcast_message(const char *msg) {
    strncpy(chat_room->messages[chat_room->msg_count % 100].message, msg,
            MAX_MSG_LEN);
    chat_room->msg_count++;
}

void handle_new_client(const char *name) {
    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (chat_room->clients[i].active == 0) {
            strncpy(chat_room->clients[i].name, name, MAX_NAME_LEN);
            chat_room->clients[i].active = 1;
            char welcome_msg[MAX_MSG_LEN];
            snprintf(welcome_msg, MAX_MSG_LEN, "%s has joined the chat!", name);
            broadcast_message(welcome_msg);
            return;
        }
    }
}

void handle_client_exit(const char *name) {
    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (chat_room->clients[i].active &&
            strcmp(chat_room->clients[i].name, name) == 0) {
            chat_room->clients[i].active = 0;
            char exit_msg[MAX_MSG_LEN];
            snprintf(exit_msg, MAX_MSG_LEN, "%s has left the chat!", name);
            broadcast_message(exit_msg);
            return;
        }
    }
}

void handle_client_message(const char *name, const char *msg) {
    char full_msg[MAX_MSG_LEN];
    snprintf(full_msg, MAX_MSG_LEN, "%s: %s", name, msg);
    broadcast_message(full_msg);
}

int main() {
    key_t key = ftok("/chatroom", 65);
    shm_id = shmget(key, SHM_SIZE, 0666 | IPC_CREAT);
    chat_room = (ChatRoom *)shmat(shm_id, (void *)0, 0);

    memset(chat_room, 0, sizeof(ChatRoom));

    while (1) {
        // Server main loop to handle new clients and messages
        sleep(1);
    }

    shmdt(chat_room);
    shmctl(shm_id, IPC_RMID, NULL);

    return 0;
}
