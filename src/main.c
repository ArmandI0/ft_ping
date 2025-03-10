#include <sys/socket.h>
#include <stdio.h>

int main(int ac, char** av)
{
    struct addrinfo hints, *res, *p;
    av++;
    if (ac < 2)
    {
        fprintf(stderr, "Invalid number args \n");
    }
    
    int adr = 0;

    adr = getaddrinfo()
    

    return 0;
}