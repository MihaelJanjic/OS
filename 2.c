#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/wait.h>

int *mem;

/*
mem[0] = PRAVO
mem[1] = ZASTAVICA[0]
mem[2] = ZASTAVICA[1]
*/

void enterCriticalSection(int p1, int p2) {
    mem[1 + p1] = 1;

    while(mem[1 + p2] != 0) {

        if(mem[0] == p2) {

            mem[1 + p1] = 0;

            while(mem[0] == p2) {
                //WAIT
            }

            mem[1 + p1] = 1;
        }
    }
}

void exitCriticalSection(int p1, int p2) {
    mem[0] = p2;
    mem[1 + p1] = 0;
}

int main(void) {
    int shmid;

    shmid = shmget(IPC_PRIVATE, 3 * sizeof(int), 0600);

    if(shmid == -1) {
        exit(1);
    }

    mem = (int *) shmat(shmid, NULL, 0);

    mem[0] = 0; // RIGHT
    mem[1] = 0; // FLAG[0]
    mem[2] = 0; // FLAG[1]

    pid_t pid;

    for(int i = 0; i < 2; i++) {
        pid = fork();

        if(pid == 0) {
            //CHILD

            int j = 1 - i;

            for(int k = 1; k <= 5; k++) {
                enterCriticalSection(i, j);
                //CRITICAL SECTION

                for(int m = 1; m <= 5; m++) {
                    printf("(%d, %d, %d)\n", i, k, m);

                    sleep(1);
                }

                exitCriticalSection(i, j);
            }

            exit(0);
        }
    }

    wait(NULL);
    wait(NULL);

    shmdt(mem);
    shmctl(shmid, IPC_RMID, NULL);

    return 0;
}