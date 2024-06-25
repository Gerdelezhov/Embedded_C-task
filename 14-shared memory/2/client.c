#include <ncurses.h>
#include <pthread.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/shm.h>
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
char username[MAX_NAME_LEN];
WINDOW *chat_win, *input_win, *user_win;
int running = 1;

void *receive_messages(void *arg) {
    int last_msg_count = 0;
    while (running) {
        if (chat_room->msg_count != last_msg_count) {
            last_msg_count = chat_room->msg_count;
            werase(chat_win);
            for (int i = 0; i < last_msg_count && i < 100; i++) {
                wprintw(chat_win, "%s\n", chat_room->messages[i].message);
            }
            wrefresh(chat_win);

            werase(user_win);
            for (int i = 0; i < MAX_CLIENTS; i++) {
                if (chat_room->clients[i].active) {
                    wprintw(user_win, "%s\n", chat_room->clients[i].name);
                }
            }
            wrefresh(user_win);
        }
        usleep(500000);
    }
    return NULL;
}

void send_message(const char *msg) {
    char full_msg[MAX_MSG_LEN];
    snprintf(full_msg, MAX_MSG_LEN, "%s: %s", username, msg);
    strncpy(chat_room->messages[chat_room->msg_count % 100].message, full_msg,
            MAX_MSG_LEN);
    chat_room->msg_count++;
}

void register_client(const char *name) {
    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (chat_room->clients[i].active == 0) {
            strncpy(chat_room->clients[i].name, name, MAX_NAME_LEN);
            chat_room->clients[i].active = 1;
            char welcome_msg[MAX_MSG_LEN];
            snprintf(welcome_msg, MAX_MSG_LEN, "%s has joined the chat!", name);
            strncpy(chat_room->messages[chat_room->msg_count % 100].message,
                    welcome_msg, MAX_MSG_LEN);
            chat_room->msg_count++;
            return;
        }
    }
}

void unregister_client(const char *name) {
    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (chat_room->clients[i].active &&
            strcmp(chat_room->clients[i].name, name) == 0) {
            chat_room->clients[i].active = 0;
            char exit_msg[MAX_MSG_LEN];
            snprintf(exit_msg, MAX_MSG_LEN, "%s has left the chat!", name);
            strncpy(chat_room->messages[chat_room->msg_count % 100].message,
                    exit_msg, MAX_MSG_LEN);
            chat_room->msg_count++;
            return;
        }
    }
}

void handle_exit() {
    running = 0;
    unregister_client(username);
    endwin();
    shmdt(chat_room);
    exit(0);
}

int main() {
    signal(SIGINT, handle_exit);

    key_t key = ftok("chatroom", 65);
    shm_id = shmget(key, SHM_SIZE, 0666);
    chat_room = (ChatRoom *)shmat(shm_id, (void *)0, 0);

    initscr();
    noecho();
    cbreak();
    int height, width;
    getmaxyx(stdscr, height, width);

    chat_win = newwin(height - 3, width - 20, 0, 0);
    user_win = newwin(height - 3, 20, 0, width - 20);
    input_win = newwin(3, width, height - 3, 0);

    scrollok(chat_win, TRUE);
    scrollok(input_win, TRUE);

    wprintw(input_win, "Enter your username: ");
    wrefresh(input_win);
    wgetstr(input_win, username);
    register_client(username);

    pthread_t recv_thread;
    pthread_create(&recv_thread, NULL, receive_messages, NULL);

    while (running) {
        werase(input_win);
        mvwprintw(input_win, 1, 0, "Type your message: ");
        wrefresh(input_win);
        char msg[MAX_MSG_LEN];
        wgetstr(input_win, msg);
        if (running) {
            send_message(msg);
        }
    }

    handle_exit();
    return 0;
}
