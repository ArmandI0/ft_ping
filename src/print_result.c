#include "../ft_ping.h"

void print_ip_header_dump(struct iphdr *ip_header)
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

void print_icmp_details(struct icmphdr *icmp_header)
{
    printf("ICMP: type %d, code %d, size %d, id 0x%04x, seq 0x%04x\n",
           icmp_header->type,
           icmp_header->code,
           ntohs(icmp_header->un.echo.sequence + 4), // taille brute avec header IP
           ntohs(icmp_header->un.echo.id),
           ntohs(icmp_header->un.echo.sequence));
}

void print_end(struct print_infos data)
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

void print_start(struct print_infos data)
{
    printf("%d bytes from %s (%s):",
        data.bytes_recv,
        data.hostname,
        data.ip_str
    );
}

void print_result(struct print_infos data, cmd *command)
{
    (void)command;
    print_start(data);
    print_end(data);

    if (command->verbose && data.icmp_type != ICMP_ECHOREPLY) {
        print_ip_header_dump(data.ip_header);
        print_icmp_details(data.icmp_header);
    }
}