#include "../ft_ping.h"

void print_addrinfo_simple(struct addrinfo *info) {
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
    char ip_str[46]; // Big enough for IPv4 or IPv6
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
        print_addrinfo_simple(info->ai_next);
    }
}

int main(int ac, char** av)
{
    if (ac < 2)
    {
        fprintf(stderr, "Invalid number args \n");
        exit(EXIT_FAILURE);
    }
    struct addrinfo hints;
    struct addrinfo *res;
    
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_DGRAM;
    hints.ai_protocol = IPPROTO_UDP;

    int status = getaddrinfo(av[1], NULL, &hints, &res);
    if (status != 0) {
        fprintf(stderr, "getaddrinfo error: %s\n", gai_strerror(status));
        exit(EXIT_FAILURE);
    }
    // print_addrinfo_simple(&hints);
	print_addrinfo_simple(res);

    freeaddrinfo(res);

    return 0;
}