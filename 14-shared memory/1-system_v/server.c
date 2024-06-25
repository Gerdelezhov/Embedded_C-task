#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h>

#define SHM_SIZE 4096
#define SHM_KEY 1234

int main() {
    int shm_id = shmget(SHM_KEY, SHM_SIZE, IPC_CREAT | 0666);
    if (shm_id == -1) {
        perror("Error opening shm");
        exit(EXIT_FAILURE);
    }
    void *shm_ptr = shmat(shm_id, NULL, 0);
    if (shm_ptr == (void *)-1) {
        perror("Error maping");
        exit(EXIT_FAILURE);
    }
    char *message = "Hi!";
    memcpy(shm_ptr, message, strlen(message) + 1);

    while (strcmp((char *)shm_ptr, "Hi!") == 0) {
        sleep(1);
    }
    printf("Server: Received message from client: %s\n", (char *)shm_ptr);

    if (shmdt(shm_ptr) == -1) {
        perror("Error unmaping");
        exit(EXIT_FAILURE);
    }

    if (shmctl(shm_id, IPC_RMID, NULL) == -1) {
        perror("Error unlinking");
        exit(EXIT_FAILURE);
    }

    return 0;
}
