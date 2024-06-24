#include "functions.h"

struct user_t *users = NULL;
int users_count = 0;
int active_users = 0;
mqd_t login_q = 0;

static void sigint_handler(int sig, siginfo_t *si, void *unused) { exit(0); }

void shutdown_server(void) {
    int index;

    if (login_q > 0) {
        mq_close(login_q);
        mq_unlink(LOGIN_NAME);
    }

    if (users != NULL) {
        if (active_users != 0) {
            for (index = 0; index < users_count; ++index) {
                if (users[index].id == -1) continue;

                mq_send(users[index].snd_q, "0", 1, DISCONNECT_PRIORITY);

                mq_close(users[index].rcv_q);
                mq_close(users[index].snd_q);
                mq_unlink(users[index].cts_ch);
                mq_unlink(users[index].stc_ch);
            }
        }

        free(users);
    }

    puts("Сервер завершил работу");
}

int server() {
    struct sigaction sa;
    char users_list[USER_SIZE];
    char tmp_buf[USER_SIZE];
    int users_num = USER_COUNT;
    struct mq_attr login_attr;
    struct mq_attr user_attr;
    unsigned int prio;
    int index;
    ssize_t ret;

    users = malloc(sizeof(struct user_t) * users_num);

    login_attr.mq_maxmsg = LOGIN_MAX;
    login_attr.mq_msgsize = LOGIN_SIZE;
    user_attr.mq_maxmsg = USER_MAX;
    user_attr.mq_msgsize = USER_SIZE;

    memset(users_list, 0, USER_SIZE);

    sigfillset(&sa.sa_mask);
    sa.sa_sigaction = sigint_handler;
    if (sigaction(SIGINT, &sa, NULL) == -1) {
        perror("sigaction");
        exit(1);
    }

    atexit(shutdown_server);

    login_q =
        mq_open(LOGIN_NAME, O_CREAT | O_RDWR | O_NONBLOCK, PERM, &login_attr);
    if (login_q == -1) {
        perror("login_q mq_open");
        exit(1);
    }

    while (1) {
        if ((ret = mq_receive(login_q, tmp_buf, LOGIN_SIZE, &prio)) == -1) {
            if (errno != EAGAIN) {
                perror("login_q mq_receive");
                exit(1);
            }
        } else if (ret > 0) {
            if (SERVER_PRIORYTY == prio) {
                mq_send(login_q, tmp_buf, LOGIN_SIZE, prio);
                memset(tmp_buf, 0, LOGIN_SIZE);
            } else if (USER_PRIORITY == prio) {
                if (users_count >= users_num) {
                    users_num += USER_COUNT;
                    struct user_t *tmp =
                        realloc(users, sizeof(struct user_t) * users_num);
                    if (tmp == NULL) {
                        perror("realloc");
                        exit(1);
                    }
                    users = tmp;
                }
                users_count++;
                active_users++;

                users[users_count - 1].id = users_count;

                snprintf(users[users_count - 1].login, LOGIN_SIZE,
                         "Пользователь%d", users[users_count - 1].id);
                snprintf(users[users_count - 1].stc_ch, LOGIN_SIZE,
                         "/client%d_stoc", users[users_count - 1].id);
                snprintf(users[users_count - 1].cts_ch, LOGIN_SIZE,
                         "/client%d_ctos", users[users_count - 1].id);

                printf(
                    "Зарегистрирован \"%s\" как пользователь #%d с %s и %s "
                    "каналами\n",
                    users[users_count - 1].login, users[users_count - 1].id,
                    users[users_count - 1].stc_ch,
                    users[users_count - 1].cts_ch);

                users[users_count - 1].rcv_q =
                    mq_open(users[users_count - 1].cts_ch,
                            O_CREAT | O_RDONLY | O_NONBLOCK, PERM, &user_attr);
                if (users[users_count - 1].rcv_q == -1) {
                    perror("rcv_q mq_open");
                    exit(1);
                }
                users[users_count - 1].snd_q =
                    mq_open(users[users_count - 1].stc_ch,
                            O_CREAT | O_WRONLY | O_NONBLOCK, PERM, &user_attr);
                if (users[users_count - 1].snd_q == -1) {
                    perror("snd_q mq_open");
                    exit(1);
                }

                snprintf(tmp_buf, LOGIN_SIZE, "%d", users[users_count - 1].id);
                mq_send(login_q, tmp_buf, LOGIN_SIZE, SERVER_PRIORYTY);

                snprintf(tmp_buf, USER_SIZE, "<%s>\n",
                         users[users_count - 1].login);
                strncat(users_list, tmp_buf,
                        USER_SIZE - strlen(users_list) - 1);
                for (index = 0; index < users_count; ++index) {
                    if (users[index].id == -1) continue;
                    mq_send(users[index].snd_q, users_list, USER_SIZE,
                            UPDUSER_PRIORITY);
                }
            }
        }

        for (index = 0; index < users_count; ++index) {
            if (users[index].id == -1) continue;
            if ((ret = mq_receive(users[index].rcv_q, users[index].rcv_buf,
                                  USER_SIZE, &prio)) == -1) {
                if (errno != EAGAIN) {
                    perror("rcv_q mq_receive");
                    exit(1);
                }
            } else if (ret > 0) {
                if (USER_PRIORITY == prio) {
                    printf("Получено: <%s> от пользователя \"%s\"\n",
                           users[index].rcv_buf, users[index].login);

                    snprintf(tmp_buf, USER_SIZE, "%s: %s", users[index].login,
                             users[index].rcv_buf);
                    for (int sec_index = 0; sec_index < users_count;
                         ++sec_index) {
                        if (users[sec_index].id != -1) {
                            mq_send(users[sec_index].snd_q, tmp_buf, USER_SIZE,
                                    MSG_PRIORYTY);
                        }
                    }
                } else if (USRNAME_PRIORITY == prio) {
                    strncpy(users[index].login, users[index].rcv_buf,
                            LOGIN_SIZE - 1);
                    users[index].login[LOGIN_SIZE - 1] = '\0';

                    printf("Пользователь #%d переменован: <%s>\n",
                           users[index].id, users[index].login);

                    memset(users_list, 0, USER_SIZE);
                    for (int sec_index = 0; sec_index < users_count;
                         ++sec_index) {
                        if (users[sec_index].id == -1) continue;
                        snprintf(tmp_buf, USER_SIZE, "<%s>\n",
                                 users[sec_index].login);
                        strncat(users_list, tmp_buf,
                                USER_SIZE - strlen(users_list) - 1);
                    }
                    for (int sec_index = 0; sec_index < users_count;
                         ++sec_index) {
                        if (users[sec_index].id == -1) continue;
                        mq_send(users[sec_index].snd_q, users_list, USER_SIZE,
                                UPDUSER_PRIORITY);
                    }
                } else if (REQUSR_PRIORITY == prio) {
                    mq_send(users[index].snd_q, users_list, USER_SIZE,
                            UPDUSER_PRIORITY);
                } else if (DISCONNECT_PRIORITY == prio) {
                    printf("Пользователь %d %s отключился\n", users[index].id,
                           users[index].login);

                    mq_close(users[index].rcv_q);
                    mq_close(users[index].snd_q);
                    mq_unlink(users[index].cts_ch);
                    mq_unlink(users[index].stc_ch);

                    users[index].id = -1;
                    active_users--;

                    if (active_users <= 0) {
                        exit(0);
                    }

                    memset(users_list, 0, USER_SIZE);
                    for (int sec_index = 0; sec_index < users_count;
                         ++sec_index) {
                        if (users[sec_index].id == -1) continue;
                        snprintf(tmp_buf, USER_SIZE, "<%s>\n",
                                 users[sec_index].login);
                        strncat(users_list, tmp_buf,
                                USER_SIZE - strlen(users_list) - 1);
                    }

                    for (int sec_index = 0; sec_index < users_count;
                         ++sec_index) {
                        if (users[sec_index].id == -1) continue;
                        mq_send(users[sec_index].snd_q, users_list, USER_SIZE,
                                UPDUSER_PRIORITY);
                    }
                }
            }
        }
    }

    return 0;
}

int main(int argc, char const *argv[]) {
    server();

    exit(0);
}
