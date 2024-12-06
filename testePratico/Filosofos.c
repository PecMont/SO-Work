#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h> // Necessário para usleep
#include "/home/peclinux/Trabalhão/trabalhoFinal/so.h"

#define NUM_PHILOSOPHERS 5
#define NUM_MEALS 3

Mutex forks[NUM_PHILOSOPHERS];

void *philosopher(void *arg) {
    int id = *(int *)arg;
    int left = id;
    int right = (id + 1) % NUM_PHILOSOPHERS;

    for (int i = 0; i < NUM_MEALS; i++) {
        printf("Filósofo %d está pensando.\n", id);

        // Pega os garfos
        lock_mutex(&forks[left]);
        printf("Filósofo %d pegou o garfo esquerdo.\n", id);
        lock_mutex(&forks[right]);
        printf("Filósofo %d pegou o garfo direito.\n", id);

        // Come
        printf("Filósofo %d está comendo.\n", id);
        usleep(1000 * (rand() % 100)); // Simula o tempo comendo

        // Larga os garfos
        unlock_mutex(&forks[right]);
        unlock_mutex(&forks[left]);
        printf("Filósofo %d largou os garfos.\n", id);
    }

    return NULL;
}

int main() {
    pthread_t philosophers[NUM_PHILOSOPHERS];
    int ids[NUM_PHILOSOPHERS];

    // Inicializa os mutexes (garfos)
    for (int i = 0; i < NUM_PHILOSOPHERS; i++) {
        init_mutex(&forks[i]);
    }

    // Cria threads para os filósofos
    for (int i = 0; i < NUM_PHILOSOPHERS; i++) {
        ids[i] = i;
        if (pthread_create(&philosophers[i], NULL, philosopher, &ids[i]) != 0) {
            perror("Erro ao criar thread");
            return 1;
        }
    }

    // Aguarda todos os filósofos terminarem
    for (int i = 0; i < NUM_PHILOSOPHERS; i++) {
        pthread_join(philosophers[i], NULL);
    }

    printf("Todos os filósofos terminaram.\n");
    return 0;
}