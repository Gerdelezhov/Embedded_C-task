#include <stdio.h>
#include <sys/wait.h>
#include <unistd.h>

void create_process(int child_processes, int parent_pid) {
    pid_t pid;

    for (int i = 0; i < child_processes; ++i) {
        pid = fork();

        if (pid == 0) {
            printf("PID: %d, PPID: %d\n", getpid(), parent_pid);
            return;
        } else if (pid < 0) {
            fprintf(stderr, "Failed to fork\n");
            return;
        }
    }

    for (int i = 0; i < child_processes; ++i) {
        wait(NULL);
    }
}

int main() {
    pid_t pid = getpid();

    printf("Главный PID: %d, PPID: %d\n", pid, getppid());

    pid_t pid1 = fork();

    if (pid1 == 0) {  // процесс 1
        printf("1 PID: %d, PPID: %d\n", getpid(), getppid());
        create_process(2, getpid());  // создание процессов 3 и 4
    } else if (pid1 > 0) {
        pid_t pid2 = fork();

        if (pid2 == 0) {  // процесс 2
            printf("2 PID: %d, PPID: %d\n", getpid(), getppid());
            create_process(1, getpid());  // создание процесса 5
        } else if (pid2 < 0) {
            fprintf(stderr, "Failed to fork\n");
            return 1;
        }

        wait(NULL);
    } else {
        fprintf(stderr, "Failed to fork\n");
        return 1;
    }

    return 0;
}
