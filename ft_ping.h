#ifndef ft_ping
# define ft_ping
# include <stdbool.h>
# include <stdio.h>
# include <stdlib.h>
# include <string.h>
# include <netdb.h>
# include <sys/types.h>
# include <sys/socket.h>
# include <arpa/inet.h>
# include <netinet/in.h>

# define USAGE "Usage: ft_ping [-v] [-?] <destination>"

typedef struct command
{
	bool			verbose;	// -v
	bool			help;	// -?
	struct addrinfo	*addr; //list of address
}				cmd;

cmd*	initCommandStruct(void);
void 	freeComandAndExit(cmd *command);
void	printStructCmd(cmd *command);
cmd*	parseEntry(char **av);



#endif