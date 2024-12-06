#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "/home/peclinux/Trabalhão/trabalhoFinal/so.h"

#define MATRIX_SIZE 2000
#define NUM_THREADS 4

int A[MATRIX_SIZE][MATRIX_SIZE];
int B[MATRIX_SIZE][MATRIX_SIZE];
int C[MATRIX_SIZE][MATRIX_SIZE];
Barrier barrier;

void *multiply_section(void *arg) {
    int thread_id = *(int *)arg;
    int start = thread_id * (MATRIX_SIZE / NUM_THREADS);
    int end = (thread_id + 1) * (MATRIX_SIZE / NUM_THREADS);

    for (int i = start; i < end; i++) {
        for (int j = 0; j < MATRIX_SIZE; j++) {
            C[i][j] = 0;
            for (int k = 0; k < MATRIX_SIZE; k++) {
                C[i][j] += A[i][k] * B[k][j];
            }
        }
    }

    barrier_wait(&barrier); // Sincroniza as threads
    return NULL;
}

int main() {
    pthread_t threads[NUM_THREADS];
    int ids[NUM_THREADS];

    // Inicializa matrizes A e B com valores aleatórios
    for (int i = 0; i < MATRIX_SIZE; i++) {
        for (int j = 0; j < MATRIX_SIZE; j++) {
            A[i][j] = rand() % 10;
            B[i][j] = rand() % 10;
        }
    }

    // Inicializa a barreira
    init_barrier(&barrier, NUM_THREADS);

    // Cria threads
    for (int i = 0; i < NUM_THREADS; i++) {
        ids[i] = i;
        if (pthread_create(&threads[i], NULL, multiply_section, &ids[i]) != 0) {
            perror("Erro ao criar thread");
            return 1;
        }
    }

    // Aguarda todas as threads
    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }

    // Exibe o resultado
    printf("Resultado da multiplicação:\n");
    for (int i = 0; i < MATRIX_SIZE; i++) {
        for (int j = 0; j < MATRIX_SIZE; j++) {
            printf("%d ", C[i][j]);
        }
        printf("\n");
    }

    return 0;
}
