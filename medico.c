#include "utils.h"

 #define NOMES 30
 #define SINTOMAS 100

//Comunicação com o balcao, identificadores dos Named Pipes
int fd_balcao;

int main(int argc, char **argv)
{

    especialista esp;
    int res;

    if(argc != 3){
        perror("ERRO NA QT DE ARGUMENTOS");
        exit(EXIT_FAILURE);
    }


      // se nao consegue detetar um pipe aberto, termina
    if (access(FIFO_BALCAO, F_OK) == -1)
    {
        fprintf(stderr, "balcao indespinivel");
        exit(1);
    }

    // info especialista 
    strcpy(esp.nome,argv[1]);
    strcpy(esp.especialidade,argv[2]); 
    esp.pid = getpid();
    sprintf(esp.fifoNome, FIFO_MEDICO, getpid()); // nome do fifo deste medico
    esp.emConsulta=-1; // nao esta em consulta


    fprintf(stdout,"[ Bem-vindo Dr. %s ]\n", esp.nome);

    // cria fifo medico
    int res = mkfifo(esp.fifoNome, 0666);
    if (res == -1)
    {
        perror("\nErro criar fifo");
        exit(1);
    }


    // abre fifo balcao
    fd_balcao = open(FIFO_BALCAO, O_RDWR);
    if (fd_balcao == -1)
    {
        perror("\nAbrir fifo balcao");
        exit(1);
    }

    res = write(fd_balcao, &esp, sizeof(esp));
    fprintf(stdout, " %d bytes de informação enviada ao balcao, aguarde... ", &res);



    // sinal de alarme a cada 10 segundos , sinais alarm(10); ??
    /*
    do{
        fprintf(stdout," Aguarde a sua próxima consulta.... ");
        
        // com um named pipe bloqueantes fica à espera 
        // de receber do balcao nomeUtente, pidUtente, sintomas;
        
        fprintf(stdout,"Colocado em consulta de %s : \n", especialidade );
        fprintf(stdout,"Utente: %s [%d]\n Sintomas: %s\n", nomeUtente , pidUtente, sintomas);        
        configPipeUtente();

        do{
        // dialogo com o paciente
        // named pipe sem bloqueante
        // o medico pode enviar duas perguntas seguidas sem esperar uma resposta

        

        }while(strcmp(str,"adeus")!=0);
        // avisa balcao que terminou a consulta




    }while(strcmp(str,"sair")!=0);
    // avisa o balcao que irá sair 
    */


    return (EXIT_SUCCESS);
}