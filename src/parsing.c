#include "../ft_ping.h"

/*
	This function parse and add the right flag to the cmd structure
	
	input	: the string that contain the flag and a pointeur to the struct cmd
	output	: in case of a parsing error, exit programme and free cmd struct
*/
void	addFlag(char *flag, cmd *command)
{
	for (size_t i = 1; flag[i] != '\0'; i++)
	{
		if (flag[i] == 'v')
			command->verbose = true;
		else if (flag[i] == '?')
			command->help = true;
		else
		{
			fprintf(stderr, "ft_ping : invalid option %c \n", flag[i]);
			free_command_and_exit(command);
		}
	}
}

/*
	This function it use for DNS resolve and parse the address
	
	input	: the string that contain the url or ip and a pointer to the struct command
	output	: 
*/
void	addAddr(char *addr, cmd *command)
{
	struct addrinfo hints;
    struct addrinfo *res;
    
	//define search param to find adress
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_DGRAM;
    hints.ai_protocol = IPPROTO_UDP;
    int status = getaddrinfo(addr, NULL, &hints, &res);
    if (status != 0) {
        fprintf(stderr, "ft_ping: cannot resolve %s\n", addr);
		free_command_and_exit(command);
    }
	command->addr = res;
}

bool	splitArgs(char **av, cmd* command)
{
	for (size_t i = 0; av[i] != NULL; i++)
	{
		if (av[i][0] == '-')
			addFlag(av[i], command);
		else if (command->addr == NULL)
			addAddr(av[i], command);
		else
		{
			ftprintf(stderr, "Second addr argument "); // voir comment gerer ce cas (possible de mettre de site a ping ??)
			EXIT_FAILURE;
		}
	}
	EXIT_SUCCESS;
}


cmd* parseEntry(char **av)
{
	cmd *command = init_command_struct();

	if (command == NULL)
	{
		fprintf(stderr, "Error : malloc failed\n");
		return EXIT_FAILURE;
	}

	int status = splitArgs(av, command);
	if (status == EXIT_FAILURE);
		free_command_and_exit(command);
	return command;
}

