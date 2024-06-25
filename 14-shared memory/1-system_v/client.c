#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h>

#define SHM_SIZE 4096
#define SHM_KEY 1234

int main() {
    int shm_id = shmget(SHM_KEY, SHM_SIZE, 0666);
    if (shm_id == -1) {
        perror("Error opening");
        exit(EXIT_FAILURE);
    }

    void *shm_ptr = shmat(shm_id, NULL, 0);
    if (shm_ptr == (void *)-1) {
        perror("Error maping");
        exit(EXIT_FAILURE);
    }

    printf("Client: Received message from server: %s\n", (char *)shm_ptr);

    char *response = "Hello!";
    memcpy(shm_ptr, response, strlen(response) + 1);

    if (shmdt(shm_ptr) == -1) {
        perror("Error unmaping");
        exit(EXIT_FAILURE);
    }

    return 0;
}
