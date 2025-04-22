## Leak de la lib

## options
-? -> equals to --help
-v -> --verbose 

### bonus

-i wait
    Wait wait seconds between sending each packet. The default is to wait for one second between each packet. This option is incompatible with the -f option.
-c -> --count 
ttl -> 
## Case error
invalid flag -> ping: invalid argument: 'fsdf'


## Creation du packet
```
    https://en.wikipedia.org/wiki/Internet_Control_Message_Protocol
```

### Type = 8bits
```
    Type de message ICMP
```
### Code (8bits)
    Code associe au type voir tableau protocol ICMP
    Pour ping c'est 0.


    struct iphdr 
    struct icmphdr *icmp = (struct icmphdr *)packet;
    icmp->type = ICMP_ECHO;
    icmp->code = 0;
    icmp->un.echo.id = htons(1234);     // Identifiant arbitraire
    icmp->un.echo.sequence = htons(1);  // Numéro de séquence
    icmp->checksum = 0;

### Checksum

Paquet brut de 64 bits :
```
    [ 10101010 11001100 11110000 00001111 01010101 10101010 11001100 11110000 ]
```

Separation en bloc de 16 bits :
```
    Bloc 1 : 10101010 11001100 (0xAACC)
    Bloc 2 : 11110000 00001111 (0xF00F)
    Bloc 3 : 01010101 10101010 (0x55AA)
    Bloc 4 : 11001100 11110000 (0xCCF0)
```

Addition (dans un bloc de 32bits pour ne pas overflow) :
```
    0xAACC + 0xF00F = 0x9ADC5 (17 bits)
    0x9ADC5 en uint_32 -> 0000 0000 0000 1001 1010 1101 1100 0101
```

```
➜  ft_ping git:(main) ✗ ping google.com
PING google.com (142.250.179.110) 56(84) bytes of data.
64 bytes from par21s20-in-f14.1e100.net (142.250.179.110): icmp_seq=1 ttl=117 time=22.9 ms
64 bytes from par21s20-in-f14.1e100.net (142.250.179.110): icmp_seq=2 ttl=117 time=23.2 ms
64 bytes from par21s20-in-f14.1e100.net (142.250.179.110): icmp_seq=3 ttl=117 time=22.9 ms
^C
--- google.com ping statistics ---
3 packets transmitted, 3 received, 0% packet loss, time 2028ms
rtt min/avg/max/mdev = 22.863/22.980/23.202/0.157 ms
```