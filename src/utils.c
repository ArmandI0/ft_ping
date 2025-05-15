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