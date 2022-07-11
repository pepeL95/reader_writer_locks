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
    FILE * fp;
    rw_lock_t * rwl = malloc(sizeof(rw_lock_t)); 
    rw_lock_init(rwl, 0);

    fp = fopen(file_name, "r");
    if (fp == NULL) {
        puts("Error while reading file...");
        exit(1);
    }
    
    while(fscanf(fp, "%c", &rw) != EOF) {
        if (rw == '\n') { 
            puts("\nnew scenario, resetting...");
            sleep(5);
            rw_lock_destuct(rwl);
            rw_lock_init(rwl, 0);
            continue; 
        }
        printf("%c", rw);
    }

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