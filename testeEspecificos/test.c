#include <stdio.h>
#include <pthread.h>
#include "/home/peclinux/Trabalhão/trabalhoFinal/so.h" // Biblioteca personalizada

Mutex mutex;
int shared_counter = 0;

void *mutex_test_thread(void *arg) {
    for (int i = 0; i < 100000; i++) {
        lock_mutex(&mutex);
        shared_counter++;
        unlock_mutex(&mutex);
    }
    return NULL;
}

void test_mutex() {
    printf("Iniciando teste de Mutex...\n");
    init_mutex(&mutex);

    pthread_t t1, t2;
    pthread_create(&t1, NULL, mutex_test_thread, NULL);
    pthread_create(&t2, NULL, mutex_test_thread, NULL);

    pthread_join(t1, NULL);
    pthread_join(t2, NULL);

    printf("Valor final do contador (esperado: 200000): %d\n", shared_counter);
}

Semaphore sem;

void *semaphore_test_thread(void *arg) {
    int thread_id = *(int *)arg;
    wait_semaphore(&sem);
    printf("Thread %d entrou na região crítica.\n", thread_id);
    sched_yield(); // Simula algum trabalho na região crítica
    signal_semaphore(&sem);
    return NULL;
}

void test_semaphore() {
    printf("Iniciando teste de Semáforo...\n");
    init_semaphore(&sem, 2); // Permite até 2 threads simultaneamente

    pthread_t threads[5];
    int thread_ids[5];
    for (int i = 0; i < 5; i++) {
        thread_ids[i] = i + 1;
        pthread_create(&threads[i], NULL, semaphore_test_thread, &thread_ids[i]);
    }

    for (int i = 0; i < 5; i++) {
        pthread_join(threads[i], NULL);
    }

    printf("Teste de Semáforo concluído.\n");
}

Cond cond;
Mutex cond_mutex;
int ready = 0;

void *producer(void *arg) {
    lock_mutex(&cond_mutex);
    printf("Produtor: preparando recurso...\n");
    ready = 1;
    signal_cond(&cond);
    unlock_mutex(&cond_mutex);
    return NULL;
}

void *consumer(void *arg) {
    lock_mutex(&cond_mutex);
    while (!ready) {
        printf("Consumidor: aguardando recurso...\n");
        wait_cond(&cond, &cond_mutex);
    }
    printf("Consumidor: recurso disponível!\n");
    unlock_mutex(&cond_mutex);
    return NULL;
}

void test_cond() {
    printf("Iniciando teste de Variável de Condição...\n");
    init_cond(&cond);
    init_mutex(&cond_mutex);

    pthread_t prod, cons;
    pthread_create(&cons, NULL, consumer, NULL);
    pthread_create(&prod, NULL, producer, NULL);

    pthread_join(prod, NULL);
    pthread_join(cons, NULL);

    printf("Teste de Variável de Condição concluído.\n");
}

Barrier barrier;

void *barrier_test_thread(void *arg) {
    int thread_id = *(int *)arg;
    printf("Thread %d chegou na barreira.\n", thread_id);
    barrier_wait(&barrier);
    printf("Thread %d passou da barreira.\n", thread_id);
    return NULL;
}

void test_barrier() {
    printf("Iniciando teste de Barreira...\n");
    init_barrier(&barrier, 3); // Espera 3 threads

    pthread_t threads[3];
    int thread_ids[3];
    for (int i = 0; i < 3; i++) {
        thread_ids[i] = i + 1;
        pthread_create(&threads[i], NULL, barrier_test_thread, &thread_ids[i]);
    }

    for (int i = 0; i < 3; i++) {
        pthread_join(threads[i], NULL);
    }

    printf("Teste de Barreira concluído.\n");
}

RWLock rwlock;

void *reader(void *arg) {
    int thread_id = *(int *)arg;
    r_lock(&rwlock);
    printf("Leitor %d está lendo.\n", thread_id);
    sched_yield(); // Simula trabalho
    printf("Leitor %d terminou de ler.\n", thread_id);
    r_unlock(&rwlock);
    return NULL;
}

void *writer(void *arg) {
    int thread_id = *(int *)arg;
    w_lock(&rwlock);
    printf("Escritor %d está escrevendo.\n", thread_id);
    sched_yield(); // Simula trabalho
    printf("Escritor %d terminou de escrever.\n", thread_id);
    w_unlock(&rwlock);
    return NULL;
}

void test_rwlock() {
    printf("Iniciando teste de RWLock...\n");
    init_rwlock(&rwlock);

    pthread_t threads[5];
    int thread_ids[5];

    // 3 leitores e 2 escritores
    for (int i = 0; i < 3; i++) {
        thread_ids[i] = i + 1;
        pthread_create(&threads[i], NULL, reader, &thread_ids[i]);
    }
    for (int i = 3; i < 5; i++) {
        thread_ids[i] = i + 1;
        pthread_create(&threads[i], NULL, writer, &thread_ids[i]);
    }

    for (int i = 0; i < 5; i++) {
        pthread_join(threads[i], NULL);
    }

    printf("Teste de RWLock concluído.\n");
}

int main() {
    test_mutex();
    test_semaphore();
    test_cond();
    test_barrier();
    test_rwlock();
    return 0;
}
