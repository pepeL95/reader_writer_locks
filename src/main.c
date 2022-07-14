/* This programs implements non-starving rader/writer locks. 
   The input stream is drawn from the scenarios text file provided.
   Expected number of threads per scenario is 12.
 */ 
# include "readerwriter.h"
# include <stdio.h>
# include <stdlib.h>
# include <pthread.h>
# include <assert.h>
# include <unistd.h>
# define THREADS_PER_SCENARIO 12
# define NUMBER_OF_SCENARIOS 4

void * simulate_read(void *);
void * simulate_write(void *);

int main(int argc, char *argv[]) {
    const char * file_name = "scenarios.txt";
    char rw;
    int rc;
    pthread_t p[THREADS_PER_SCENARIO * NUMBER_OF_SCENARIOS];
    FILE * fp;
    rw_lock_t * rwl = NULL;
    args_t * my_args = NULL; // this will be passed as args in new threads

    // allocating heap spcace for custom  structs
    my_args = (args_t *) malloc(sizeof(args_t));
    rwl = (rw_lock_t *) malloc(sizeof(rw_lock_t));
    rwl->reader_lock = (sem_t *) malloc(sizeof(sem_t));
    rwl->writer_lock = (sem_t *) malloc(sizeof(sem_t));
    rwl->FIFO_ticket = (sem_t *) malloc(sizeof(sem_t));
    if (rwl == NULL || rwl->reader_lock == NULL || rwl->writer_lock == NULL || rwl->FIFO_ticket == NULL) {
        puts("Failed to allocate heap memory. Exiting program with code 1...");
        exit(1);
    }    
    // initializations...
    rw_lock_init(rwl, 1); 
    my_args->rwlp = rwl;
    my_args->rwp = &rw;

    fp = fopen(file_name, "r");
    if (fp == NULL) {
        puts("Error while reading file...");
        exit(1);
    }
    
    puts("*************************************************");
    for(int i = 0 ; fscanf(fp, "%c", &rw) != EOF;) {
        if (rw == 'r') {
            rc = pthread_create(&p[i], NULL, simulate_read, (void *) my_args);
            assert(rc == 0);
            i++;
        }
        else if (rw == 'w') {
            rc = pthread_create(&p[i], NULL, simulate_write, (void *) my_args);
            assert(rc == 0);
            i++;
        }
        else if (rw == '\n') { 
            // wait for ongoing threads to finish...
            for (int i = 0; i < THREADS_PER_SCENARIO; i ++) {
                rc = pthread_join(p[i], NULL);
                assert(rc == 0);
            }
            puts("*************************************************");
            puts("new scenario, resetting...\ndestroying prior locks...");
            rc = pthread_mutex_destroy(&rwl->reader_lock->lock); assert(rc == 0);
            rc = pthread_mutex_destroy(&rwl->writer_lock->lock); assert(rc == 0);
            rc = pthread_mutex_destroy(&rwl->FIFO_ticket->lock); assert(rc == 0);
            rc = pthread_cond_destroy(&rwl->reader_lock->cond); assert(rc == 0);
            rc = pthread_cond_destroy(&rwl->writer_lock->cond); assert(rc == 0);
            rc = pthread_cond_destroy(&rwl->FIFO_ticket->cond); assert(rc == 0);
            puts("initializing new locks...");
            rw_lock_init(rwl, 1);
            puts("resetting done successfully...");
            puts("*************************************************");
        }
        else continue; // invalid entry, continue... 
    }

    for (int i = 0; i < THREADS_PER_SCENARIO; i++) {
        rc = pthread_join(p[i], NULL);
        assert(rc == 0);
    }
    puts("*************************************************");
    
    // freeing heap memory
    free(rwl->reader_lock);
    free(rwl->writer_lock);
    free(rwl->FIFO_ticket);
    free(rwl);
    return 0;
}

void * simulate_read(void * arg) {
    args_t * my_args = (args_t *) arg;
    acquire_readlock(my_args->rwlp);
    int T = rand() % 10000;
    for (int i = 0; i < T; i++)
        for (int j = 0; j < T; j++)
            continue;
    release_readlock(my_args->rwlp);
    puts("read done");
    return (void *) NULL;
}

void * simulate_write(void * arg) {
    args_t * my_args = (args_t *) arg;
    acquire_writelock(my_args->rwlp);
    int T = rand() % 10000;
    for (int i = 0; i < T; i++)
        for (int j = 0; j < T; j++)
            continue;
    release_writelock(my_args->rwlp);
    puts("write done");
    return (void *) NULL;
}