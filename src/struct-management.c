#include "../ft_ping.h"

cmd* initCommandStruct(void)
{
	cmd *command;
	command = malloc(sizeof(cmd));
	if (!command)
		return NULL;
	command->verbose = false;
	command->help = false;
	command->addr = NULL;
	command->packet = NULL;
	command->socket = -1;
    memset(&command->start, 0, sizeof(struct timeval));
    memset(&command->end, 0, sizeof(struct timeval));
	return command;
}

void freeAndExit(cmd *command, int exit_code)
{
    if (command != NULL) {
        if (command->addr != NULL)
            freeaddrinfo(command->addr);
        if (command->packet != NULL)
            free(command->packet);
        free(command);
    }
    exit(exit_code);
}