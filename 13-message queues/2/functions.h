#include <curses.h>
#include <errno.h>
#include <mqueue.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/msg.h>
#include <termios.h>

#define USER_PRIORITY 1
#define SERVER_PRIORYTY 2
#define MSG_PRIORYTY 3
#define USRNAME_PRIORITY 4
#define UPDUSER_PRIORITY 5
#define REQUSR_PRIORITY 6
#define DISCONNECT_PRIORITY 7

#define PERM 0666
#define LOGIN_NAME "/login"
#define USER_COUNT 10
#define LOGIN_MAX 5
#define LOGIN_SIZE 80
#define USER_MAX 5
#define USER_SIZE 1024

struct user_t {
    int id;
    mqd_t rcv_q;
    mqd_t snd_q;
    char login[LOGIN_SIZE];
    char stc_ch[LOGIN_SIZE];
    char cts_ch[LOGIN_SIZE];
    char rcv_buf[USER_SIZE];
    char snd_buf[USER_SIZE];
};

struct q_handler_t {
    mqd_t rcv_q;
    WINDOW *wnd;
    WINDOW *main_wnd;
    WINDOW *users_wnd;
    WINDOW *msg_wnd;
    char buf[USER_SIZE];
};

int server();
int client();
