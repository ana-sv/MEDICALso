#include "medico_utils.h"

 #define NOMES 30
 #define SINTOMAS 100


int main(int argc, char **argv)
{


    if(argc != 3){
        perror("ERRO NA QT DE ARGUMENTOS");
        exit(EXIT_FAILURE);
    }

    char nome[NOMES], especialidade[NOMES];
    char str[100];

    char nomeUtente[NOMES], sintomas[SINTOMAS];
    int pidUtente=0;

     // nome e sintomas
    strcpy(nome,argv[1]);
    strcpy(especialidade,argv[2]); 

    fprintf(stdout,"[ Bem-vindo Dr. %s ]\n", nome);

    configPipeBalcao();
    // sinal de alarme a cada 10 segundos , sinais alarm(10); ??

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


    return (EXIT_SUCCESS);
}