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
    int opt_set_ttl = 64;
    int opt_recv_ttl = 1;
    int sockfd = -1;
    
    sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);   // SOCK_RAW -> Socket brute pour tous les protocoles + filtre ICMP
    if (sockfd < 0)
    {
        perror("socket");
        freeAndExit(command, EXIT_FAILURE);
    }
    if (setsockopt(sockfd, IPPROTO_IP, IP_TTL, &opt_set_ttl, sizeof(opt_set_ttl)) < 0)
    {
        perror("setsockopt - IP_TTL");
        freeAndExit(command, EXIT_FAILURE);
    }
    if (setsockopt(sockfd, IPPROTO_IP, IP_RECVTTL, &opt_recv_ttl, sizeof(opt_recv_ttl)) < 0)
    {
        perror("setsockopt - IP_RECVTTL");
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
    long start_time = getTimeInMs();
    if (start_time != -1)
        command->start_time = start_time;
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
        return; // Rajouter perror
    double    time = end_time - command->start_time;

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
        ret++; 
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
    char                    buffer_rcv[512];    //Contient le paquet brut [IP header][ICMP header][ICMP data...]
    struct sockaddr_storage recv_address;
    socklen_t               addr_len = sizeof(recv_address);

    memset(&recv_address, 0, sizeof(recv_address));

    status = recvfrom(
        command->socket,
        buffer_rcv,
        sizeof(buffer_rcv),
        0,
        (struct sockaddr *)&recv_address,
        &addr_len
    );

    if (status == -1)
    {
        perror("recvfrom");
        freeAndExit(command, EXIT_FAILURE);
    }

    parseRawPacket(buffer_rcv, command);

    // char host_ip[NI_MAXHOST];
    // char host_name[NI_MAXHOST];
    
    // if (getnameinfo_recv(NI_NUMERICHOST, recv_address, addr_len, host_ip) !=  0)
    // {
    //     fprintf(stderr, "Erreur lors de la récupération de l'adresse IP.\n");
    //     return;
    // }
    
    // if (getnameinfo_recv(NI_NAMEREQD, recv_address, addr_len, host_name) != 0)
    // {
    //     fprintf(stderr, "Erreur lors de la récupération du nom d'hôte.\n");
    //     return;
    // }
    
    // // Affichage du résultat
    // struct icmphdr *icmp = (struct icmphdr *)command->packet;
    // double    end_time = getTimeInMs();
    // if (end_time == -1)
    //     return;
    // double    time = end_time - command->start_time;
    // printf("64 bytes from %s (%s): icmp_seq=%d ttl=0 time=%.1f ms\n", host_name, host_ip, ntohs(icmp->un.echo.sequence), time);
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