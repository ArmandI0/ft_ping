#include "../ft_ping.h"


/*
    Input: pointer to the packet, sizeof(packet)
    Return: checksum result

    Description: Calcul le checksum du packet cree, utilisé pour vérifier l'intégrité des paquets ICMP.
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


/*
    Input: cmd *command
    Return: pointer to the prepared packet

    Description: Alloue et initialise un paquet ICMP avec un en-tête ICMP
*/
char* preparePacket(cmd *command)
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
    uint16_t pid = getpid() & 0xFFFF;
    icmp->un.echo.id = htons(pid);                          // Identifiant pour permettre d'identifier la reponse : Generalement le pid()
    icmp->un.echo.sequence = htons(seq);                    // C'est le numero du ping dans la sequence de ping commence generalement a 1
    icmp->checksum = 0;
    icmp->checksum = checksum(packet, packet_len);          // Resultat d'un calcul de somme des informations du packet pour verifier l'integrite
    if (seq == 0)
        printHeader(command, pid);
    seq++;

    return packet;
}


/*
    Input: cmd *command
    Description: Crée un socket brut configuré pour le protocole ICMP.
    Plus configurer la socket pour ne pas bloquer a recvmfrom
*/
void    createSocket(cmd *command)
{
    int sockfd = -1;
    
    sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);   // SOCK_RAW -> Socket brute pour tous les protocoles + filtre ICMP
    if (sockfd < 0)
    {
        perror("socket");
        freeAndExit(command, EXIT_FAILURE);
    }

    // Pour que les fonctions comme recvfrom soit non bloquante
    int flags = fcntl(sockfd, F_GETFL, 0);

    if (flags == -1 || fcntl(sockfd, F_SETFL, flags | O_NONBLOCK) == -1) {
        perror("fcntl - non-blocking");
        freeAndExit(command, EXIT_FAILURE);
    }

    // Modifier le time to leave
    int ttl = command->ttl;

    if (setsockopt(sockfd, IPPROTO_IP, IP_TTL, &ttl, sizeof(ttl)) < 0) {
        perror("setsockopt");
    }
    command->socket = sockfd;
}


/*
    Input: cmd *command

    Description: Envoie le paquet ICMP
*/
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


/*
    Input: buffer (paquet brut), cmd *command, taille du paquet reçu

    Description: Analyse le paquet brut pour extraire les en-têtes IP et ICMP.
*/
void parseRawPacket(char *buffer, cmd *command, int size_recv)
{
    struct print_infos  data;

    // Extract IP_HEADER and infos
    struct iphdr    *ip_header = (struct iphdr *)buffer;
    int             ip_header_lenght = ip_header->ihl * 4;

    data.ip_header = ip_header;
    data.bytes_recv = size_recv - ip_header_lenght;
    data.ttl = ip_header->ttl;

    // Recuperer Source Address dans IP_HEADER
    char            ip_str[INET_ADDRSTRLEN];
    struct in_addr  src_ip;
    
    src_ip.s_addr = ip_header->saddr;
    inet_ntop(AF_INET, &src_ip, ip_str, sizeof(ip_str));

    strncpy(data.ip_str, ip_str, INET_ADDRSTRLEN);

    // Extract ICMP_HEADER
    struct  icmphdr *icmp_header = (struct icmphdr *)(buffer + ip_header_lenght);

    // En cas d'erreur le packet ICMP est encapsule dans un autre packet ICMP
    if (icmp_header->type != ICMP_ECHOREPLY) {
        // Access the encapsulated IP header
        struct iphdr *encapsulated_ip_header = (struct iphdr *)(buffer + ip_header_lenght + sizeof(struct icmphdr));
        int encapsulated_ip_header_length = encapsulated_ip_header->ihl * 4;

        // Access the encapsulated ICMP header
        struct icmphdr *encapsulated_icmp_header = (struct icmphdr *)((char *)encapsulated_ip_header + encapsulated_ip_header_length);

    data.sequence_number = ntohs(encapsulated_icmp_header->un.echo.sequence);
    data.icmp_header = encapsulated_icmp_header;
    data.icmp_type = encapsulated_icmp_header->type;
    } 
    else
    {
        data.sequence_number = ntohs(icmp_header->un.echo.sequence);
        data.icmp_header = icmp_header;
        data.icmp_type = icmp_header->type;
    }

    // Calculer temps de ping
    double    end_time = getTimeInMs();
    if (end_time == -1)
    {
        perror("gettime");
        freeAndExit(command, EXIT_FAILURE);
    }
    data.time = end_time - command->start_time;

    // Save new_packet dans la list
    packet  *new_packet = createPacket(buffer, data.time);
    appendPacket(&command->packets, new_packet);
    print_result(data, command);
}

/*
    Input: cmd *command

    Description: Attend la réception d'un paquet ICMP via le Raw socket
*/
void recvPacket(cmd *command)
{
    int                     status = 0;
    bool                    timeout = true;
    char                    buffer_rcv[512];    //Contient le paquet brut [IP header][ICMP header][ICMP data...]
    struct sockaddr_storage recv_address;
    socklen_t               addr_len = sizeof(recv_address);

    memset(&recv_address, 0, sizeof(recv_address));
    while ((getTimeInMs() - command->start_time) < TIMEOUT && g_signal_received)
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
        parseRawPacket(buffer_rcv, command, status);
        command->nb_of_received_packets++;
    }
}

/*
    Input: cmd *command

    Description: fonction principal de ping : Prépare un paquet ICMP, crée un socket brut, envoie le paquet
    et attend une réponse.
*/
void    createAndSendPacket(cmd *command)
{
    command->packet = preparePacket(command);
    if (command->packet == NULL)
    {
        perror("malloc");
        freeAndExit(command, EXIT_FAILURE);
    }
    createSocket(command);
    sendPacket(command);
    recvPacket(command);
}