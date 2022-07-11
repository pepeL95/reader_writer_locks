# ifndef RW_H
# define RW_H
# include <stdio.h>
# include <pthread.h>

typedef struct __sem_t {
    int val;
    pthread_cond_t cond;
    pthread_mutex_t lock; 
} sem_t;

void sem_init(sem_t * s, int val);
void sem_wait(sem_t * s);
void sem_post(sem_t *s);

typedef struct __rw_lock_t {
    sem_t * reader_lock;
    sem_t * writer_lock;
    sem_t * FIFO_ticket;
    int readers;
} rw_lock_t;

void rw_lock_init(rw_lock_t * rwl, int val);
void acquire_writelock(rw_lock_t * rwl);
void release_writelock(rw_lock_t * rwl);
void acquire_readlock(rw_lock_t * rwl);
void release_readlock(rw_lock_t * rwl);

# endif 