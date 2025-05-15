#include "../ft_ping.h"

void printIpHeaderDump(struct iphdr *ip_header)
{
    unsigned char *bytes = (unsigned char *)ip_header;

    printf("IP Hdr Dump:\n");
    for (size_t i = 0; i < sizeof(struct iphdr); ++i) {
        printf(" %02x", bytes[i]);
        if ((i + 1) % 10 == 0)
            printf("\n");
    }
    if (sizeof(struct iphdr) % 10 != 0)
        printf("\n");

    printf("Vr HL TOS  Len   ID Flg  off TTL Pro  cks      Src\tDst\tData\n");

    printf(" %1x  %1x  %02x %04x %04x  %1x %04x  %02x  %02x %04x  ",
           ip_header->version,
           ip_header->ihl,
           ip_header->tos,
           ntohs(ip_header->tot_len),
           ntohs(ip_header->id),
           (ntohs(ip_header->frag_off) & 0xE000) >> 13,
           ntohs(ip_header->frag_off) & 0x1FFF,
           ip_header->ttl,
           ip_header->protocol,
           ntohs(ip_header->check));

    struct in_addr src, dst;
    src.s_addr = ip_header->saddr;
    dst.s_addr = ip_header->daddr;

    printf("%s  ", inet_ntoa(src));
    printf("%s\n", inet_ntoa(dst));
}

void printIcmpHeaderDetails(struct icmphdr *icmp_header, struct iphdr *ip_header)
{
    int icmp_size = ntohs(ip_header->tot_len) - (ip_header->ihl * 4);

    if (icmp_header->type != ICMP_ECHOREPLY) {
        icmp_size -= sizeof(struct iphdr); // Soustraire l'en-tête IP encapsulé
    }
    printf("ICMP: type %d, code %d, size %d, id 0x%04x, seq 0x%04x\n",
           icmp_header->type,
           icmp_header->code,
           icmp_size, // taille brute avec header IP
           ntohs(icmp_header->un.echo.id),
           ntohs(icmp_header->un.echo.sequence));
}

void printEnd(struct print_infos data)
{
    if (data.icmp_type == ICMP_TIME_EXCEEDED)
        printf(" Time to live exceeded\n");
    else
    printf(" icmp_seq=%d ttl=%d time=%.1f ms\n",
        data.sequence_number,
        data.ttl,
        data.time
    );
}

void printStart(struct print_infos data)
{
    printf("%d bytes from %s:",
        data.bytes_recv,
        data.ip_str
    );
}

void print_result(struct print_infos data, cmd *command)
{
    (void)command;
    printStart(data);
    printEnd(data);

    if (command->verbose && data.icmp_type != ICMP_ECHOREPLY) {
        printIpHeaderDump(data.ip_header);
        printIcmpHeaderDetails(data.icmp_header, data.ip_header);
    }
}