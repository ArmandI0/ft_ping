#include "../ft_ping.h"

cmd* init_command_struct(void)
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

void free_command_and_exit(cmd *command)
{
	if (command->addr != NULL)
		freeaddrinfo(command->addr);
	free(command);
	command = NULL;
	exit(EXIT_SUCCESS);
}