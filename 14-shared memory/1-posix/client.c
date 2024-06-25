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
    int shm_fd = shm_open(SHM_NAME, O_RDWR, 0666);
    if (shm_fd == -1) {
        perror("Error opening shm");
        exit(EXIT_FAILURE);
    }
    void *shm_ptr =
        mmap(0, SHM_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if (shm_ptr == MAP_FAILED) {
        perror("Error maping");
        exit(EXIT_FAILURE);
    }
    printf("Client: Received message from server: %s\n", (char *)shm_ptr);

    char *response = "Hello!";
    memcpy(shm_ptr, response, strlen(response) + 1);

    if (munmap(shm_ptr, SHM_SIZE) == -1) {
        perror("Error unmaping");
        exit(EXIT_FAILURE);
    }
    close(shm_fd);

    return 0;
}
