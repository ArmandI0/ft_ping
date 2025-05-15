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
# include <signal.h>
# include <fcntl.h>
# include <errno.h>
# include <math.h>
# include <float.h>

# define TIMEOUT 1000 //en ms

extern	volatile sig_atomic_t g_signal_received;

# define USAGE "Usage: ft_ping [-v] [-?] <destination>\n"

typedef	struct packet_received
{
	char					*data;
	double					time_to_tramsmit_packet;
    struct packet_received	*next;
    struct packet_received	*prev;

}		packet;

typedef struct command
{
	bool			verbose;	// -v
	bool			help;		// -?
	int				ttl;		// --ttl
	char			*packet;	// stock le packet envoye
	int				socket;		// fd de la raw socket
	int				nb_of_transmitted_packets;
	int				nb_of_received_packets;
	struct addrinfo	*addr;
	char			*raw_adress;
	double			start_time;
	packet			*packets;
}				cmd;	

struct print_infos
{
	int             bytes_recv;
	char            hostname[NI_MAXHOST];
	char            ip_str[INET_ADDRSTRLEN];
	int             sequence_number;
	int				icmp_type;
	int             ttl;
	double          time;
	struct iphdr    *ip_header;
	struct icmphdr  *icmp_header;
};

cmd*			initCommandStruct(void);
void 			freeAndExit(cmd *command, int exit_code);
cmd*			parseEntry(char **av);
void    		createAndSendPacket(cmd *command);
char*			convertIpToString(struct addrinfo *addr);
double 			getTimeInMs(void);
void 			setSignalAction(void);
packet* 		createPacket(char *data, double time_to_transmit_packet);
void 			appendPacket(packet **head, packet *new_packet);
void			printStatistics(cmd *command);
void 			printHeader(cmd *command, u_int16_t pid);
void 			print_result(struct print_infos data, cmd *command);


#endif