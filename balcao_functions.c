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
int fd_balcao_aux;


 utente listaEspera[50];
 especialista listaMedicos[50];


typedef struct dados_pipes
{
    char nomePipe[25];
    int fd;
    int cancel;
} TDados;




void printListaEspera(){

    fprintf(stdout,"\n----- Lista de Espera -----");

    for(int i=0; i< maxclientes; i++){
        if( listaEspera[i].pid != 0 ){
        fprintf(stdout, "\n[ Utente %d ] %s", listaEspera[i].pid, listaEspera[i].nome );
        fprintf(stdout,"\n- Especialidade: %s ",listaEspera[i].especialidade );
        fprintf(stdout,"\n- Prioridade: %d", listaEspera[i].prioridade );
        }

    }

}

void printListaMedicos(){

    fprintf(stdout,"\n----- Lista de Especialistas-----");

    for(int i=0; i< maxclientes; i++){
        if( listaEspera[i].pid != 0 ){
        fprintf(stdout, "\n[ Especialista %d ] Dr %s", listaMedicos[i].pid, listaMedicos[i].nome );
        fprintf(stdout,"\n- Especialidade: %s ",listaMedicos[i].especialidade );
        }

    }

}




void addCliente(utente u)
{
    int i , j , key; 

    // adiciona cliente à lista de espera
    for( i=0; i<maxclientes, listaEspera[i].pid==0; i++){
            listaEspera[i] = u ; 
    }
    if(i == maxclientes){
         printf("\nAtencao ! lista de espera cheia!");
    }

    // organiza-a por prioridade

    for(i=1; i<maxclientes ; i++){
        key = listaEspera[i].prioridade;
        j = i-1;

        while(j>= 0 && listaEspera[i].prioridade > key){
            listaEspera[j+1] = listaEspera[j];
            j= j-1;
        }
        listaEspera[j+1].prioridade = key; 
    }

}


void addMedico(especialista esp)
{
       int i;
    for( i=0; i<maxmedicos, listaMedicos[i].pid==0; i++){
            listaMedicos[i] = esp ; 
    }
    if(i == maxmedicos){
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

        printf("\n[CLIENTE] %s os sintomas: %s", u.nome, u.sintomas);
        u = classifica(u); // classifica retorna a struct cliente já preenchida com prioridade e especialidade
        addCliente(u);


        fd_cliente = open(u.fifoNome, O_RDWR | O_NONBLOCK );
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

        if ( n > 0 ) {

            printf("\n[MEDICO] %s especialidade: %s", esp.nome , esp.especialidade);
            addMedico(esp);
        }

        
    }

    return NULL;
}



void running()
{
    char str[50], str1[25], str2[25];
    int res;



    // para as threads
    pthread_t th[2];
    TDados tdadosClientes, tdadosMedicos;

    if (configClassificador() == 1)
    {
        shutdown();
    }

    //inicializar arrays;
    // enquanto o pid for 0 , nao interessa 
    for(int i=0; i< maxclientes; i++){
        listaEspera[i].pid = 0 ; 
    }

    for(int i=0; i<maxmedicos; i++){
        listaMedicos[i].pid = 0;
    }



    // cria pipe balcao
    res = mkfifo(FIFO_BALCAO, 0666);
    res = mkfifo(FIFO_BALCAO_AUX, 0666);

    // abre pipe para leitura
    fd_balcao = open(FIFO_BALCAO, O_RDWR  );
    if (fd_balcao == -1)
    {
        perror("\nAbrir fifo balcao");
        exit(1);
    }
    
    fd_balcao_aux = open(FIFO_BALCAO_AUX, O_RDWR  );
    if (fd_balcao_aux == -1)
    {
        perror("\nAbrir fifo balcao auxiliar");
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


    char commando[30];       //fica com tudo o que escrevo 
    char* cmd[3];           // fica com tudo divide, 2 palavras + '\0'
    int i;

    do
    {


        fprintf(stdout, "\n comando: ");

        fgets(commando, 30, stdin); //mete tudo o que escreveu em commads, PROBLEMA! com "\n" e " "

        commando[strlen(commando) - 1] = '\0'; // mete \0 no fim de tudo

        i = 0;
        cmd[i] = strtok(commando, " ");

        while (cmd[i] != NULL) {
            i++;
            cmd[i] = strtok(NULL, " ");
        }


        if (strcmp(cmd[0], "utentes") == 0)
        {
            printListaEspera(); 
        }
        if (strcmp(cmd[0], "especialistas") == 0)
        {
            // lista dos especialistas ligados, em consulta ou nao e a sua especialidade
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








void shutdown()  ///// FALTA 
{
    fprintf(stdout, "O balcao vai encerrar... \n ");

    // encerra o classificador
    utente u;
    strcpy(u.sintomas, "#fim");
    classifica(u);

    fflush(stdout);
    fflush(stdin);

    // unlink pipes
    close(fd_balcao);
    unlink(FIFO_BALCAO);
    remove(FIFO_BALCAO);

    unlink(FIFO_BALCAO_AUX);
    remove(FIFO_BALCAO_AUX);

    // envia sinal para todos os clientes encerrarem

    // envia sinal para todos os especialistas encerrarem
    exit(0);
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
    }else{
        maxclientes = 5;
    }

    if (getenv("MAXMEDICOS") != NULL)
    {
        a = atoi(getenv("MAXMEDICOS"));
        maxmedicos = a;

    }else{
        maxmedicos = 5; 
    }
}

void isAlreadyRunning()
{
    // se consegue detetar um pipe aberto, termina
    if (access(FIFO_BALCAO, F_OK) != -1)
    {
        fprintf(stderr, "BALCAO ALREADY RUNNING");
        shutdown();
    }

}

int begin()
{
    isAlreadyRunning();
    environmentVariables();
    showEnvironmentVariables();

    return 0;
}



