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