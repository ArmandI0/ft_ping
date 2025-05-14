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
    setSignalAction();
    // printf("FT_PING %s (%s): 56 data bytes\n", command->raw_adress)
    while(g_signal_received)
    {
        createAndSendPacket(command);
        sleep(1);
    }
    printStatistics(command);
    freeAndExit(command, EXIT_SUCCESS);
    return 0;
}