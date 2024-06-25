#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

#define SHM_NAME "/task_shm1"
#define SHM_SIZE 4096

int main() {
    int shm_fd = shm_open(SHM_NAME, O_CREAT | O_RDWR, 0666);
    if (shm_fd == -1) {
        perror("Error opening shm");
        exit(EXIT_FAILURE);
    }
    if (ftruncate(shm_fd, SHM_SIZE) == -1) {
        perror("Truncate error");
        exit(EXIT_FAILURE);
    }

    void *shm_ptr =
        mmap(0, SHM_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if (shm_ptr == MAP_FAILED) {
        perror("Error maping");
        exit(EXIT_FAILURE);
    }
    char *message = "Hi!";
    memcpy(shm_ptr, message, strlen(message) + 1);
    while (strcmp((char *)shm_ptr, "Hi!") == 0) {
        sleep(1);
    }
    // sleep(10);

    printf("Server: Received message from client: %s\n", (char *)shm_ptr);

    if (munmap(shm_ptr, SHM_SIZE) == -1) {
        perror("Error unmaping");
        exit(EXIT_FAILURE);
    }

    close(shm_fd);

    if (shm_unlink(SHM_NAME) == -1) {
        perror("Error unlinking shm");
        exit(EXIT_FAILURE);
    }

    return 0;
}
