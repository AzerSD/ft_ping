#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/ip_icmp.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/time.h>
#include <errno.h>

static unsigned short calculate_checksum(void *b, int len)
{    unsigned short *buf = b;
    unsigned int sum=0;
    unsigned short result;
 
    for ( sum = 0; len > 1; len -= 2 )
        sum += *buf++;
    if ( len == 1 )
        sum += *(unsigned char*)buf;
    sum = (sum >> 16) + (sum & 0xFFFF);
    sum += (sum >> 16);
    result = ~sum;
    return result;
}

int main(int argc, char *argv[], char *envp[])
{
    int sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
    if (sockfd < 0)
    {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }


    // Resolve FQDN to IP
    struct addrinfo hints, *res;
    char ip_str[INET_ADDRSTRLEN];
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_RAW;

    if (getaddrinfo(argv[1], NULL, &hints, &res) != 0) {
        perror("getaddrinfo failed");
        exit(EXIT_FAILURE);
    }

    struct sockaddr_in *addr = (struct sockaddr_in *)res->ai_addr;
    inet_ntop(AF_INET, &(addr->sin_addr), ip_str, sizeof(ip_str));
    printf("Resolved IP for %s: %s\n", argv[1], ip_str);

    struct icmphdr icmp_hdr;
    icmp_hdr.type = ICMP_ECHO;
    icmp_hdr.code = 0;
    icmp_hdr.checksum = 0;
    icmp_hdr.un.echo.id = getpid();
    icmp_hdr.un.echo.sequence = 1;

    icmp_hdr.checksum = calculate_checksum(&icmp_hdr, sizeof(icmp_hdr));


    struct sockaddr_in dest_addr;
    dest_addr.sin_family = AF_INET;
    dest_addr.sin_addr = addr->sin_addr;
    // inet_pton(AF_INET, "8.8.8.8", &dest_addr.sin_addr);

    if (sendto(sockfd, &icmp_hdr, sizeof(icmp_hdr), 0, (struct sockaddr *)&dest_addr, sizeof(dest_addr)) < 0) {
        perror("Sendto failed");
        exit(EXIT_FAILURE);
    }


    // receive response
    char buffer[1024];
    struct sockaddr_in reply_addr;
    socklen_t addr_len = sizeof(reply_addr);

    if (recvfrom(sockfd, buffer, sizeof(buffer), 0, (struct sockaddr *)&reply_addr, &addr_len) < 0) {
        perror("Recvfrom failed");
        exit(EXIT_FAILURE);
    }
 
    struct iphdr *ip_hdr = (struct iphdr *)buffer;
    struct icmphdr *icmp_reply = (struct icmphdr *)(buffer + ip_hdr->ihl * 4);
    printf("%d bytes from %s icmp_seq=%d ttl=%d\n", \
        ntohs(ip_hdr->tot_len), \
        inet_ntoa(reply_addr.sin_addr), \
        icmp_reply->un.echo.sequence, \
        ip_hdr->ttl
    );

    return 0;
}