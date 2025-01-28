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
#include <signal.h>
#include <stdbool.h>

#include "libft.h"

#define ICMP_PAYLOAD_SIZE 56

// TODO
// - [x] Fix packet size why 28 bytes?
// - [ ] Fix packet loss calculations
// - [ ] Fix Running multiple pings at the same time
// - [ ] Fix ping randomly stops receiving!!
// - [x] Fix "56(84) bytes of data" is hardcoded

int interval = 1;
int sig_int = false;

static unsigned short calculate_checksum(void *b, int len)
{   
    unsigned short *buf = b;
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

typedef struct ping {
    char *ping_hostname;
    size_t ping_num_xmit;
    size_t ping_num_recv;
    size_t ping_num_rept;
} ping_t;

ping_t ping = {
    .ping_hostname = NULL,
    .ping_num_xmit = 0,
    .ping_num_recv = 0,
    .ping_num_rept = 0
};

static int ping_finish(ping_t ping)
{
    printf("\n--- %s ping statistics ---\n", ping.ping_hostname);
    printf("%ld packets transmitted, %ld received, %ld%% packet loss\n", \
        ping.ping_num_xmit, \
        ping.ping_num_recv, \
        (ping.ping_num_xmit - ping.ping_num_recv) / ping.ping_num_xmit * 100
    );
    return 0;
}

static void handle_sigint(int sig)
{
    sig_int = true;
    ping_finish(ping);
    exit(EXIT_SUCCESS);
}

double calculate_rtt(struct timeval start, struct timeval end) {
    return (end.tv_sec - start.tv_sec) * 1000.0 + (end.tv_usec - start.tv_usec) / 1000.0;
}

typedef struct icmp_payload {
    struct timeval timestamp;
    char data[ICMP_PAYLOAD_SIZE - sizeof(struct timeval)];
} icmp_payload_t;

int main(int argc, char *argv[])
{
    if (getuid() != 0)
    {
        fprintf(stderr, "ft_ping: You need to be root to run this program\n");
        exit(EXIT_FAILURE);
    }

    if (argc != 2)
    {
        fprintf(stderr, "Usage: %s <hostname>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    int sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
    if (sockfd < 0)
    {
        printf("ping: error: %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }

    ping.ping_hostname = argv[1];

    signal(SIGINT, handle_sigint);

    // Resolve FQDN to IP
    struct addrinfo hints, *res;
    char ip_str[INET_ADDRSTRLEN];
    ft_memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_RAW;

    if (getaddrinfo(argv[1], NULL, &hints, &res) != 0) {
        printf("ping: error: %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }

    struct sockaddr_in *addr = (struct sockaddr_in *)res->ai_addr;
    inet_ntop(AF_INET, &(addr->sin_addr), ip_str, sizeof(ip_str));
    int sequence_nb = 1;
    printf("PING %s (%s) %d(%d) bytes of data.\n", argv[1], ip_str, ICMP_PAYLOAD_SIZE, ICMP_PAYLOAD_SIZE + sizeof(struct icmphdr) + sizeof(struct iphdr));
    
    while (1) {
        struct timeval start_time, end_time;
        gettimeofday(&start_time, NULL);

        // Create a buffer for the full packet
        

        struct icmphdr icmp_hdr;
        icmp_hdr.type = ICMP_ECHO;
        icmp_hdr.code = 0;
        icmp_hdr.checksum = 0;
        icmp_hdr.un.echo.id = getpid();
        icmp_hdr.un.echo.sequence = sequence_nb++;

        icmp_payload_t payload;
        gettimeofday(&payload.timestamp, NULL);
        memcpy(payload.data, "abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxxx", sizeof(payload.data));
    
        size_t packet_size = sizeof(struct icmphdr) + sizeof(payload);
        char packet[packet_size];

        memcpy(packet, &icmp_hdr, sizeof(icmp_hdr));
        memcpy(packet + sizeof(icmp_hdr), &payload, sizeof(payload));

        ((struct icmphdr *)packet)->checksum = calculate_checksum(packet, packet_size);

        // Send ICMP packet
        struct sockaddr_in dest_addr;
        dest_addr.sin_family = AF_INET;
        dest_addr.sin_addr = addr->sin_addr;

        if (sendto(sockfd, packet, packet_size, 0, (struct sockaddr *)&dest_addr, sizeof(dest_addr)) < 0) {
            printf("ping: error: %s\n", strerror(errno));
            exit(EXIT_FAILURE);
        }
        ping.ping_num_xmit++;


        // receive response
        char buffer[1024];
        struct sockaddr_in reply_addr;
        socklen_t addr_len = sizeof(reply_addr);

        struct msghdr msg;
        struct iovec iov[1];
        iov[0].iov_base = buffer;
        iov[0].iov_len = sizeof(buffer);
        ft_memset(&msg, 0, sizeof(msg));
        msg.msg_name = &reply_addr;
        msg.msg_namelen = addr_len;
        msg.msg_iov = iov;
        msg.msg_iovlen = 1;

        if (recvmsg(sockfd, &msg, 0) < 0) {
            printf("ping: error: %s\n", strerror(errno));
            exit(EXIT_FAILURE);
        }

        gettimeofday(&end_time, NULL);
        ping.ping_num_recv++;

        double rtt = calculate_rtt(start_time, end_time);

        // Parse IP and ICMP headers
        struct iphdr *ip_hdr = (struct iphdr *)buffer;
        struct icmphdr *icmp_reply = (struct icmphdr *)(buffer + ip_hdr->ihl * 4);

        printf("%d bytes from %s icmp_seq=%d ttl=%dms time=%.1f ms\n", \
            ntohs(ip_hdr->tot_len) - ip_hdr->ihl * 4,
            inet_ntop(AF_INET, &reply_addr.sin_addr, ip_str, sizeof(ip_str)), \
            icmp_reply->un.echo.sequence, \
            ip_hdr->ttl, \
            rtt
        );

        sleep(interval);
    }

    return 0;
}


// 0000   00 00 00 01 00 06 d4 3f cb 9e 70 aa 08 05 08 00   .......?..p.....
// 0010   45 e0 00 54 00 00 00 00 74 01 74 83 08 08 08 08   E..T....t.t.....
// 0020   c0 a8 00 8e 00 00 7e f4 00 07 00 01 27 af 98 67   ......~.....'..g
// 0030   00 00 00 00 01 1a 01 00 00 00 00 00 10 11 12 13   ................
// 0040   14 15 16 17 18 19 1a 1b 1c 1d 1e 1f 20 21 22 23   ............ !"#
// 0050   24 25 26 27 28 29 2a 2b 2c 2d 2e 2f 30 31 32 33   $%&'()*+,-./0123
// 0060   34 35 36 37                                       4567

// 50 bytes