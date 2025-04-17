#ifndef ft_ping
# define ft_ping
# include <unistd.h> 
# include <stdbool.h>
# include <stdio.h>
# include <stdlib.h>
# include <string.h>
# include <netdb.h>
# include <sys/types.h>
# include <sys/socket.h>
# include <arpa/inet.h>
# include <netinet/in.h>
# include <netinet/ip_icmp.h>
# define USAGE "Usage: ft_ping [-v] [-?] <destination>"

typedef struct command
{
	bool			verbose;	// -v
	bool			help;	// -?
	char			*packet;
	int				socket;
	struct addrinfo	*addr; //list of address
}				cmd;

cmd*			initCommandStruct(void);
void 			freeAndExit(cmd *command, int exit_code);
void			printStructCmd(cmd *command);
cmd*			parseEntry(char **av);
void    		createAndSendPacket(cmd *command);

#endif