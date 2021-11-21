

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#include <time.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <signal.h>

typedef struct {
    char nome[30];
    char especialidade[10];
    int prioridade;
    int pid;
}cliente;


void running();
void classifica();
int configClassificador(); 

void environmentVariables();
void showEnvironmentVariables();

int isAlreadyRunning();
void shutdown();
int begin(); 



