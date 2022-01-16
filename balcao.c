

#include "utils.h"

int main(int argc, char **argv)
{

   if( isAlreadyRunning() != 1 ){

    environmentVariables();
    showEnvironmentVariables();
    running();

   }

    shutdown(); 

}