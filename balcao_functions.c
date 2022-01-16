#include "utils.h"

int maxclientes;
int maxmedicos;

// Comunicacao com classificador
int fd_in[2], fd_out[2];

//Comunicação com Clientes
int fd_balcao_cliente;
int fd_cliente;
char fifoCliente[25];

//Comunicao com Medicos
int fd_medico;
char fifoMedico[25];
int fd_balcao_medico;

utente listaEspera[50];
especialista listaMedicos[50];

typedef struct dados_pipes
{
    pthread_t id;
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
    if (access(FIFO_BALCAO_CLIENTE, F_OK) != -1)
    {
        fprintf(stderr, "Ja' existe um balcao ativo");
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

void printListaUtentes()
{

    fprintf(stdout, "\n----- Lista de Utentes -----");

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

void printListaEspera()
{

    fprintf(stdout, "\n----- Lista de Espera -----");

    for (int i = 0; i < maxclientes; i++)
    {
        if (listaEspera[i].pid != 0 && listaEspera[i].emConsulta == 1)
        { // 0 se está em consulta, 1 se não está. (1) porque queremos apenas aqueles que ainda estao à espera da sua vez
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


void deleteUtente(utente u ){

    int i=0, pos=-1; 

    for(i=0; i<maxclientes; i++){
        if( strcmp(u.nome , listaEspera[i].nome) == 0  ){
            pos = i;    
        }
    }

    if(pos==-1){
        printf("\nUtente nao encontrado!");
    }

    for(i=pos; i<maxclientes; i++)
        listaEspera[i]=listaEspera[i+1];

        // falta informar o utente
}



void deleteMedico(especialista esp){

    int i=0, pos=-1; 

    for(i=0; i<maxmedicos; i++){
        if( strcmp(esp.nome , listaMedicos[i].nome) == 0  ){
            pos = i;    
        }
    }

    if(pos==-1){
        printf("\nMedico nao encontrado!");
    }

    for(i=pos; i<maxmedicos; i++)
        listaMedicos[i]=listaMedicos[i+1];

        // falta informar o especialista 
}


void addCliente(utente u)
{
    int i, j, aux;

    // adiciona cliente à lista de espera

    for (int i = 0; i < maxclientes; i++)
    {

        if (listaEspera[i].pid == 0)
        {
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

    for (i = 0; i < maxclientes; i++)
    {
        for (j = i + 1; j < maxclientes; j++)
        {
            if (listaEspera[i].prioridade < listaEspera[j].prioridade)
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
    for (i = 0; i < maxmedicos; i++)
    {

        if (listaMedicos[i].pid == 0)
        {
            listaMedicos[i] = esp;
            break;
        }
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
        if ((n = read(fd_balcao_cliente, &u, sizeof(utente)) == -1))
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
        n = read(fd_balcao_medico, &esp, sizeof(especialista));

        if (n > 0)
        {

            printf("\n[MEDICO] %s especialidade: %s", esp.nome, esp.especialidade);
            addMedico(esp);

        }
    }

    return NULL;
}

void funcaoSinalC()
{
    printf("\n\nCtrl^C detectado! Adeus!");
    shutdown();
}

void running()
{

    int res1, res2, i;

    signal(SIGINT, funcaoSinalC);

    // para as threads
    pthread_t th[N_THREADS];
    TDados tdadosClientes, tdadosMedicos;

    if (configClassificador() == 1)
    {
        shutdown();
    }

    //inicializar arrays;
    // enquanto o pid for 0 , nao interessa
    for (i = 0; i < maxclientes; i++)
    {
        listaEspera[i].pid = 0;
    }

    for (i = 0; i < maxmedicos; i++)
    {
        listaMedicos[i].pid = 0;
    }

    // cria pipe balcao
    res1 = mkfifo(FIFO_BALCAO_CLIENTE, 0666);
    res2 = mkfifo(FIFO_BALCAO_MEDICO, 0666);
    if (res1 == -1 || res2 == -1)
    {
        perror("\nerro ao criar fifos do balcao");
        shutdown();
    }

    // abre pipe para leitura
    fd_balcao_cliente = open(FIFO_BALCAO_CLIENTE, O_RDWR );
    if (fd_balcao_cliente == -1)
    {
        perror("\nerro ao fifo do balcao");
        exit(1);
    }

    fd_balcao_medico = open(FIFO_BALCAO_MEDICO, O_RDWR | O_NONBLOCK);
    if (fd_balcao_medico == -1)
    {
        perror("\nAbrir fifo do  balcao auxiliar");
        exit(1);
    }

    //lança thread que espera clientes
    tdadosClientes.cancel = 0;
    pthread_create(&th[0], NULL, recolheClientes, &tdadosClientes);

    //lança thread que espera especialistas
    tdadosMedicos.cancel = 0;
    pthread_create(&th[1], NULL, recolheMedicos, &tdadosMedicos);

    //lança thread que faz pares para consulta??
    /// EM FALTA

    char comando[30]; //fica com tudo o que escrevo
    char *cmd[3];     // fica com tudo divide, 2 palavras + '\0'

    fprintf(stdout, "\n>> Insira um comando: ");

    do
    {

        printf("\n");

        fgets(comando, 30, stdin);           // mete tudo o que escreveu em comando, PROBLEMA! com "\n" e " "
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
            printListaUtentes();
        }
        if (strcmp(cmd[0], "especialistas") == 0)
        {
            printListaMedicos();
        }
        if (strcmp(cmd[0], "delut") == 0)
        {
            // remover um utente em espera e informando-o disso
            utente ut;
            strcpy(ut.nome,cmd[1]);
            deleteUtente(ut);
        }
        if (strcmp(cmd[0], "delesp") == 0)
        {
            // remover um especialista que nao esteja em consulta e informando-o disso
            especialista e;
            strcpy(e.nome,cmd[1]);
            deleteMedico(e);
        }
        if (strcmp(cmd[0], "freq") == 0)
        {
            // apresenta as filas de espera, atualizado de N em N segundos
            printListaEspera(); // aqui a lista já está sempre organizada, pois é organizada sempre que aparece um novo utente
        }

    } while (strcmp(cmd[0], "encerra") != 0);

    // parar as threads , pode não estar da melhor forma trocar quando der..
    pthread_cancel(th[0]);
    pthread_cancel(th[1]);

    shutdown();
}



void shutdown()
{

    printf("\n O balcao vai encerrar... \n ");

    unlink(FIFO_BALCAO_CLIENTE);
    unlink(FIFO_BALCAO_MEDICO);
    remove(FIFO_BALCAO_MEDICO);
    remove(FIFO_BALCAO_CLIENTE);
   
    int i;

    // envia sinal para todos os clientes encerrarem
    for (i = 0; i < maxclientes; i++)
    {
        kill(listaEspera[i].pid, SIGKILL); // nao está feito da melhor maneira....
    }

    // envia sinal para todos os especialistas encerrarem
    for (i = 0; i < maxmedicos; i++)
    {
        kill(listaMedicos[i].pid, SIGKILL);
    }

  
    exit(0);

    // encerra o classificador
    // sei que nao chega aqui, tem que se trocar para cima mas está a parar aqui e não deixa fechar o prog
    utente u;
    strcpy(u.sintomas, "#fim");
    classifica(u);
}