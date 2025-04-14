#include "../ft_ping.h"

icmphdr* preparePacket(void)
{
    // Prépare l'en-tête ICMP (Echo Request)
    struct icmphdr *icmp = (struct icmphdr *)packet;

    // Type de commande ICMP
    icmp->type = ICMP_ECHO;
    // Code associe a la commande ICMP voir : https://en.wikipedia.org/wiki/Internet_Control_Message_Protocol
    icmp->code = 0;
    // Identifiant pour permettre d'identifier la reponse : Generalement le pid()
    icmp->un.echo.id = htons(getpid());
    // C'est le numero du ping dans la sequence de ping commence generalement a 1
    icmp->un.echo.sequence = htons(1);  // Numéro de séquence
    // Resultat d'un calcul de somme des informations du packet pour verifier l'integrite
    icmp->checksum = 0; // initialisation a 0 avant de passer dans la fonction qui fera la somme

    return
}
