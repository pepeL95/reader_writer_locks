# include "readerwriter.h"
# include <stdio.h>
# include <stdlib.h>
# include <pthread.h>
# include <assert.h>
# include <unistd.h>
// ********************** Basic Semaphore Implementation **********************
void sem_init(sem_t * s, int val) {
    int rc;
    rc = pthread_cond_init(&s->cond, NULL);
    assert(rc == 0);
    rc = pthread_mutex_init(&s->lock, NULL);
    assert(rc == 0);
    s->val = val;
}

void sem_wait(sem_t * s) {
    int rc;
    rc = pthread_mutex_lock(&s->lock);
    assert(rc == 0);
    while (s->val <= 0) {
        rc = pthread_cond_wait(&s->cond, &s->lock);
        assert(rc == 0);
    }
    s->val--;
    rc = pthread_mutex_unlock(&s->lock);
    assert(rc == 0);
}

void sem_post(sem_t *s) {
    int rc;
    rc = pthread_mutex_lock(&s->lock);
    assert(rc == 0);
    s->val++;
    rc = pthread_cond_signal(&s->cond);
    assert(rc == 0);
    rc = pthread_mutex_unlock(&s->lock);
    assert(rc == 0);
}

// ********************** Reader Writer Lock Implementation **********************
void rw_lock_init(rw_lock_t * rwl, int val) {
    sem_init(rwl->reader_lock, 1);
    sem_init(rwl->writer_lock, 1);
    sem_init(rwl->FIFO_ticket, 1);
    rwl->readers = val;
}

void acquire_writelock(rw_lock_t * rwl) {
    /* only acquire if no other writer has it */
    sem_wait(rwl->FIFO_ticket);
    sem_wait(rwl->writer_lock);
    sem_post(rwl->FIFO_ticket);
}

void release_writelock(rw_lock_t * rwl) {
    sem_post(rwl->writer_lock);
}

void acquire_readlock(rw_lock_t * rwl) {
    /* acquire read lock and prevent writer from writting until
        lock has been released */
    sem_wait(rwl->FIFO_ticket);
    sem_wait(rwl->reader_lock); // wait for lock to crit section
    rwl->readers++;
    if (rwl->readers == 1)
        sem_wait(rwl->writer_lock);
    sem_post(rwl->FIFO_ticket);
    sem_post(rwl->reader_lock); // allow other readers to access ds
    // puts("read lock acquired...");

}

void release_readlock(rw_lock_t * rwl) {
    sem_wait(rwl->reader_lock);
    rwl->readers--;
    if (rwl->readers == 0)
        sem_post(rwl->writer_lock);
    sem_post(rwl->reader_lock);
    // puts("read lock released...");
}