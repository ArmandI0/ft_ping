#include "../ft_ping.h"

int main(int ac, char** av)
{
    if (ac < 2)
    {
        fprintf(stderr, "Error : %s \n", USAGE);
        exit(EXIT_FAILURE);
    }

    cmd* command = parseEntry(av);
    printStructCmd(command);
    return 0;
}