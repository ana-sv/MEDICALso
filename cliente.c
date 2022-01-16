#include "utils.h"

//Comunicação com o balcao, identificadores dos Named Pipes
int fd_cliente, fd_balcao;


int main(int argc, char **argv)
{

    if (argc != 2)
    {
        perror("ERRO NA QT DE ARGUMENTOS");
        exit(EXIT_FAILURE);
    }

    // se nao consegue detetar um pipe aberto, termina
    if (access(FIFO_BALCAO, F_OK) == -1)
    {
        fprintf(stderr, "balcao indespinivel");
        exit(1);
    }

    utente u, aux;
    char str[100];
    int pid, verify;

    // recolhe nome do utente
    strcpy(u.nome, argv[1]);
    u.pid = getpid();
    u.prioridade = 0;
    strcpy(u.especialidade, "indefinida");
    u.lugarFila = -1;                            // -1 ainda nao colocado , 0 para em consulta
    sprintf(u.fifoNome, FIFO_CLIENTE, getpid()); // nome do fifo deste cliente

    //indica sintomas do utente
    fprintf(stdout, "[ UTENTE %d : %s ]\n", u.pid, u.nome);
    fprintf(stdout, "Indique os seus sintomas: ");
    fflush(stdin);
    fgets(u.sintomas, sizeof(u.sintomas) - 1, stdin);

    // cria fifo cliente
    int res = mkfifo(u.fifoNome, 0666);
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

    // envia nome, pid e sintomas do utente ao balcao
    res = write(fd_balcao, &u, sizeof(u));
    fprintf(stdout, " &d bytes de informação enviada ao balcao, aguarde... ", &res);

    fd_cliente = open(u.fifoNome, O_RDWR);
    if (fd_cliente == -1)
    {
        perror("\nAbrir fifo balcao");
        exit(1);
    }

    // recebe struct utente preenchida 
    verify = read(fd_cliente, &aux, sizeof(aux));
    if (verify != sizeof(aux))
    {
        fprintf(stderr, "\nERRO NA LEITURA");
        exit(1);
    }
    u = aux;

    fprintf(stdout, "\n[UTENTE] %s", u.nome);
    fprintf(stdout, "\nEspecialidade:  %s", u.especialidade);
    fprintf(stdout, "\nPrioridade:  %d", u.prioridade);
    fprintf(stdout, "\nAguarde atendimento médico....");

/*
    do{
    // aguarda consulta... irá ser atendido pelo medico x com o pid y 
    
    // dialogo com o medico
    
    fflush(stdin);
    fgets(str,sizeof(str)-1, stdin );
    sendTo(pid,);
    }while(strcmp(str,"sair")!=0);
    // avisa balcao que irá sair 
    // termina consulta, termina prog cliente ???
    */

    close(fd_cliente);
    unlink(u.fifoNome);
    remove(u.fifoNome);

    return (EXIT_SUCCESS);
}