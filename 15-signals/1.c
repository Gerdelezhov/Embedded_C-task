#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

void sigusr1_handler(int signum) { printf("Received SIGUSR1 signal\n"); }

int main() {
    struct sigaction sa;
    sa.sa_handler = sigusr1_handler;
    sa.sa_flags = 0;
    sigemptyset(&sa.sa_mask);

    if (sigaction(SIGUSR1, &sa, NULL) == -1) {
        perror("sigaction");
        exit(EXIT_FAILURE);
    }

    printf("Ожидание SIGUSR1.. (PID: %d)\n", getpid());
    while (1) {
        pause();  // Ожидание сигнала
    }

    return 0;
}
