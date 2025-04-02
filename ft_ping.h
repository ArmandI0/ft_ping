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


typedef struct command
{
	bool		verbose;	// -v
	bool		help;	// -?
	struct addrinfo	*addr; //list of address
}				cmd;

cmd* init_command_struct(void);


#endif