#include "../ft_ping.h"

volatile sig_atomic_t g_signal_received = 1;

int main(int ac, char **av)
{
    if (ac < 2)
    {
        fprintf(stderr, "Error : %s \n", USAGE);
        exit(EXIT_FAILURE);
    }
    cmd* command = parseEntry(av);

    setSignalAction(); // Pour gerer les signaux dans les differrentes boucles

    while(g_signal_received)
    {
        createAndSendPacket(command);
        sleep(1);
    }
    printStatistics(command);
    freeAndExit(command, EXIT_SUCCESS);
    return 0;
}