
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
#include <sys/select.h>
#include <pthread.h>


#define FIFO_CLIENTE "fifoCliente%d"
#define FIFO_MEDICO "fifoMedico%d"

#define FIFO_BALCAO_CLIENTE "fifoBalcaoCliente"
#define FIFO_BALCAO_MEDICO "fifoBalcaoMedico"

#define N_THREADS 3 

typedef struct Utente {
    char nome[30];
    int pid;
    char fifoNome[30];
    char especialidade[10];
    char sintomas[100];
    int prioridade; 
    int emConsulta;  // se 0 , está em consulta
} utente;


typedef struct Especialista
{
    char nome[30];
    char especialidade[30];
    int pid;
    char fifoNome[30];
    int emConsulta; 

} especialista ;



typedef struct Consulta
{
    utente u;
    especialista esp;
    char msg[1000];

} consulta ;





// Funções Balcao

void environmentVariables();
void showEnvironmentVariables();

int isAlreadyRunning();

int configClassificador(); 
utente classifica();

void printListaEspera();
void printListaMedicos();

void deleteUtente( utente ut);
void deleteMedico(especialista esp);
void addCliente(utente u);
void addMedico(especialista esp);

void *recolheClientes(void *arg);
void *recolheMedicos(void *arg);

void funcaoSinalC();

void running();
void shutdown();