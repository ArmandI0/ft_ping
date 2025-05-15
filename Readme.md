## Note LIBC debian

-> le sujet impose Debian

La fonction getaddrinfo, lorsqu’elle est utilisée avec un nom de domaine (ex. example.com), peut provoquer une fuite mémoire dans la libc sur certaines versions de Debian. Cette fuite se produit dans les couches internes du résolveur DNS (NSS) et n'est pas présente sur Ubuntu, ce qui indique une différence de comportement entre les versions de glibc. Ce n’est pas une fuite liée à l’application, mais à l’implémentation système.


## Usage

```
ft_ping [-v] [-?] [--ttl] <destination>
```

## options
-? -> equals to --help
-v -> --verbose 
--ttl -> set the time to leave of packet

