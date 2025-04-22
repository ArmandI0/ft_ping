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
    int sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);   // SOCK_RAW -> Socket brute pour tous les protocoles + filtre ICMP
    if (sockfd < 0)
    {
        perror("socket");
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
    gettimeofday(command->start, NULL);
    printf("FT_PING %s (%s) 56(%d) bytes of data.\n", command->raw_adress, convertIpToString(command->addr), status + 20);
    printf("start time = %d", command->start);
}

void recvPacket(cmd *command)
{
    int         status = 0;
    char        buffer_rcv[64];
    socklen_t   addr_len = command->addr->ai_addrlen;

    status = recvfrom(command->socket, buffer_rcv, sizeof(buffer_rcv), 0, command->addr->ai_addr, &addr_len);
    if (status == -1)
    {
        perror("recvfrom");
        freeAndExit(command, EXIT_FAILURE);
    }
    struct iphdr *ip_header = (struct iphdr *)buffer_rcv;
    printf("ip_header->ttl = %d\n", ip_header->ttl);
    printf("ip_header->ttl = %d\n", ip_header->saddr);


    for (int i = 0; i < 64; i++)
    {
        write(1, &buffer_rcv[i], 1);
    }
    printf("\n");
    printf("taille = %d", status);
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