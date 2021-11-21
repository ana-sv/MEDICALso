#include "balcao_utils.h"

int main(int argc, char **argv)
{

    if (begin() == 1)
    {
        fprintf(stderr, "BEGIN FUNCTION ERROR");
        return 0;
    }


   running();

    return (EXIT_SUCCESS);
}
