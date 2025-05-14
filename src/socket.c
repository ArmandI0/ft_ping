#include "../ft_ping.h"


/*
    Input: pointer to the packet, sizeof(packet)
    Return: checksum result
*/
uint16_t checksum(void *buffer, size_t length) {
    uint16_t *data = (uint16_t *)buffer;
    uint32_t sum = 0;

    while (length > 1) {
        sum += *data++;
        length -= 2;
    }
    while (sum >> 16) {
        sum = (sum & 0xFFFF) + (sum >> 16);
    }

    return (uint16_t)(~sum);
}


char* preparePacket(void)
{
    static int  seq = 0;
    int         packet_len = 64;
    char        *packet = malloc(packet_len);
    
    if (packet == NULL)
        return NULL;
    memset(packet, 0, packet_len);
    struct icmphdr *icmp = (struct icmphdr *)packet;        // Prépare l'en-tête au format ICMP
    icmp->type = ICMP_ECHO;                                 // Type de commande ICMP
    icmp->code = 0;                                         // Code associe a la commande ICMP voir : https://en.wikipedia.org/wiki/Internet_Control_Message_Protocol
    icmp->un.echo.id = htons(getpid());                     // Identifiant pour permettre d'identifier la reponse : Generalement le pid()
    icmp->un.echo.sequence = htons(seq);                    // C'est le numero du ping dans la sequence de ping commence generalement a 1
    icmp->checksum = 0;
    icmp->checksum = checksum(packet, packet_len);          // Resultat d'un calcul de somme des informations du packet pour verifier l'integrite
    seq++;
    return packet;
}

void    createSocket(cmd *command)
{
    int sockfd = -1;
    
    sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);   // SOCK_RAW -> Socket brute pour tous les protocoles + filtre ICMP
    if (sockfd < 0)
    {
        perror("socket");
        freeAndExit(command, EXIT_FAILURE);
    }
    int flags = fcntl(sockfd, F_GETFL, 0);
    if (flags == -1 || fcntl(sockfd, F_SETFL, flags | O_NONBLOCK) == -1) {
        perror("fcntl - non-blocking");
        freeAndExit(command, EXIT_FAILURE);
    }
    command->socket = sockfd;
}

void    sendPacket(cmd *command)
{
    int status = 0;
    status = sendto(command->socket, command->packet, 64, 0, command->addr->ai_addr, command->addr->ai_addrlen);
    if (status == -1)
    {
        perror("sendto");
        freeAndExit(command, EXIT_FAILURE);
    }
    free(command->packet);
    command->packet = NULL;
    long start_time = getTimeInMs();
    if (start_time != -1)
        command->start_time = start_time;
    command->nb_of_transmitted_packets++;
}

int getnameinfo_recv(int argument, struct sockaddr_storage recv_address, socklen_t addr_len, char *host)
{
    char    service[NI_MAXSERV];

    int status = getnameinfo((struct sockaddr *)&recv_address, addr_len,
                          host, NI_MAXHOST,
                          service, NI_MAXSERV,
                          argument | NI_NUMERICSERV);
    return status;
}

void parseRawPacket(char *buffer, cmd *command)
{
    // Extract IP_HEADER
    struct iphdr    *ip_header = (struct iphdr *)buffer;
    int             ip_header_lenght = ip_header->ihl * 4;
    int             ip_header_ttl = ip_header->ttl;

    // Recuperer Source Address dans IP_HEADER
    char            ip_str[INET_ADDRSTRLEN];
    struct in_addr  src_ip;

    src_ip.s_addr = ip_header->saddr;
    inet_ntop(AF_INET, &src_ip, ip_str, sizeof(ip_str));

    // Extract ICMP_HEADER
    struct  icmphdr *icmp_header = (struct icmphdr *)(buffer + ip_header_lenght);
    int             sequence_number;
    sequence_number = ntohs(icmp_header->un.echo.sequence);

    // Calculer temps de ping
    double    end_time = getTimeInMs();
    if (end_time == -1)
    {
        perror("gettime");
        freeAndExit(command, EXIT_FAILURE);
    }
    double    time = end_time - command->start_time;

    // Sauvegarder le packet
    packet  *new_packet = createPacket(buffer, time);
    appendPacket(&command->packets, new_packet);

    if (command->print_hostname == false)
    {
        // Recherche DNS Inverse
        struct sockaddr_in  addr;
        char                hostname[NI_MAXHOST];
    
        memset(&addr, 0, sizeof(addr));
        addr.sin_family = AF_INET;
        addr.sin_addr = src_ip;
    
        int ret = getnameinfo(
            (struct sockaddr *)&addr,
            sizeof(addr),
            hostname, sizeof(hostname),
            NULL, 0, NI_NAMEREQD
        );
        if (ret != 0)
        {
            perror("getnameinfo");
            freeAndExit(command, EXIT_FAILURE);
        }
        printf("64 bytes from %s (%s): icmp_seq=%d ttl=%d time=%.1f ms\n", hostname, ip_str, sequence_number, ip_header_ttl, time);
    }
    else
    {
        printf("64 bytes from %s : icmp_seq=%d ttl=%d time=%.1f ms\n", ip_str, sequence_number, ip_header_ttl, time);
    }

}

void recvPacket(cmd *command)
{
    int                     status = 0;
    bool                    timeout = true;
    char                    buffer_rcv[512];    //Contient le paquet brut [IP header][ICMP header][ICMP data...]
    struct sockaddr_storage recv_address;
    socklen_t               addr_len = sizeof(recv_address);

    memset(&recv_address, 0, sizeof(recv_address));
    while ((getTimeInMs() - command->start_time) < TIMEOUT && g_signal_received) // et supp a 0
    {
        status = recvfrom(
            command->socket,
            buffer_rcv,
            sizeof(buffer_rcv),
            0,
            (struct sockaddr *)&recv_address,
            &addr_len
        );
        if (status != 0)
        {
            if (status == -1)
            {
                if (errno == EWOULDBLOCK || errno == EAGAIN)
                {
                    usleep(10000);
                    continue;
                }
                else
                {
                    perror("recvfrom");
                    freeAndExit(command, EXIT_FAILURE);
                }
            }
            timeout = false;
            break;
        }
    }
    if (timeout == false)
    {
        parseRawPacket(buffer_rcv, command);
        command->nb_of_received_packets++;
    }
}

void    createAndSendPacket(cmd *command)
{
    command->packet = preparePacket();
    if (command->packet == NULL)
    {
        perror("malloc");
        freeAndExit(command, EXIT_FAILURE);
    }
    createSocket(command);
    sendPacket(command);
    recvPacket(command);
}