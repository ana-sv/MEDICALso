#include "utils.h"

int maxclientes;
int maxmedicos;

int stopRunning = 0;

int fd_balcao;
int fd_lixo;

// Comunicacao com classificador
int fd_in[2], fd_out[2];

//Comunicação com Clientes
int fd_cliente;
char fifoCliente[25];

//Comunicao com Medicos
int fd_medico;
char fifoMedico[25];


utente *listaUtentes;
especialista *listaMedicos;

typedef struct dados_pipes{
    char nomePipe[25];
    int fd;
    int cancel;
}TDados;


utente classifica( utente u )
{

    char str_temp[100];
    int readAux = 0;

    // envia ao classificador
    write(fd_in[1], u.sintomas, strlen(u.sintomas));

    // recebe do classificador
    readAux = read(fd_out[0], str_temp, sizeof(str_temp) - 1);
    if (readAux == -1)
    {
        fprintf(stderr, "READ FROM CLASSIFICADOR");
        exit(1);
    }
    str_temp[readAux] = '\0';

    fprintf(stdout, "%s", str_temp);


    // u.especialidade = ??
    // u.prioridade == ?? 


    return u;
}


void addClient(utente u){



}

void addTOline(){  // mete o utente em fila de espera dependendo da sua prioridade; 
    // só organiza porque o lugar na lita de espera está na stuct de cada um ?? 
    // ou será melhor fazer uma lista ligada de clientes e ordena-la ?

}



void * trataPipesClientes( void * arg){
    TDados *as = (TDados * )arg;
    int n;
    char str[50];
    utente u;
    utente uAtualizado; 

    while( !as->cancel ){
        if( ( n = read(as->fd, &u , sizeof(utente) ) == -1))
            perror("ERRO ao ler do pipe cliente");

        printf("\n[CLIENTE] %s os sintomas: %s", u.nome, u.sintomas);
        uAtualizado = classifica(u);
        addClient(uAtualizado);
        addTOline(uAtualizado);
    }

    return NULL; 
}



void running()
{
    char str1[50];
    pthread_t threadClientes, threadMedicos;
    TDados tdadosClientes, tdadosMedicos;
    int res;

    if (configClassificador() == 1)
    {
        shutdown();
        return;
    }
    
    if( mkfifo(FIFO_BALCAO, 00777)==-1 )
        perror("\nErro ao abrir o FIFO_CLIENTE");

   
    fd_cliente = open(FIFO_CLIENTE,O_RDWR);
    if(fd_cliente == -1 )
        perror("\nErro ao abrir o FIFO_CLIENTE");

    fd_medico = open(FIFO_MEDICO, O_RDWR);
    if(fd_medico == -1 )
        perror("\nErro ao abrir o FIFO_CLIENTE");


    //lança thread que espera clientes
    tdadosClientes.fd = fd_cliente;
    strcpy(tdadosClientes.nomePipe, FIFO_CLIENTE );
    tdadosClientes.cancel = 0;
    res = pthread_create( &threadClientes,NULL, &trataPipesClientes,&tdadosClientes);
    if (res == 0 )
       perror("\nERRO ao criar pthread para recolher clientes ");



    //lança thread que espera especialistas 




    // Termina de recolher cliente e medicos , cancela as threads
    tdadosClientes.cancel = 1; //pede à thread para sair 
    pthread_join(threadClientes,NULL); // espera que a thread acabe 




    // thread para fazer os pares cliente-medico 
                // 
    





    do{


        if (strcmp(str1, "utentes") == 0)
        {
            // lista de utentes em espera e os atendidos, indicando especialidade e prioridade
            printf("Não implementado");
        }
        if (strcmp(str1, "especialistas") == 0)
        {
            // lista dos especialistas ligados, em consulta ou nao e a sua especialidade
            printf("Não implementado");
        }
        if (strcmp(str1, "delut") == 0)
        {
            // remover um utente em espera e informando-o disso
            printf("Não implementado");
        }
        if (strcmp(str1, "delesp") == 0)
        {
            // remover um especialista que nao esteja em consulta e informando-o disso
            printf("Não implementado");
        }
        if (strcmp(str1, "freq") == 0)
        {
            // apresenta as filas de espera, atualizado de N em N segundos
            printf("Não implementado");
        }


    }while( strcmp(str1, "encerra") != 0);

    shutdown();
    
}






// NAO MEXE
int configClassificador()
{

    pipe(fd_in);
    pipe(fd_out);

    switch (fork())
    {
    case 1:
        fprintf(stderr, "FORK");
        return 1;
        break;

    case 0: // Child Process
        close(0);
        dup(fd_in[0]);
        close(fd_in[0]);
        close(fd_in[1]);

        close(1);
        dup(fd_out[1]);
        close(fd_out[0]);
        close(fd_out[1]);

        execl("./classificador", "./classificador", NULL);
        break;

    default: // Parent Process
        close(fd_in[0]);
        close(fd_out[1]);
        break;
    }

    return 0;
}

void showEnvironmentVariables()
{
    printf("\nMAXCLIENTES: %d", maxclientes);
    printf("\nMAXMEDICOS: %d\n", maxmedicos);
}

void environmentVariables()
{
    int a;

    if (getenv("MAXCLIENTES") != NULL)
    {
        a = atoi(getenv("MAXCLIENTES"));
        maxclientes = a;
    }

    if (getenv("MAXMEDICOS") != NULL)
    {
        a = atoi(getenv("MAXMEDICOS"));
        maxmedicos = a;
    }
}





void shutdown()
{
    fprintf(stdout, "O balcao vai encerrar... \n ");

    // encerra o classificador
    utente u; 
    strcpy(u.sintomas,"#fim");
    classifica(u);

    // unlink pipes

    // envia sinal para todos os clientes encerrarem

    // envia sinal para todos os especialistas encerrarem
}

int isAlreadyRunning()
{
    // se consegue detetar um pipe aberto, termina

    // if( access(      )
    //    fprintf(stderr, "BALCAO ALREADY RUNNING" ");
    return 1;
}

int begin()
{

    if (!isAlreadyRunning())
    {
        shutdown();
        return 1;
    }

    environmentVariables();
    showEnvironmentVariables();

    return 0;
}