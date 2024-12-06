#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "/home/peclinux/Trabalhão/trabalhoFinal/so.h"

#define BUFFER_SIZE 5
#define NUM_PRODUCERS 1
#define NUM_CONSUMERS 1
#define NUM_ITEMS 2

int buffer[BUFFER_SIZE];
int in = 0, out = 0;

Semaphore empty_slots;
Semaphore full_slots;
Mutex buffer_mutex;

void *producer(void *arg) {
    int id = *(int *)arg;
    for (int i = 0; i < NUM_ITEMS; i++) {
        int item = rand() % 100; // Produz um item aleatório

        wait_semaphore(&empty_slots);
        lock_mutex(&buffer_mutex);

        buffer[in] = item;
        printf("Produtor %d produziu: %d\n", id, item);
        in = (in + 1) % BUFFER_SIZE;

        unlock_mutex(&buffer_mutex);
        signal_semaphore(&full_slots);
    }
    return NULL;
}

void *consumer(void *arg) {
    int id = *(int *)arg;
    for (int i = 0; i < NUM_ITEMS; i++) {
        wait_semaphore(&full_slots);
        lock_mutex(&buffer_mutex);

        int item = buffer[out];
        printf("Consumidor %d consumiu: %d\n", id, item);
        out = (out + 1) % BUFFER_SIZE;

        unlock_mutex(&buffer_mutex);
        signal_semaphore(&empty_slots);
    }
    return NULL;
}

int main() {
    pthread_t producers[NUM_PRODUCERS], consumers[NUM_CONSUMERS];
    int ids[NUM_PRODUCERS + NUM_CONSUMERS];

    // Inicializa os semáforos e mutex
    init_semaphore(&empty_slots, BUFFER_SIZE);
    init_semaphore(&full_slots, 0);
    init_mutex(&buffer_mutex);

    // Cria threads produtoras
    for (int i = 0; i < NUM_PRODUCERS; i++) {
        ids[i] = i;
        pthread_create(&producers[i], NULL, producer, &ids[i]);
    }

    // Cria threads consumidoras
    for (int i = 0; i < NUM_CONSUMERS; i++) {
        ids[NUM_PRODUCERS + i] = i;
        pthread_create(&consumers[i], NULL, consumer, &ids[NUM_PRODUCERS + i]);
    }

    // Aguarda threads
    for (int i = 0; i < NUM_PRODUCERS; i++) {
        pthread_join(producers[i], NULL);
    }
    for (int i = 0; i < NUM_CONSUMERS; i++) {
        pthread_join(consumers[i], NULL);
    }

    return 0;
}
