#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>

int threadCount;
int tableCount;

int *tables;
int *ENTRY;
int *NUM;



void enterCriticalSection(int i) {
    ENTRY[i] = 1;
    
    int max = 0;
    for(int j = 0; j < threadCount; j++) {
        if(NUM[j] > max) {
            max = NUM[j];
        }
    }

    NUM[i] = max + 1;
    ENTRY[i] = 0;

    for(int j = 0; j < threadCount; j++) {
        while(ENTRY[j] != 0) {
            //WAIT
        }

        while(NUM[j] != 0 && (NUM[j] < NUM[i] || (NUM[j] == NUM[i] && j < i))) {
            //WAIT
        }
    }

}

void exitCriticalSection(int i) {
    NUM[i] = 0;
}



int freeTables() {
    for(int i = 0; i < tableCount; i++) {
        if(tables[i] == 0) {
            return 1;
        }
    }

    return 0;
}

void printState() {
    for(int i = 0; i < tableCount; i++) {
        if(tables[i] == 0) {
            printf("-");
        }
        else {
            printf("%d", tables[i]);
        }
    }

    printf("\n");
}

void *reserve(void *arg) {
    int id = *((int *)arg);
    unsigned int seed = time(NULL) ^ id;

    while(1){
        sleep(1);

        if(!freeTables()) {
            break;
        }

        int chosenTable = rand_r(&seed) % tableCount;

        printf("Thread %d: Table %d\n", id + 1, chosenTable + 1);

        enterCriticalSection(id);

        if(tables[chosenTable] == 0) {
            tables[chosenTable] = id + 1;

            printf("Thread %d: Reserved table %d, state:\n", id + 1, chosenTable + 1);

            printState();
        } 
        else {
            printf("Thread %d: Unable to reserve table %d, state:\n", id + 1, chosenTable + 1 );
            
            printState();
        }

        exitCriticalSection(id);
    }

    pthread_exit(NULL);
}



int main(int argCount, char *argValues[]) {
    if (argCount != 3)
    {
        printf("Use 2 arguments: thread count and table count.\n");
        return 1;
    }

    threadCount = atoi(argValues[1]);
    tableCount = atoi(argValues[2]);

    srand(time(NULL));

    tables = (int *)calloc(tableCount, sizeof(int));
    ENTRY = (int *)calloc(threadCount, sizeof(int));
    NUM = (int *)calloc(threadCount, sizeof(int));

    pthread_t threads[threadCount];
    int threadIds[threadCount];

    for(int i = 0; i < threadCount; i++) {
        threadIds[i] = i;

        pthread_create(&threads[i], NULL, reserve, &threadIds[i]);
    }

    for(int i = 0; i < threadCount; i++)
    {
        pthread_join(threads[i], NULL);
    }

    free(tables);
    free(ENTRY);
    free(NUM);

    return 0;
}