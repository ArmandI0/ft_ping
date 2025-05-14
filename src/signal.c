#include "../ft_ping.h"

void sigHandler(int signal)
{
    (void)signal;
    g_signal_received = 0;
}

void setSignalAction(void)
{
    struct sigaction act;
    bzero(&act, sizeof(act));
    act.sa_handler = &sigHandler;
    sigemptyset(&act.sa_mask);
    act.sa_flags = 0;
    sigaction(SIGINT, &act, NULL);
    sigaction(SIGQUIT, &act, NULL);
}
