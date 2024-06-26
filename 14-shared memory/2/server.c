#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <signal.h>
#include <errno.h>

#define SHM_SIZE 1024
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
    Message messages[MAX_CLIENTS];
    int msg_count;
} ChatRoom;

int shm_id;
ChatRoom *chat_room;

void cleanup(int sig) {
    shmctl(shm_id, IPC_RMID, NULL);
    exit(0);
}

void initialize_chat_room() {
    for (int i = 0; i < MAX_CLIENTS; i++) {
        chat_room->clients[i].active = 0;
        memset(chat_room->clients[i].name, 0, MAX_NAME_LEN);
    }
    chat_room->msg_count = 0;
    for (int i = 0; i < MAX_CLIENTS; i++) {
        memset(chat_room->messages[i].message, 0, MAX_MSG_LEN);
    }
}

int main() {
    key_t key = ftok("chatroom", 65);

    // Remove any existing shared memory segment
    shm_id = shmget(key, SHM_SIZE, 0666);
    if (shm_id != -1) {
        shmctl(shm_id, IPC_RMID, NULL);
    }

    // Create a new shared memory segment
    shm_id = shmget(key, SHM_SIZE, IPC_CREAT | 0666);
    if (shm_id == -1) {
        perror("shmget");
        exit(1);
    }

    chat_room = (ChatRoom *)shmat(shm_id, NULL, 0);
    if (chat_room == (ChatRoom *)(-1)) {
        perror("shmat");
        exit(1);
    }

    initialize_chat_room();

    struct sigaction sa;
    sa.sa_handler = cleanup;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    sigaction(SIGINT, &sa, NULL);

    printf("Server is running...\n");

    while (1) {
        sleep(1);  // Server main loop placeholder
    }

    return 0;
}
