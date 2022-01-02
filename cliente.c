#include "cliente_utils.h"


int configPipe(){

}

int sendTo(int pid){



}




int main(int argc, char **argv)
{


    if(argc != 2){
        perror("ERRO NA QT DE ARGUMENTOS");
        exit(EXIT_FAILURE);
    }

   
    utente u, aux;
    char str[100];

     // nome e sintomas
    strcpy(u.nome,argv[1]); 

    fprintf(stdout,"[ UTENTE : %s ]\n", u.nome);
    fprintf(stdout,"Indique os seus sintomas: ");
    fflush(stdin);
    fgets(u.sintomas,sizeof(u.sintomas)-1, stdin );

    // envia sintomas 
    int pid;

    configPipeBalcao();
    sendTo(pid, );
    reciveFrom(pid) ; 
    configPipeMedico();

    do{
    // recebe especialidade e medicos disponiveis dessa especialidade, prioridade, lugar
    // mandar os nomes dos medicos numa string ?? chega ??
    fprintf...

    // aguarda consulta aka resposta do balcao ... irá ser atendido pelo medico x com o pid y 
    reciveFrom(pid,);

    // dialogo com o medico
    reciveFrom(pid,);
    
    fflush(stdin);
    fgets(str,sizeof(str)-1, stdin );

    sendTo(pid,);

    }while(strcmp(str,"sair")!=0);
    // avisa balcao que irá sair 


    // termina consulta, termina prog cliente ???


    return (EXIT_SUCCESS);
}