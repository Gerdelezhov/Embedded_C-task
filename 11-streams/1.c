#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define NUM_SHOPS 5
#define NUM_CUSTOMERS 3
#define MAX_REQUEST 110000
#define MIN_REQUEST 90000
#define MAX_GOODS 11000
#define MIN_GOODS 9000

int shops[NUM_SHOPS];
pthread_mutex_t mutex_shops = PTHREAD_MUTEX_INITIALIZER;

void *customer(void *arg) {
    int *customer_id = (int *)arg;
    int request = (rand() % (MAX_REQUEST - MIN_REQUEST + 1)) + MIN_REQUEST;
    int goods = (rand() % (MAX_GOODS - MIN_GOODS + 1)) + MIN_GOODS;
    printf("Customer %d: Requested %d with goods %d\n", *customer_id, request,
           goods);

    while (request > 0) {
        pthread_mutex_lock(&mutex_shops);
        int chosen_shop = rand() % NUM_SHOPS;
        if (shops[chosen_shop] > 0) {
            int taken_goods =
                (request > shops[chosen_shop]) ? shops[chosen_shop] : request;
            shops[chosen_shop] -= taken_goods;
            request -= taken_goods;
            printf(
                "Customer %d bought %d from shop %d, remaining request: %d\n",
                *customer_id, taken_goods, chosen_shop, request);
        }
        pthread_mutex_unlock(&mutex_shops);
        sleep(2);
    }

    printf("Customer %d finished shopping.\n", *customer_id);
    pthread_exit(NULL);
}

void *loader(void *arg) {
    while (1) {
        pthread_mutex_lock(&mutex_shops);
        for (int i = 0; i < NUM_SHOPS; i++) {
            if (shops[i] == 0) {
                shops[i] = (rand() % (MAX_GOODS - MIN_GOODS + 1)) + MIN_GOODS;
                printf("Loader: Shop %d restocked with %d goods.\n", i,
                       shops[i]);
            }
        }
        pthread_mutex_unlock(&mutex_shops);
        sleep(5);
    }
}

int main() {
    pthread_t customers[NUM_CUSTOMERS];
    pthread_t loader_thread;

    srand(time(NULL));

    for (int i = 0; i < NUM_SHOPS; i++) {
        shops[i] = (rand() % (MAX_GOODS - MIN_GOODS + 1)) + MIN_GOODS;
    }

    int customer_ids[NUM_CUSTOMERS];
    for (int i = 0; i < NUM_CUSTOMERS; i++) {
        customer_ids[i] = i;
        pthread_create(&customers[i], NULL, customer, &customer_ids[i]);
    }

    pthread_create(&loader_thread, NULL, loader, NULL);

    for (int i = 0; i < NUM_CUSTOMERS; i++) {
        pthread_join(customers[i], NULL);
    }

    pthread_cancel(loader_thread);

    return 0;
}
