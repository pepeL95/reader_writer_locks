# include "readerwriter.h"
# include <stdio.h>
# include <stdlib.h>
# include <pthread.h>
# include <assert.h>
# include <unistd.h>

void simulate_read_write(void);

void child (void);

int main(int argc, char *argv[]) {

    const char * file_name = "scenarios.txt";
    char rw;
    int rc;
    FILE * fp;
    // allocating heap spcace for structs
    rw_lock_t * rwl = (rw_lock_t *) malloc(sizeof(rw_lock_t));
    rwl->reader_lock = (sem_t *) malloc(sizeof(sem_t));
    rwl->writer_lock = (sem_t *) malloc(sizeof(sem_t));
    rwl->FIFO_ticket = (sem_t *) malloc(sizeof(sem_t));
    if (rwl == NULL || rwl->reader_lock == NULL || rwl->writer_lock == NULL || rwl->FIFO_ticket == NULL) {
        puts("Failed to allocate heap memory. Exiting program with code 1...");
        exit(1);
    }
    
    rw_lock_init(rwl, 0);

    // printf("reader lock val: %d", rwl->reader_lock->val);
    // printf("writer lock val: %d", rwl->writer_lock->val);
    // printf("FIFO lock val: %d", rwl->FIFO_ticket->val);

    fp = fopen(file_name, "r");
    if (fp == NULL) {
        puts("Error while reading file...");
        exit(1);
    }
    
    while(fscanf(fp, "%c", &rw) != EOF) {
        if (rw == '\n') { 
            puts("\nnew scenario, resetting...");
            sleep(5);
            puts("destroying prior locks...");
            rc = pthread_cond_destroy(&rwl->reader_lock->cond);
            assert(rc == 0);
            rc = pthread_mutex_destroy(&rwl->reader_lock->lock);
            assert(rc == 0);
            rc = pthread_cond_destroy(&rwl->writer_lock->cond);
            assert(rc == 0);
            rc = pthread_mutex_destroy(&rwl->writer_lock->lock);
            assert(rc == 0);
            rc = pthread_cond_destroy(&rwl->FIFO_ticket->cond);
            assert(rc == 0);
            rc = pthread_mutex_destroy(&rwl->FIFO_ticket->lock);
            assert(rc == 0);
            puts("initializing new locks...");
            rw_lock_init(rwl, 0);
            puts("resetting done successfully...");
            continue; 
        }
        printf("%c", rw);
    }
    puts("\nfreeing heap memory...");
    free(rwl->reader_lock);
    free(rwl->writer_lock);
    free(rwl->FIFO_ticket);
    free(rwl);
    return 0;
}

void simulate_read_write(void) {
    int T = rand() % 10000;
    for (int i = 0; i < T; i++)
        for (int j = 0; j < T; j++)
            continue;
}

void child (void) {

}