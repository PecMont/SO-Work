# Documentação da Biblioteca `so.h`

A biblioteca `so.h` implementa primitivas de sincronização de baixo nível utilizando operações atômicas. Ela fornece mecanismos para controle de acesso a recursos compartilhados, utilizando estruturas de **Mutex**, **Semáforo**, **Variável de Condição**, **Barreira** e **Read-Write Lock**.

## Índice

1. [Mutex](#mutex)
2. [Semáforo](#semáforo)
3. [Variável de Condição](#variável_de_condição)
4. [Barreira](#barreira)
5. [Read-Write Lock](#readWrite_lock)
6. [Exemplos de Uso](#exemplos_de_uso)
7. [Comclusão](#conclusão)

---

## Mutex
Um Mutex é utilizado para garantir que apenas uma thread acesse uma seção crítica de cada vez. Ele é implementado usando operações atômicas para evitar a necessidade de bloqueios de kernel, proporcionando uma forma eficiente de sincronização.

### Estrutura
```c    
typedef struct {
    atomic_int locked;
} Mutex;
```

### Função
```c
init_mutex(Mutex *mutex): Inicializa o mutex, configurando seu estado de bloqueio para 0 (não bloqueado).
void init_mutex(Mutex *mutex);

lock_mutex(Mutex *mutex): Bloqueia o mutex. Se já estiver bloqueado, a thread entra em espera até que o mutex seja liberado.
void lock_mutex(Mutex *mutex);

unlock_mutex(Mutex *mutex): Desbloqueia o mutex, permitindo que outras threads acessem a seção crítica.
void unlock_mutex(Mutex *mutex);
```
## Semáforo
O Semáforo controla o acesso a um recurso compartilhado, permitindo que múltiplas threads acessem esse recurso simultaneamente até o limite definido.

### Estrutura
```c  
typedef struct {
    atomic_int value;
    Mutex mutex;
} Semaphore;
```
### Função
```c 
init_semaphore(Semaphore *sem, int initial_value): Inicializa o semáforo com um valor inicial.
void init_semaphore(Semaphore *sem, int initial_value);

wait_semaphore(Semaphore *sem): Decrementa o valor do semáforo. Se o valor for 0, a thread entra em espera.
void wait_semaphore(Semaphore *sem);

signal_semaphore(Semaphore *sem): Incrementa o valor do semáforo, liberando uma thread bloqueada, se houver.
void signal_semaphore(Semaphore *sem);
```
## Variável_de_Condição
As Variáveis de Condição são usadas para sincronizar a execução de threads que precisam esperar ou ser notificadas quando uma condição específica for atendida.

### Estrutura
```c  
typedef struct {
    Semaphore sem;
    atomic_int count;
} Cond;
```

### Função
```c 
init_cond(Cond *cond): Inicializa a variável de condição.
void init_cond(Cond *cond);

wait_cond(Cond *cond, Mutex *mutex): A thread espera a condição ser sinalizada. O mutex é desbloqueado enquanto a thread espera.
void wait_cond(Cond *cond, Mutex *mutex);

signal_cond(Cond *cond): Sinaliza uma thread esperando na variável de condição.
void signal_cond(Cond *cond);

cond_broadcast(Cond *cond): Sinaliza todas as threads esperando na variável de condição.
void cond_broadcast(Cond *cond);
```
## Barreira
A Barreira é usada para sincronizar um conjunto de threads que devem esperar até que todas cheguem a um ponto específico antes de prosseguir.

### Estrutura
```c  
typedef struct {
    Semaphore sem;
    Mutex mutex;
    atomic_int count;
    int n;
} Barrier;
```
### Função
```c  
init_barrier(Barrier *barrier, int n): Inicializa a barreira, definindo o número de threads que devem esperar por ela.
void init_barrier(Barrier *barrier, int n);

barrier_wait(Barrier *barrier): A thread chama essa função para esperar até que todas as outras threads também chamem. A última thread a chegar libera todas as outras para continuar.
void barrier_wait(Barrier *barrier);
```
## ReadWrite_Lock
    
    O Read-Write Lock permite múltiplas threads acessarem recursos para leitura simultaneamente, mas garante que apenas uma thread tenha acesso para escrita por vez, bloqueando as outras.

### Estrutura
```c  
typedef struct {
    Mutex mutex;
    Semaphore w;
    atomic_int read_count;
} RWLock;
```
### Função
```c 
init_rwlock(RWLock *rwlock): Inicializa o Read-Write Lock, configurando a mutex e o semáforo para controle de escrita.
void init_rwlock(RWLock *rwlock);

r_lock(RWLock *rwlock): Adquire o bloqueio de leitura. Se não houver escritores, várias threads podem adquirir esse bloqueio simultaneamente
void r_lock(RWLock *rwlock);

r_unlock(RWLock *rwlock): Libera o bloqueio de leitura.
void r_unlock(RWLock *rwlock);

w_lock(RWLock *rwlock): Adquire o bloqueio de escrita, garantindo que nenhuma outra thread esteja lendo ou escrevendo.
void w_lock(RWLock *rwlock);

w_unlock(RWLock *rwlock): Libera o bloqueio de escrita.
void w_unlock(RWLock *rwlock);
```

## Exemplos_de_Uso
Essa biblioteca pode ser utilizada para resolver problemas de concorrência, como:

Implementação de produtores e consumidores.
Leitura e escrita concorrente em dados compartilhados.
Sincronização de múltiplas threads para realizar tarefas em conjunto.

## Testes


### Testes especificos: 
- [Test](testeEspecificos/test.c)

### Testes praticos:
- [Filosofo](testePratico/Filosofos.c)
- [Matriz](testePratico/MatrizesParalela.c)
- [PC](testePratico/ProdutorConsumidor.c)

# Conclusão
A biblioteca `so.h` é uma implementação simples e eficiente de primitivas de sincronização, utilizando operações atômicas para evitar o uso de chamadas de sistema pesadas e garantir o acesso controlado a recursos compartilhados entre múltiplas threads.