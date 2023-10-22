#ifndef FT_PING_H
# define FT_PING_H

# include <time.h>
# include <stdio.h> // printf
# include <unistd.h>
# include <stdlib.h> // exit
# include <signal.h>
# include <string.h>
# include <sys/types.h>

# include <netinet/ip_icmp.h>
# include <sys/socket.h>
# include <netinet/in.h>
# include <arpa/inet.h>
# include <netinet/ip.h>
# include <netdb.h>

     
# include "ANSI-color-codes.h"


struct addrinfo resolve_fqdn(char *domainName);
int send_ping(int sockfd, char *domainName);
void intHandler(int dummy);
int ft_ping(char *domainName);
unsigned short checksum(void *b, int len);


#endif