#include "functions.h"

struct winsize size;
mqd_t login_q;
struct user_t user;
struct q_handler_t q_handler;

int main(void) {
    client();
    return 0;
}

void sig_winch(int signo) {
    ioctl(fileno(stdout), TIOCGWINSZ, (char *)&size);
    resizeterm(size.ws_row, size.ws_col);
}

static void sigint_handler(int sig, siginfo_t *si, void *unused) { exit(0); }

static void handle_msg(union sigval sv) {
    struct q_handler_t *q_handler = ((struct q_handler_t *)sv.sival_ptr);
    struct sigevent sev;
    int prio;
    ssize_t ret;
    sev.sigev_notify = SIGEV_THREAD;
    sev.sigev_notify_function = handle_msg;
    sev.sigev_notify_attributes = NULL;
    sev.sigev_value.sival_ptr = q_handler;

    while (ret = mq_receive(q_handler->rcv_q, q_handler->buf, USER_SIZE,
                            &prio) > 0) {
        if (MSG_PRIORYTY == prio) {
            wprintw(q_handler->main_wnd, "%s\n", q_handler->buf);
            wrefresh(q_handler->main_wnd);
        }

        else if (UPDUSER_PRIORITY == prio) {
            wclear(q_handler->users_wnd);
            wprintw(q_handler->users_wnd, "%s", q_handler->buf);
            wrefresh(q_handler->users_wnd);
        }

        else if (DISCONNECT_PRIORITY == prio) {
            exit(0);
        }
    }
    if (ret == -1) {
        if (errno != EAGAIN) {
            printf("handle_msg mq_receive");
            exit(1);
        }
    }

    while (ret = mq_notify(q_handler->rcv_q, &sev) == -1) {
        printf("rcv_q mq_notify");
        exit(1);
    }
}

void shutdown_client(void) {
    delwin(q_handler.main_wnd);
    delwin(q_handler.users_wnd);
    delwin(q_handler.msg_wnd);
    delwin(q_handler.wnd);
    endwin();

    if (login_q > 0) {
        mq_close(login_q);
    }

    if (user.snd_q > 0) {
        mq_send(user.snd_q, "0", USER_SIZE, DISCONNECT_PRIORITY);
        mq_close(user.snd_q);
    }

    if (user.rcv_q) {
        mq_close(user.rcv_q);
    }

    puts("Клиент завершил работу");
}

int _login_user(WINDOW **pwnd, char *name) {
    WINDOW *wnd;
    WINDOW *subwnd;
    int symbol;
    int index = 0;

    wnd = derwin(*pwnd, 7, 30, (size.ws_row / 2) - 3, (size.ws_col / 2) - 11);
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
            if (LOGIN_SIZE < index) break;
            wprintw(subwnd, "%c", symbol);
            name[index] = symbol;
            index++;
        }
    }
    if (0 == index) {
        snprintf(name, LOGIN_SIZE, "user");
    }

    wclear(wnd);
    wrefresh(wnd);
    delwin(subwnd);
    delwin(wnd);

    return 0;
}

int client() {
    struct sigevent sev;
    struct sigaction sa;
    char tmp_buf[USER_SIZE];
    int symbol;
    int index = 0;
    int prio;
    ssize_t ret;

    memset(tmp_buf, 0, USER_SIZE);
    memset(q_handler.buf, 0, USER_SIZE);

    sev.sigev_notify = SIGEV_THREAD;
    sev.sigev_notify_function = handle_msg;
    sev.sigev_notify_attributes = NULL;
    sev.sigev_value.sival_ptr = &q_handler;

    atexit(shutdown_client);

    sigemptyset(&sa.sa_mask);
    sa.sa_sigaction = sigint_handler;
    if (sigaction(SIGINT, &sa, NULL) == -1) {
        printf("sigaction");
        exit(1);
    }

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

    q_handler.wnd = newwin(size.ws_row, size.ws_col, 0, 0);
    q_handler.main_wnd = derwin(q_handler.wnd, (size.ws_row - 7),
                                (size.ws_col / 4) * 3 - 1, 1, 1);
    q_handler.users_wnd =
        derwin(q_handler.wnd, size.ws_row - 2, (size.ws_col / 4) - 2, 1,
               (size.ws_col / 4) * 3 + 1);
    q_handler.msg_wnd = derwin(q_handler.wnd, 4, (size.ws_col / 4) * 3 - 1,
                               (size.ws_row - 5), 1);

    box(q_handler.wnd, '|', '-');
    wmove(q_handler.wnd, (size.ws_row - 6), 1);
    whline(q_handler.wnd, '-', (size.ws_col / 4) * 3 - 1);
    wmove(q_handler.wnd, 1, (size.ws_col / 4) * 3);
    wvline(q_handler.wnd, '|', size.ws_row - 2);

    wrefresh(q_handler.wnd);

    _login_user(&q_handler.wnd, user.login);

    wprintw(
        q_handler.main_wnd,
        "Please wait while the client establishes connection with server for "
        "user '%s'\n",
        user.login);
    wrefresh(q_handler.main_wnd);
    login_q = mq_open(LOGIN_NAME, O_RDWR);
    while ((login_q = mq_open(LOGIN_NAME, O_RDWR)) == -1) {
        if (errno != ENOENT) {
            printf("login_q mq_open");
            exit(1);
        }
    }

    while (mq_send(login_q, "0", LOGIN_SIZE, USER_PRIORITY) == -1) {
        if (errno != EAGAIN) {
            printf("login_q mq_send");
            exit(1);
        }
    }

    while (1) {
        if (mq_receive(login_q, tmp_buf, LOGIN_SIZE, &prio) == -1) {
            printf("mq_receive");
        } else {
            if (USER_PRIORITY == prio) {
                mq_send(login_q, tmp_buf, LOGIN_SIZE, prio);
            }

            else if (SERVER_PRIORYTY == prio) {
                user.id = atoi(tmp_buf);

                snprintf(user.cts_ch, LOGIN_SIZE, "/client%d_ctos", user.id);
                snprintf(user.stc_ch, LOGIN_SIZE, "/client%d_stoc", user.id);

                wclear(q_handler.main_wnd);
                wprintw(q_handler.main_wnd, "ID %d\n", user.id);
                wrefresh(q_handler.main_wnd);

                user.rcv_q = mq_open(user.stc_ch, O_RDONLY | O_NONBLOCK);
                if (-1 == user.rcv_q) {
                    printf("rcv_q mq_open");
                    exit(1);
                }
                user.snd_q = mq_open(user.cts_ch, O_WRONLY | O_NONBLOCK);
                if (-1 == user.snd_q) {
                    printf("snd_q mq_open");
                    exit(1);
                }

                q_handler.rcv_q = user.rcv_q;
                if (mq_notify(user.rcv_q, &sev) == -1) {
                    printf("rcv_q mq_notify");
                    exit(1);
                }

                ret = mq_send(user.snd_q, user.login, LOGIN_SIZE,
                              USRNAME_PRIORITY);
                if (ret == -1) {
                    if (errno != EAGAIN) {
                        printf("snd_q mq_send");
                        exit(1);
                    }
                }

                while (ret = mq_receive(user.rcv_q, user.rcv_buf, USER_SIZE,
                                        &prio) > 0) {
                    if (MSG_PRIORYTY == prio) {
                        wprintw(q_handler.main_wnd, "%s\n", q_handler.buf);
                        wrefresh(q_handler.main_wnd);
                    }

                    else if (UPDUSER_PRIORITY == prio) {
                        wclear(q_handler.users_wnd);
                        wprintw(q_handler.users_wnd, "%s", user.rcv_buf);
                        wrefresh(q_handler.users_wnd);
                        memset(user.rcv_buf, 0, USER_SIZE);
                    }

                    else if (DISCONNECT_PRIORITY == prio) {
                        exit(0);
                    }
                }
                if (ret == -1) {
                    if (errno != EAGAIN) {
                        printf("rcv_q mq_receive");
                        exit(1);
                    }
                }
                break;
            }
        }
    }

    keypad(q_handler.msg_wnd, true);
    while (1) {
        symbol = wgetch(q_handler.msg_wnd);
        if ('\n' == symbol) {
            if (0 == index) continue;

            wclear(q_handler.msg_wnd);
            wrefresh(q_handler.msg_wnd);

            mq_send(user.snd_q, user.snd_buf, USER_SIZE, USER_PRIORITY);

            index = 0;
            memset(user.snd_buf, 0, USER_SIZE);
        }

        else if (symbol >= 32 && symbol < 127) {
            if (80 < index) break;
            wprintw(q_handler.msg_wnd, "%c", (char)symbol);
            user.snd_buf[index] = (char)symbol;
            index++;
        } else if (KEY_F(3) == symbol) {
            break;
        }
    }

    return 0;
}
