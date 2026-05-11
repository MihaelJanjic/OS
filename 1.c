#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <math.h>

int num = 1;

int readLastItemInFile(const char *fileName)
{
    FILE *file = fopen(fileName, "r");
    int x = 0, last = 0;

    if (file == NULL) {
        return 0;
    }

    while (fscanf(file, "%d", &x) == 1){
        last = x;
    }

    fclose(file);
    return last;
}

int readFile(const char *fileName)
{
    FILE *file = fopen(fileName, "r");

    if(file==NULL) {
        return 0;
    }

    int x = 0;
    fscanf(file, "%d", &x);
    fclose(file);

    return x;
}

void overwriteFile(const char *fileName, int x) {
    FILE *file = fopen(fileName, "w");

    if(file==NULL) {
        return;
    }

    fprintf(file, "%d\n", x);
    
    fclose(file);
}

void writeInFile(const char *fileName, int x) {
    FILE *file = fopen(fileName, "a");

    if(file==NULL) {
        return;
    }

    fprintf(file, "%d\n", x);
    
    fclose(file);
}

void sigusr1Handler(int sig)
{ 
    printf("\n[SIGUSR1] Current number: %d\n", num);
    fflush(stdout);
}

void sigtermHandler(int sig)
{ 
    int last = readLastItemInFile("obrada.txt");
    overwriteFile("status.txt", (int)sqrt(last));
    printf("\n[SIGTERM] Save and quit.\n");
    exit(0);
}

void sigintHandler(int sig)
{ 
    printf("[SIGINT] Interrupt");
    exit(0);
}

int main (void)
{
    signal(SIGUSR1, sigusr1Handler);
    signal(SIGTERM, sigtermHandler);
    signal(SIGINT, sigintHandler);

    printf("PID: %d\n", (int)getpid());

    int status = readFile("status.txt");

    if(status == 0) {
        int last = readLastItemInFile("obrada.txt");
        num = (int)sqrt(last) + 1;
    }
    else {
        num = status + 1;
    }

    overwriteFile("status.txt", 0);

    while(1) {
        writeInFile("obrada.txt", num*num);
        num++;
        sleep(5);
    }

    return 0;
}
