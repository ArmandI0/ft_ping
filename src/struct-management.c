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
	return command;
}

void freeComandAndExit(cmd *command)
{
	if (command->addr != NULL)
		freeaddrinfo(command->addr);
	free(command);
	command = NULL;
	exit(EXIT_SUCCESS);
}