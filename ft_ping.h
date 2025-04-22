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
# include <netinet/ip.h>
# include <netinet/in.h>
# include <netinet/ip_icmp.h>
# include <sys/time.h>
# define USAGE "Usage: ft_ping [-v] [-?] <destination>"

typedef struct command
{
	bool			verbose;	// -v
	bool			help;	// -?
	char			*packet;
	int				socket;
	int				nb_of_transmitted_packets;
	int				nb_of_received_packets;
	struct addrinfo	*addr; //list of address
	char			*raw_adress;
	struct timeval	start;
	struct timeval	end;
}				cmd;

cmd*			initCommandStruct(void);
void 			freeAndExit(cmd *command, int exit_code);
void			printStructCmd(cmd *command);
cmd*			parseEntry(char **av);
void    		createAndSendPacket(cmd *command);
char*			convertIpToString(struct addrinfo *addr);

#endif