#include "balcao_utils.h"


int maxclientes;
int maxmedicos;

int stopRunning = 0;

// Comunicacao com classificador
int fd_in[2], fd_out[2];



void running()
{
    if (configClassificador() == 1)
    {
        shutdown();
        return;
    }

    char str[100];



    while (!stopRunning)
    {

  
        /// comandos vao aqui 


        //////// Só para a Meta 1
        printf("Insira os sintomas que apresenta: ");
        fgets(str, sizeof(str) - 1, stdin);

        if (strcmp(str, "encerra\n") == 0)
        {
            stopRunning=1;
            return;
        
        }
       
        classifica(str);


    }

}

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
    fprintf(stdout, "O balcao vai encerrar! \n ");
    // enviar sinal aos clientes/medicos para terminarem ??!?
}

int isAlreadyRunning()
{
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