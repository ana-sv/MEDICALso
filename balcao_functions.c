#include "balcao_utils.h"

int maxclientes;
int maxmedicos;

int stopRunning = 0;

// Comunicacao com classificador
int fd_in[2], fd_out[2];

//Comuicação com Clientes 


//Comunicao com Medicos


void running()
{
    // thread a fazer a recolha dos medico e utentes ligados idk 
    // e a receber o aviso/alarme que os medicos x y x ainda continuam ligados ?!
    // vai atualizando uma lista 


    if (configClassificador() == 1)
    {
        shutdown();
        return;
    }

    while (!stopRunning)
    {

        char cmd[100];
        char str1[49], str2[49];

        // recebe comando e "corta-o" pelo espaço


        if (strcmp(str1, "encerra") == 0)
        {
            stopRunning = 1;
            return;
        }
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

    }







}



int configCliente(){
    //usamos named pipes
    // bloqueantes idk





}



// NAO MEXE
void classifica(char str[100])
{

    char str_temp[100];
    int readAux = 0;

    // envia ao classificador
    write(fd_in[1], str, strlen(str));

    // recebe do classificador
    readAux = read(fd_out[0], str_temp, sizeof(str_temp) - 1);
    if (readAux == -1)
    {
        fprintf(stderr, "READ FROM CLASSIFICADOR");
        return;
    }
    str_temp[readAux] = '\0';

    fprintf(stdout, "%s", str_temp);
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
    classifica("#fim");

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