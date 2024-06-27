#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main() {
    sigset_t sigset;
    sigemptyset(&sigset);
    sigaddset(&sigset, SIGINT);

    if (sigprocmask(SIG_BLOCK, &sigset, NULL) == -1) {
        perror("sigprocmask");
        exit(EXIT_FAILURE);
    }

    printf(
        "SIGINT заблокирован. Нажатие Ctrl+C, не прекратит выплнение "
        "программы. (PID: %d)\n",
        getpid());
    while (1) {
        sleep(1);  // Или любое другое действие
    }

    return 0;
}
