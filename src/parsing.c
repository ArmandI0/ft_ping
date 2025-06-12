#include "../ft_ping.h"

/*
	This function parse and add the right flag to the cmd structure
	
	input	: the string that contain the flag and a pointeur to the struct cmd
	output	: in case of a parsing error, exit programme and free cmd struct
*/
void addFlag(char *flag, cmd *command)
{
	// -- options
    if (strncmp(flag, "--ttl=", 6) == 0)
    {
        char *endptr;
        long ttl = strtol(flag + 6, &endptr, 10);
        if (*endptr == '\0' && ttl > 0 && ttl < 256)
        {
            command->ttl = ttl;
        }
        else
        {
            fprintf(stderr, "ft_ping: invalid TTL value '%s'\n", flag + 6);
            freeAndExit(command, EXIT_FAILURE);
        }
        return;
    }
    // - options
    for (size_t i = 1; flag[i] != '\0'; i++)
    {
        if (flag[i] == 'v')
        {
            command->verbose = true;
        }
        else if (flag[i] == '?')
        {
            command->help = true;
            printf("%s\n", USAGE);
            printf("Send ICMP ECHO_REQUEST packets to network hosts.\n\n");
            printf("-?                      give this help list\n");
            printf("--ttl=N                 specify N as time-to-live\n");
            printf("-v                      verbose output\n");
            freeAndExit(command, EXIT_SUCCESS);
        }
        else
        {
            fprintf(stderr, "ft_ping: unrecognized option '%s'\n", flag);
            fprintf(stderr, "Try 'ft_ping --help' for more information.\n");
            freeAndExit(command, EXIT_FAILURE);
        }
    }
}

bool is_ip_adress(const char *str)
{
    struct sockaddr_in	sa;
    int 				result = inet_pton(AF_INET, str, &(sa.sin_addr));
    if(result == 1)
	{
        return true;
    }
    return false;
}

/*
	This function it use for DNS resolve and parse the address
	
	input	: the string that contain the url or ip and a pointer to the struct command
	output	: exit and free
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
        fprintf(stderr, "ft_ping: unknown host\n");
		freeAndExit(command, EXIT_FAILURE);
    }
	command->addr = res;
	command->raw_adress = addr;
}


bool	splitArgs(char **av, cmd* command)
{
	for (size_t i = 1; av[i] != NULL; i++)
	{
		if (av[i][0] == '-' && av[i][1] != '\0')
			addFlag(av[i], command);
		else if (command->addr == NULL)
			addAddr(av[i], command);
		else
		{
			fprintf(stderr, "Only one destination are required\n"); // voir comment gerer ce cas (possible de mettre de site a ping ??)
			return EXIT_FAILURE;
		}
	}
	return EXIT_SUCCESS;
}

cmd* parseEntry(char **av)
{
	cmd *command = initCommandStruct();
	if (command == NULL)
	{
		perror("malloc fail");
		return NULL;
	}

	int status = splitArgs(av, command);

	if (status == EXIT_FAILURE)
		freeAndExit(command, EXIT_FAILURE);
	return command;
}

