#ifndef RWLOCK_H
#define RWLOCK_H

#include <stdlib.h>
#include <stdio.h>
#include <stdatomic.h>
#include <sched.h>


// ==================== MUTEX ====================

typedef struct {
    atomic_int locked;
} Mutex;

static inline void init_mutex(Mutex *mutex) {
    atomic_store(&(mutex->locked), 0);
}

static inline void lock_mutex(Mutex *mutex) {
    while (atomic_exchange(&(mutex->locked), 1)) {
        sched_yield(); // Evita busy-waiting contínuo
    }
}

static inline void unlock_mutex(Mutex *mutex) {
    atomic_store(&(mutex->locked), 0);
}

// ==================== SEMÁFORO ====================

typedef struct {
    atomic_int value;
    Mutex mutex;
} Semaphore;

static inline void init_semaphore(Semaphore *sem, int initial_value) {
    atomic_store(&(sem->value), initial_value);
    init_mutex(&(sem->mutex));
}

static inline void wait_semaphore(Semaphore *sem) {
    for (;;) {
        lock_mutex(&(sem->mutex));
        if (atomic_load(&(sem->value)) > 0) {
            atomic_fetch_sub(&(sem->value), 1);
            unlock_mutex(&(sem->mutex));
            return;
        }
        unlock_mutex(&(sem->mutex));
        sched_yield(); // Libera CPU para outras threads
    }
}

static inline void signal_semaphore(Semaphore *sem) {
    lock_mutex(&(sem->mutex));
    atomic_fetch_add(&(sem->value), 1);
    unlock_mutex(&(sem->mutex));
}

// ==================== VARIÁVEL DE CONDIÇÃO ====================

typedef struct {
    Semaphore sem;
    atomic_int count;
} Cond;

static inline void init_cond(Cond *cond) {
    init_semaphore(&(cond->sem), 0);
    atomic_store(&(cond->count), 0);
}

static inline void wait_cond(Cond *cond, Mutex *mutex) {
    atomic_fetch_add(&(cond->count), 1);
    unlock_mutex(mutex);
    wait_semaphore(&(cond->sem));
    lock_mutex(mutex);
}

static inline void signal_cond(Cond *cond) {
    if (atomic_load(&(cond->count)) > 0) {
        atomic_fetch_sub(&(cond->count), 1);
        signal_semaphore(&(cond->sem));
    }
}

static inline void cond_broadcast(Cond *cond) {
    while (atomic_load(&(cond->count)) > 0) {
        signal_cond(cond);
    }
}

// ==================== BARREIRA ====================

typedef struct {
    Semaphore sem;
    Mutex mutex;
    atomic_int count;
    int n;
} Barrier;

static inline void init_barrier(Barrier *barrier, int n) {
    init_semaphore(&(barrier->sem), 0);
    init_mutex(&(barrier->mutex));
    atomic_store(&(barrier->count), 0);
    barrier->n = n;
}

static inline void barrier_wait(Barrier *barrier) {
    lock_mutex(&(barrier->mutex));
    if (atomic_fetch_add(&(barrier->count), 1) == barrier->n - 1) {
        atomic_store(&(barrier->count), 0);
        for (int i = 0; i < barrier->n - 1; i++) {
            signal_semaphore(&(barrier->sem));
        }
        unlock_mutex(&(barrier->mutex));
    } else {
        unlock_mutex(&(barrier->mutex));
        wait_semaphore(&(barrier->sem));
    }
}

// ==================== READ-WRITE LOCK ====================

typedef struct {
    Mutex mutex;
    Semaphore w;
    atomic_int read_count;
} RWLock;

static inline void init_rwlock(RWLock *rwlock) {
    init_mutex(&(rwlock->mutex));
    init_semaphore(&(rwlock->w), 1);
    atomic_store(&(rwlock->read_count), 0);
}

static inline void r_lock(RWLock *rwlock) {
    lock_mutex(&(rwlock->mutex));
    if (atomic_fetch_add(&(rwlock->read_count), 1) == 0) {
        wait_semaphore(&(rwlock->w));
    }
    unlock_mutex(&(rwlock->mutex));
}

static inline void r_unlock(RWLock *rwlock) {
    lock_mutex(&(rwlock->mutex));
    if (atomic_fetch_sub(&(rwlock->read_count), 1) == 1) {
        signal_semaphore(&(rwlock->w));
    }
    unlock_mutex(&(rwlock->mutex));
}

static inline void w_lock(RWLock *rwlock) {
    wait_semaphore(&(rwlock->w));
}

static inline void w_unlock(RWLock *rwlock) {
    signal_semaphore(&(rwlock->w));
}

#endif // RWLOCK_H
