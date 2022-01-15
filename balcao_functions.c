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

typedef struct dados_pipes
{
    char nomePipe[25];
    int fd;
    int cancel;
} TDados;

void addTOline()
{
    printf("\n passou por addTOline !");
    // adiciona cliente à lista de espera , lista lgada ??

    // organiza-a por prioridade
}

void *recolheClientes(void *arg)
{
    TDados *as = (TDados *)arg;
    int n;
    char str[50];
    utente u;
    utente uAtualizado;

    while (!as->cancel)
    {
        if ((n = read(fd_balcao, &u, sizeof(utente)) == -1))
            perror("ERRO ao ler do pipe cliente");

        printf("\n[CLIENTE] %s os sintomas: %s", u.nome, u.sintomas);
        u = classifica(u); // classifica retorna a struct cliente já preenchida com prioridade e especialidade
        addTOline(u);


        fd_cliente = open(u.fifoNome, O_RDWR);
        if ((n = write(fd_cliente, &u, sizeof(u)) == -1))
            perror("ERRO ao ler do pipe cliente");
        

    }

    return NULL;
}

void running()
{
    utente listaUtentes[maxclientes];
    especialista listaEsp[maxmedicos];
    char str1[50];
    int res;

    pthread_t th[2];
    TDados tdadosClientes, tdadosMedicos;

    if (configClassificador() == 1)
    {
        shutdown();
    }

    // cria pipe balcao
    res = mkfifo(FIFO_BALCAO, 0666);

    // abre pipe para leitura
    fd_balcao = open(FIFO_BALCAO, O_RDWR);
    if (fd_balcao == -1)
    {
        perror("\nAbrir fifo balcao");
        exit(1);
    }

    //lança thread que espera clientes
    tdadosClientes.cancel = 0;
    pthread_create(&th[0], NULL, &recolheClientes, &tdadosClientes);

    //lança thread que espera especialistas

    // Termina de recolher cliente e medicos , cancela as threads
    //   tdadosClientes.cancel = 1; //pede à thread para sair
    // pthread_join(th[0],NULL); // espera que a thread acabe



    do
    {
        fprintf(stdout, "\n comando: ");
        fgets(str1, sizeof(str1) - 1, stdin);

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

    } while (strcmp(str1, "encerra") != 0);

    shutdown();
}










void shutdown()  ///// FALTA 
{
    fprintf(stdout, "O balcao vai encerrar... \n ");

    // encerra o classificador
    utente u;
    strcpy(u.sintomas, "#fim");
    classifica(u);

    // unlink pipes
    close(fd_balcao);
    unlink(FIFO_BALCAO);

    // envia sinal para todos os clientes encerrarem

    // envia sinal para todos os especialistas encerrarem

}






// ESTÁ OKAY

utente classifica(utente u)
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

    char *aux;
    strtok_r(str_temp, " ", &aux);

    u.prioridade = atoi(aux);
    strcpy(u.especialidade, str_temp);

    printf("\nUtente: %s - %s - [%d]", u.nome, u.especialidade, u.prioridade);

    return u;
}

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

int isAlreadyRunning()
{
    // se consegue detetar um pipe aberto, termina
    if (access(FIFO_BALCAO, F_OK) != -1)
    {
        fprintf(stderr, "BALCAO ALREADY RUNNING");
        return 1;
    }
    return 0;
}

int begin()
{

    if (isAlreadyRunning() == 1)
    {
        shutdown();
    }

    environmentVariables();
    showEnvironmentVariables();

    return 0;
}
