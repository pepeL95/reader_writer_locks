# ifndef RW
# define RW
# include <stdio.h>
# include <pthread.h>

// ********************** Basic Semaphore Implementation **********************
typedef struct __sem_t {
    int val;
    pthread_cond_t cond;
    pthread_mutex_t lock; 
} sem_t;

// Initialization method
void sem_init(sem_t * s, int val) {
    s->val = val;
    pthread_cond_init(&s->cond, NULL);
    pthread_mutex_init(&s->lock, NULL);
}

// wait method
void sem_wait(sem_t * s) {
    pthread_mutex_lock(&s->lock);
    while (s->val <= 0)
        pthread_cond_wait(&s->cond, &s->lock);
    (s->val)--;
    pthread_mutex_unlock(&s->lock);
}

// post method
void sem_post(sem_t *s) {
    pthread_mutex_lock(&s->lock);
    (s->val)++;
    pthread_cond_signal(&s->cond);
    pthread_mutex_unlock(&s->lock);
}

// ********************** Reader Writer Lock Implementation **********************
typedef struct __rw_lock {
    sem_t reader_lock;
    sem_t writer_lock;
    sem_t FIFO_ticket;
    unsigned int readers;
    
} rw_lock_t;

// rw_lock initilizer
void rw_lock_init(rw_lock_t * rwl, unsigned int rval, unsigned int wval) {
    sem_init(&rwl->reader_lock, 1);
    sem_init(&rwl->writer_lock, 1);
    sem_init(&rwl->FIFO_ticket, 1);
    rwl->readers = rval;
}

// acquire write lock
void acquire_writelock(rw_lock_t * rwl) {
    /* only acquire if no other writer has it */
    sem_wait(&rwl->FIFO_ticket);
    sem_wait(&rwl->writer_lock);
    sem_post(&rwl->FIFO_ticket);
}
// realease write lock
void release_writelock(rw_lock_t * rwl) {
    sem_post(&rwl->writer_lock);
}

// acquire read lock
void acquire_readlock(rw_lock_t * rwl) {
    /* acquire read lock and prevent writer from writting until
        lock has been released */
    sem_wait(&rwl->FIFO_ticket);
    sem_wait(&rwl->reader_lock); // wait for lock to crit section
    (rwl->readers)++;
    if (rwl->readers == 1)
        sem_wait(&rwl->writer_lock);
    sem_post(&rwl->FIFO_ticket);
    sem_post(&rwl->reader_lock); // allow other readers to enter ds
}

// release read lock
void release_readlock(rw_lock_t * rwl) {
    sem_wait(&rwl->reader_lock);
    (rwl->readers)--;
    if (rwl->readers == 0)
        sem_post(&rwl->writer_lock);
    sem_post(&rwl->reader_lock);
}
#endif