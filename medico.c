#include "utils.h"

//Comunicação Named Pipes
int fd_cliente, fd_balcao;


int main(int argc, char **argv)
{

    especialista esp;
    utente u;
    int res;

    if(argc != 3){
        perror("ERRO NA QT DE ARGUMENTOS");
        exit(EXIT_FAILURE);
    }


      // se nao consegue detetar um pipe aberto, termina
    if (access(FIFO_BALCAO_MEDICO, F_OK) == -1)
    {
        fprintf(stderr, "balcao indesponivel");
        exit(1);
    }


    // info especialista 
    strcpy(esp.nome,argv[1]);
    strcpy(esp.especialidade,argv[2]); 
    esp.pid = getpid();
    sprintf(esp.fifoNome, FIFO_MEDICO, getpid()); // nome do fifo deste medico
    esp.emConsulta=-1; // nao esta em consulta

    
    fprintf(stdout,"[ Bem-vindo Dr. %s ]\n", esp.nome);
    // sinal de alarme a cada 10 segundos , sinais alarm(10); ?? FALTA


    // cria fifo medico
     res = mkfifo(esp.fifoNome, 0666);
    if (res == -1)
    {
        perror("\nErro criar fifo medico");
        exit(1);
    }


    // abre fifo balcao
    fd_balcao = open(FIFO_BALCAO_MEDICO, O_RDWR);
    if (fd_balcao == -1)
    {
        perror("\nAbrir fifo balcao");
        exit(1);
    }

    res = write(fd_balcao, &esp, sizeof(esp));
    fprintf(stdout, " %d bytes de informação enviada ao balcao, aguarde... ", res);


    sleep(20); // como nao tem mais, só para evitar que termine logo 

    // recebe struct utente do balcao

    // dialogo com um utente 



    close(fd_balcao);
    remove(esp.fifoNome);


    return (EXIT_SUCCESS);
    
}