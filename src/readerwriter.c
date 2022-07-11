# include "readerwriter.h"
# include <stdio.h>
# include <stdlib.h>
# include <pthread.h>
# include <assert.h>

// ********************** Basic Semaphore Implementation **********************
void sem_init(sem_t * s, int val) {
    int rc;
    s = malloc(sizeof(sem_t)); // make room in heap for custom type sem_t
    s->val = val;
    rc = pthread_cond_init(&s->cond, NULL);
    assert(rc == 0);
    rc = pthread_mutex_init(&s->lock, NULL);
    assert(rc == 0);
}

void sem_wait(sem_t * s) {
    pthread_mutex_lock(&s->lock);
    while (s->val <= 0)
        pthread_cond_wait(&s->cond, &s->lock);
    (s->val)--;
    pthread_mutex_unlock(&s->lock);
}

void sem_post(sem_t *s) {
    pthread_mutex_lock(&s->lock);
    (s->val)++;
    pthread_cond_signal(&s->cond);
    pthread_mutex_unlock(&s->lock);
}

void sem_destuct(sem_t * s) {
    /* destroy locks and deallocate heap memory */
    int rc;
    puts("destroying locks...");
    rc = pthread_cond_destroy(&s->cond);
    assert(rc == 0);
    rc = pthread_mutex_destroy(&s->lock);
    assert(rc == 0);
    puts("locks destroyed successfully...");
    free(s);
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
    (rwl->readers)++;
    if (rwl->readers == 1)
        sem_wait(rwl->writer_lock);
    sem_post(rwl->FIFO_ticket);
    sem_post(rwl->reader_lock); // allow other readers to enter ds
}

void release_readlock(rw_lock_t * rwl) {
    sem_wait(rwl->reader_lock);
    (rwl->readers)--;
    if (rwl->readers == 0)
        sem_post(rwl->writer_lock);
    sem_post(rwl->reader_lock);
}

void rw_lock_destuct(rw_lock_t *rwl) {
    /* deallocates heap memory */
    sem_destuct(rwl->reader_lock);
    sem_destuct(rwl->writer_lock);
    sem_destuct(rwl->FIFO_ticket);
}
