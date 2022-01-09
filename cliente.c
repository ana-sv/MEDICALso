#include "utils.h"


//Comunicação com o balcao, identificadores dos Named Pipes
int fd_cliente, fd_balcao;

//Named pipe deste cliente
char fifoCliente[25];



// criar o fifo do cliente 
void configFIFOcliente(){

    sprintf(fifoCliente, FIFO_CLIENTE, getpid() );
    if(mkfifo(fifoCliente,0777) == -1 ){
        fprintf(stderr, "FIFO CLIENTE ");
        exit(1);
    }
    fprintf(stderr,"\n %s criado com sucesso!\n", fifoCliente );

}


// abrir fifo cliente read+write
void openFIFOcliente(){
    fd_cliente = open(fifoCliente, O_RDWR);
        if( fd_cliente == -1 ){
        fprintf(stderr, "ABRIR FIFO CLIENTE ");
        close(fd_balcao);
        unlink(FIFO_CLIENTE);
        exit(1);
    }
    fprintf(stderr,"\n Fifo cliente aberto com sucesso !\n", fifoCliente );


}



int main(int argc, char **argv)
{


    if(argc != 2){
        perror("ERRO NA QT DE ARGUMENTOS");
        exit(EXIT_FAILURE);
    }

    configFIFOcliente();


    utente u, aux;
    msg sms;
    char str[100];
    int pid, verify;


     // recolhe nome do utente
    strcpy(u.nome,argv[1]);
    u.pid = getpid();
    u.prioridade = 0;
    strcpy(u.especialidade,"indefinida");
    u.lugarFila= -1; // -1 ainda nao colocado , 0 para em consulta

    //indica sintomas do utente

    fprintf(stdout,"[ UTENTE %d : %s ]\n",u.pid, u.nome);
    fprintf(stdout,"Indique os seus sintomas: ");
    fflush(stdin);
    fgets(u.sintomas,sizeof(u.sintomas)-1, stdin );

  
   // openFIFObalcao();
    fd_balcao = open(FIFO_BALCAO,O_WRONLY);
    if(fd_balcao == -1 ){
        perror("\nAbrir fifo balcao");
        exit(1);
    }

    openFIFOcliente();

    // envia nome, pid e sintomas do utente ao balcao
    write(fd_balcao, &u, sizeof(u) );
    fprintf(stdout,"Informação enviada ao balcao, aguarde... ");


    // a proxima info que recebe já deveria ser a do seu par 




    /*

    verify = read(fd_balcao, &aux, sizeof(aux));
    if(verify != sizeof(aux)){
        fprintf(stderr, "\nERRO NA LEITURA");
        exit(1);   
    }
    u = aux;
    fprintf(stdout,"\nEspecialidade: %s\nPrioridade: %d\nLugar em Fila: %d\n", u.especialidade,u.prioridade, u.lugarFila);
    fprintf(stdout,"Aguarde ser atendido.... ");

    */



    /*

    do{
    // recebe especialidade e medicos disponiveis dessa especialidade, prioridade, lugar
    // mandar os nomes dos medicos numa string ?? chega ??
    fprintf...

    // aguarda consulta aka resposta do balcao ... irá ser atendido pelo medico x com o pid y 
    

    // dialogo com o medico

    
    fflush(stdin);
    fgets(str,sizeof(str)-1, stdin );

    sendTo(pid,);

    }while(strcmp(str,"sair")!=0);
    // avisa balcao que irá sair 




    // termina consulta, termina prog cliente ???

    */

    return (EXIT_SUCCESS);
}