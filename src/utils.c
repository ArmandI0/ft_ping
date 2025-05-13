#include "../ft_ping.h"

void printAddrInfoSimple(struct addrinfo *info) {
    if (info == NULL) {
        printf("addrinfo NULL\n");
        return;
    }

    // Flags
    printf("Flags: 0x%x\n", info->ai_flags);
    
    // Family (IPv4 or IPv6)
    printf("Famille: %s\n", info->ai_family == AF_INET ? "IPv4" : 
                           (info->ai_family == AF_INET6 ? "IPv6" : "Autre"));
    
    // Socket type
    printf("Type de socket: %s\n", info->ai_socktype == SOCK_STREAM ? "SOCK_STREAM" : 
                                  (info->ai_socktype == SOCK_DGRAM ? "SOCK_DGRAM" : "Autre"));
    
    // Protocol
    printf("Protocole: %s\n", info->ai_protocol == IPPROTO_TCP ? "TCP" : 
                            (info->ai_protocol == IPPROTO_UDP ? "UDP" : "Autre"));
    
    // Canonical name (if available)
    if (info->ai_canonname != NULL) {
        printf("Nom canonique: %s\n", info->ai_canonname);
    }
    
    // Extract and print the address
    char ip_str[46];
    void *addr;
    
    if (info->ai_family == AF_INET) {
        // IPv4
        struct sockaddr_in *ipv4 = (struct sockaddr_in *)info->ai_addr;
        addr = &(ipv4->sin_addr);
        inet_ntop(info->ai_family, addr, ip_str, sizeof(ip_str));
        printf("Adresse: %s\n", ip_str);
        printf("Port: %d\n", ntohs(ipv4->sin_port));
    } else if (info->ai_family == AF_INET6) {
        // IPv6
        struct sockaddr_in6 *ipv6 = (struct sockaddr_in6 *)info->ai_addr;
        addr = &(ipv6->sin6_addr);
        inet_ntop(info->ai_family, addr, ip_str, sizeof(ip_str));
        printf("Adresse: %s\n", ip_str);
        printf("Port: %d\n", ntohs(ipv6->sin6_port));
    }
    
    // Print next addrinfo if exists (recursive)
    if (info->ai_next != NULL) {
        printf("\n--- Prochain addrinfo ---\n");
        printAddrInfoSimple(info->ai_next);
    }
}

char*    printBool(bool arg)
{
    if (arg == true)
        return "true";
    else
        return "false";
}

void    printStructCmd(cmd *command)
{
    printf("verbose = %s\n", printBool(command->verbose));
    printf("help = %s\n", printBool(command->help));
    printf("%p\n", command->addr);
}


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
        return -1;

    // Convertir en millisecondes
    double milliseconds = end.tv_sec * 1000.0 + end.tv_usec / 1000.0;
    return milliseconds;
}