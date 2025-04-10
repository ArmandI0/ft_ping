#include "../ft_ping.h"

icmphdr* preparePacket(void)
{
    // Prépare l'en-tête ICMP (Echo Request)
    struct icmphdr *icmp = (struct icmphdr *)packet;
    icmp->type = ICMP_ECHO;
    icmp->code = 0;
    icmp->un.echo.id = htons(1234);     // Identifiant arbitraire
    icmp->un.echo.sequence = htons(1);  // Numéro de séquence
    icmp->checksum = 0;
}
