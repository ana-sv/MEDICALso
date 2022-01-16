#include "utils.h"

int maxclientes;
int maxmedicos;

int stopRunning = 0;

// int fd_lixo;

// Comunicacao com classificador
int fd_in[2], fd_out[2];

//Comunicação com Clientes

int fd_balcao;
int fd_cliente;
char fifoCliente[25];

//Comunicao com Medicos
int fd_medico;
char fifoMedico[25];
int fd_balcao_aux;


utente listaEspera[50];
especialista listaMedicos[50];


typedef struct dados_pipes
{
    char nomePipe[25];
    int fd;
    int cancel;
} TDados;

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
    else
    {
        maxclientes = 5;
    }

    if (getenv("MAXMEDICOS") != NULL)
    {
        a = atoi(getenv("MAXMEDICOS"));
        maxmedicos = a;
    }
    else
    {
        maxmedicos = 5;
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

utente classifica(utente u)
{

    char str_temp[100];
    int readAux = 0;

    // envia ao classificador
    write(fd_in[1], u.sintomas, strlen(u.sintomas));

    // recebe do classificador
    readAux = read(fd_out[0], str_temp, sizeof(str_temp) - 1);
    if (readAux == 0)
    {
        fprintf(stderr, "READ FROM CLASSIFICADOR");
        shutdown();
    }
    str_temp[readAux] = '\0';

    fprintf(stdout, "%s", str_temp);

    char *aux;
    strtok_r(str_temp, " ", &aux);

    u.prioridade = atoi(aux);
    strcpy(u.especialidade, str_temp);

    // printf("\nUtente: %s - %s - [%d]", u.nome, u.especialidade, u.prioridade);
    printf("\n");

    return u;
}

void printListaEspera()
{

    fprintf(stdout, "\n----- Lista de Espera -----");

    for (int i = 0; i < maxclientes; i++)
    {
        if (listaEspera[i].pid != 0)
        {
            fprintf(stdout, "\n[ Utente %d ] %s", listaEspera[i].pid, listaEspera[i].nome);
            fprintf(stdout, "\n- Especialidade: %s ", listaEspera[i].especialidade);
            fprintf(stdout, "\n- Prioridade: %d", listaEspera[i].prioridade);
        }
    }
}

void printListaMedicos()
{

    fprintf(stdout, "\n----- Lista de Especialistas-----");

    for (int i = 0; i < maxclientes; i++)
    {
        if (listaEspera[i].pid != 0)
        {
            fprintf(stdout, "\n[ Especialista %d ] Dr %s", listaMedicos[i].pid, listaMedicos[i].nome);
            fprintf(stdout, "\n- Especialidade: %s ", listaMedicos[i].especialidade);
        }
    }
}

void addCliente(utente u)
{
    int i, j, aux;

    // adiciona cliente à lista de espera

    for (int i = 0; i < maxclientes; i++)
    {
        
        if(listaEspera[i].pid == 0){
            listaEspera[i] = u;
            break;
        }
    }
    

    if (i == maxclientes)
    {
        printf("\nAtencao ! lista de espera cheia!");
    }

    // organiza-a por prioridade
    //3 tem maior prioridade, 1 tem a menor


    for(i=0; i<maxclientes; i++)
    {
        for(j=i+1; j<maxclientes; j++)
        {
            if(listaEspera[i].prioridade < listaEspera[j].prioridade)
            {
                aux = listaEspera[i].prioridade;
                listaEspera[i].prioridade = listaEspera[j].prioridade;
                listaEspera[j].prioridade = aux;
            }
        }
    }



}

void addMedico(especialista esp)
{
    int i;
    for (i = 0; i < maxmedicos, listaMedicos[i].pid == 0; i++)
    {
        listaMedicos[i] = esp;
    }
    if (i == maxmedicos)
    {
        printf("\nAtencao ! lista de medicos cheia!");
    }
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

        printf("\n[CLIENTE] %s  \n[sintomas] %s", u.nome, u.sintomas);
        u = classifica(u); // classifica retorna a struct cliente já preenchida com prioridade e especialidade
        addCliente(u);

        fd_cliente = open(u.fifoNome, O_RDWR | O_NONBLOCK);
        if ((n = write(fd_cliente, &u, sizeof(u)) == -1))
            perror("ERRO ao escrever do pipe cliente");
        close(fd_cliente);
    }

    return NULL;
}

void *recolheMedicos(void *arg)
{
    TDados *as = (TDados *)arg;
    int n;
    char str[50];
    especialista esp;

    while (!as->cancel)
    {
        n = read(fd_balcao_aux, &esp, sizeof(especialista));

        if (n > 0)
        {

            printf("\n[MEDICO] %s especialidade: %s", esp.nome, esp.especialidade);
            addMedico(esp);
        }
    }

    return NULL;
}

void shutdown()
{
    fprintf(stdout, "\n O balcao vai encerrar... \n ");

    unlink(FIFO_BALCAO);
    unlink(FIFO_BALCAO_AUX);
    remove(FIFO_BALCAO_AUX);
    remove(FIFO_BALCAO);
    exit(0);

    // encerra o classificador
    // tem que se trocar para cima mas está a parar aqui e não deixa fechar
    utente u;
    strcpy(u.sintomas, "#fim");
    classifica(u);

    // envia sinal para todos os clientes encerrarem

    // envia sinal para todos os especialistas encerrarem
}

void running()
{

    int res1, res2;

    // para as threads
    pthread_t th[2];
    TDados tdadosClientes, tdadosMedicos;

    if (configClassificador() == 1)
    {
        shutdown();
    }

    //inicializar arrays;
    // enquanto o pid for 0 , nao interessa
    for (int i = 0; i < maxclientes; i++)
    {
        listaEspera[i].pid = 0;
    }

    for (int i = 0; i < maxmedicos; i++)
    {
        listaMedicos[i].pid = 0;
    }

    // cria pipe balcao
    res1 = mkfifo(FIFO_BALCAO, 0666);
    res2 = mkfifo(FIFO_BALCAO_AUX, 0666);
    if (res1 == -1 || res2 == -1)
    {
        perror("\nerro ao criar fifos do balcao");
        shutdown();
    }

    // abre pipe para leitura
    fd_balcao = open(FIFO_BALCAO, O_RDWR);
    if (fd_balcao == -1)
    {
        perror("\nerro ao fifo do balcao");
        exit(1);
    }

    fd_balcao_aux = open(FIFO_BALCAO_AUX, O_RDWR);
    if (fd_balcao_aux == -1)
    {
        perror("\nAbrir fifo do  balcao auxiliar");
        exit(1);
    }

    //lança thread que espera clientes
    tdadosClientes.cancel = 0;
    pthread_create(&th[0], NULL, &recolheClientes, &tdadosClientes);

    //lança thread que espera especialistas
    tdadosMedicos.cancel = 0;
    pthread_create(&th[1], NULL, &recolheMedicos, &tdadosMedicos);


    // Termina de recolher cliente e medicos , cancela as threads
    // tdadosClientes.cancel = 1; //pede à thread para sair
    // pthread_join(th[0],NULL); // espera que a thread acabe


    char comando[30]; //fica com tudo o que escrevo
    char *cmd[3];     // fica com tudo divide, 2 palavras + '\0'
    int i;

    fprintf(stdout, "\n>> Insira um comando: ");

    do
    {

        fgets(comando, 30, stdin); //mete tudo o que escreveu em comando, PROBLEMA! com "\n" e " "

        comando[strlen(comando) - 1] = '\0'; // mete \0 no fim de tudo

        i = 0;
        cmd[i] = strtok(comando, " ");

        while (cmd[i] != NULL)
        {
            i++;
            cmd[i] = strtok(NULL, " ");
        }

        if (strcmp(cmd[0], "utentes") == 0)
        {
            printListaEspera();
        }
        if (strcmp(cmd[0], "especialistas") == 0)
        {
            printListaMedicos();
        }
        if (strcmp(cmd[0], "delut") == 0)
        {
            printf("\nDELUT %s", cmd[1]);
            // remover um utente em espera e informando-o disso
        }
        if (strcmp(cmd[0], "delesp") == 0)
        {
            // remover um especialista que nao esteja em consulta e informando-o disso
            printf("Não implementado");
        }
        if (strcmp(cmd[0], "freq") == 0)
        {
            // apresenta as filas de espera, atualizado de N em N segundos
            printf("Não implementado");
        }

    } while (strcmp(cmd[0], "encerra") != 0);

    shutdown();
}