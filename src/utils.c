#include "../ft_ping.h"


char *convertIpToString(struct addrinfo *addr)
{
    void *ip;
    char ipstr[INET6_ADDRSTRLEN];

    if (addr->ai_family == AF_INET) {
        struct sockaddr_in *ipv4 = (struct sockaddr_in *)addr->ai_addr;
        ip = &(ipv4->sin_addr);
    } else {
        struct sockaddr_in6 *ipv6 = (struct sockaddr_in6 *)addr->ai_addr;
        ip = &(ipv6->sin6_addr);
    }
    inet_ntop(addr->ai_family, ip, ipstr, sizeof(ipstr));
    return strdup(ipstr);
}


double getTimeInMs(void)
{
    struct timeval end;
    int status = gettimeofday(&end, NULL);

    if (status != 0)
    {
        perror("gettimeofday");
        return -1;
    }
    double milliseconds = end.tv_sec * 1000.0 + end.tv_usec / 1000.0;
    return milliseconds;
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