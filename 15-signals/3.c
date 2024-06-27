#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main() {
    sigset_t sigset;
    int sig;

    sigemptyset(&sigset);
    sigaddset(&sigset, SIGUSR1);

    if (sigprocmask(SIG_BLOCK, &sigset, NULL) == -1) {
        perror("sigprocmask");
        exit(EXIT_FAILURE);
    }

    printf("Ожидание SIGUSR1.. (PID: %d)\n", getpid());
    while (1) {
        if (sigwait(&sigset, &sig) == 0) {
            if (sig == SIGUSR1) {
                printf("Получен сигнал SIGUSR1\n");
            }
        } else {
            perror("sigwait");
            exit(EXIT_FAILURE);
        }
    }

    return 0;
}
