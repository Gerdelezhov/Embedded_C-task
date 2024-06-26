#include <errno.h>
#include <ncurses.h>
#include <pthread.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h>

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
char username[MAX_NAME_LEN];
WINDOW *chat_win, *input_win, *user_win;
WINDOW *wnd;

struct winsize size;

void sig_winch(int signo) {
    ioctl(fileno(stdout), TIOCGWINSZ, (char *)&size);
    resizeterm(size.ws_row, size.ws_col);
    wresize(wnd, size.ws_row, size.ws_col);
    wresize(chat_win, size.ws_row - 7, (size.ws_col / 4) * 3 - 1);
    wresize(user_win, size.ws_row - 2, (size.ws_col / 4) - 2);
    wresize(input_win, 4, (size.ws_col / 4) * 3 - 1);
    mvwin(chat_win, 1, 1);
    mvwin(user_win, 1, (size.ws_col / 4) * 3 + 1);
    mvwin(input_win, size.ws_row - 5, 1);
    box(wnd, '|', '-');
    wmove(wnd, size.ws_row - 6, 1);
    whline(wnd, '-', (size.ws_col / 4) * 3 - 1);
    wmove(wnd, 1, (size.ws_col / 4) * 3);
    wvline(wnd, '|', size.ws_row - 2);
    wrefresh(wnd);
    wrefresh(chat_win);
    wrefresh(user_win);
    wrefresh(input_win);
}

void update_user_list();  // Предварительное объявление функции

void *receive_messages(void *arg) {
    int last_msg_count = 0;
    while (1) {
        if (chat_room->msg_count != last_msg_count) {
            last_msg_count = chat_room->msg_count;
            werase(chat_win);
            for (int i = 0; i < last_msg_count && i < MAX_CLIENTS; i++) {
                wprintw(chat_win, "%s\n", chat_room->messages[i].message);
            }
            wrefresh(chat_win);
            update_user_list();
        }
        usleep(500000);
    }
    return NULL;
}

void send_message(const char *msg) {
    char full_msg[MAX_MSG_LEN];
    snprintf(full_msg, MAX_MSG_LEN, "%s: %s", username, msg);
    strncpy(chat_room->messages[chat_room->msg_count % MAX_CLIENTS].message,
            full_msg, MAX_MSG_LEN);
    chat_room->msg_count++;
}

void register_client(const char *name) {
    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (chat_room->clients[i].active == 0) {
            strncpy(chat_room->clients[i].name, name, MAX_NAME_LEN);
            chat_room->clients[i].active = 1;
            char welcome_msg[MAX_MSG_LEN];
            snprintf(welcome_msg, MAX_MSG_LEN, "%s has joined the chat!", name);
            strncpy(
                chat_room->messages[chat_room->msg_count % MAX_CLIENTS].message,
                welcome_msg, MAX_MSG_LEN);
            chat_room->msg_count++;
            return;
        }
    }
}

void update_user_list() {
    werase(user_win);
    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (chat_room->clients[i].active) {
            wprintw(user_win, "%s\n", chat_room->clients[i].name);
        }
    }
    wrefresh(user_win);
}

void shutdown_client(void) {
    delwin(chat_win);
    delwin(user_win);
    delwin(input_win);
    delwin(wnd);
    endwin();
    shmdt(chat_room);
    puts("Client has shut down");
}

void sigint_handler(int sig) { exit(0); }

void login_user(WINDOW *win, char *name) {
    WINDOW *wnd;
    WINDOW *subwnd;
    int symbol;
    int index = 0;

    wnd = derwin(win, 7, 30, (size.ws_row / 2) - 3, (size.ws_col / 2) - 15);
    box(wnd, '|', '-');
    subwnd = derwin(wnd, 1, 28, 3, 1);
    wmove(wnd, 1, 8);
    wprintw(wnd, "Enter user name");
    wmove(wnd, 2, 1);
    whline(wnd, '*', 28);
    wmove(wnd, 4, 1);
    whline(wnd, '*', 28);
    wmove(wnd, 5, 2);
    wprintw(wnd, "Press ENTER to confirm");

    wrefresh(wnd);

    keypad(subwnd, true);
    while (1) {
        symbol = wgetch(subwnd);
        if ('\n' == symbol) {
            break;
        } else if (127 > symbol && 32 <= symbol) {
            if (index >= MAX_NAME_LEN) break;
            wprintw(subwnd, "%c", symbol);
            name[index] = symbol;
            index++;
        }
    }
    if (0 == index) {
        snprintf(name, MAX_NAME_LEN, "user");
    }

    wclear(wnd);
    wrefresh(wnd);
    delwin(subwnd);
    delwin(wnd);
}

int main() {
    key_t key = ftok("chatroom", 65);
    shm_id = shmget(key, SHM_SIZE, 0666);
    chat_room = (ChatRoom *)shmat(shm_id, (void *)0, 0);

    struct sigaction sa;
    sa.sa_handler = sigint_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    sigaction(SIGINT, &sa, NULL);

    initscr();
    signal(SIGWINCH, sig_winch);
    ioctl(fileno(stdout), TIOCGWINSZ, (char *)&size);

    cbreak();
    noecho();
    curs_set(1);
    start_color();
    init_pair(1, COLOR_WHITE, COLOR_BLACK);
    bkgd(COLOR_PAIR(1));
    refresh();

    wnd = newwin(size.ws_row, size.ws_col, 0, 0);
    chat_win = derwin(wnd, size.ws_row - 7, (size.ws_col / 4) * 3 - 1, 1, 1);
    user_win = derwin(wnd, size.ws_row - 2, (size.ws_col / 4) - 2, 1,
                      (size.ws_col / 4) * 3 + 1);
    input_win = derwin(wnd, 4, (size.ws_col / 4) * 3 - 1, size.ws_row - 5, 1);

    box(wnd, '|', '-');
    wmove(wnd, size.ws_row - 6, 1);
    whline(wnd, '-', (size.ws_col / 4) * 3 - 1);
    wmove(wnd, 1, (size.ws_col / 4) * 3);
    wvline(wnd, '|', size.ws_row - 2);

    wrefresh(wnd);

    atexit(shutdown_client);

    login_user(wnd, username);
    register_client(username);
    update_user_list();  // Initial update of the user list

    wprintw(
        chat_win,
        "Please wait while the client establishes connection for user '%s'\n",
        username);
    wrefresh(chat_win);

    pthread_t recv_thread;
    pthread_create(&recv_thread, NULL, receive_messages, NULL);

    int symbol;
    int index = 0;
    char msg[MAX_MSG_LEN];

    keypad(input_win, true);
    while (1) {
        werase(input_win);
        wrefresh(input_win);
        wmove(input_win, 0, 0);
        index = 0;
        memset(msg, 0, MAX_MSG_LEN);

        while (1) {
            symbol = wgetch(input_win);
            if ('\n' == symbol) {
                if (index == 0) break;
                send_message(msg);
                update_user_list();  // Update the user list after sending a
                                     // message
                break;
            } else if (symbol >= 32 && symbol < 127) {
                if (index >= MAX_MSG_LEN) break;
                wprintw(input_win, "%c", symbol);
                msg[index] = symbol;
                index++;
            }
        }
        wmove(input_win, 0, 0);  // Move cursor back to input field
        wrefresh(input_win);
    }

    pthread_join(recv_thread, NULL);
    endwin();
    return 0;
}
