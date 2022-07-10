# include <stdio.h>
# include <stdlib.h>
# include <assert.h>
# include <pthread.h>

void simulate_read_write(void);

int main(int argc, char **argv) {
    
    
    return 0;
}

void simulate_read_write(void) {
    int T = rand() % 10000;
    for (int i = 0; i < T; i++)
        for (int j = 0; j < T; j++)
            continue;
}
