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

typedef struct ping {
    char *ping_hostname;
    size_t ping_num_xmit;
    size_t ping_num_recv;
    size_t ping_num_rept;
    struct timeval start_time;
    struct timeval end_time;
} ping_t;


typedef struct icmp_payload {
    struct timeval timestamp;
    char data[ICMP_PAYLOAD_SIZE - sizeof(struct timeval)];
} icmp_payload_t;

int sig_int = false;

ping_t ping = {
    .ping_hostname = NULL,
    .ping_num_xmit = 0,
    .ping_num_recv = 0,
    .ping_num_rept = 0
};


static unsigned short calculate_checksum(void *b, int len);
static int ping_finish(ping_t *ping);
static void handle_sigint(int sig);
static double calculate_rtt(struct timeval start, struct timeval end);


int main(int argc, char *argv[])
{

    if (getuid() != 0)
    {
        fprintf(stderr, "ft_ping: You need to be root to run this program\n");
        exit(EXIT_FAILURE);
    }

    int verbose = 0;
    int show_help = 0;
    int count = 0;
    int ttl = -1;
    int numeric = 0;
    int interval = 1;
    int payload_size = ICMP_PAYLOAD_SIZE;

    ArgParser parser;
    init_arg_parser(&parser);

    add_option(&parser, "-v", "--verbose", ARGTYPE_FLAG, &verbose);
    add_option(&parser, "-?", "--help", ARGTYPE_FLAG, &show_help);
    add_option(&parser, "-h", "--help", ARGTYPE_FLAG, &show_help);
    add_option(&parser, "-c", "--count", ARGTYPE_INT, &count);
    add_option(&parser, "-i", "--interval", ARGTYPE_INT, &interval);
    add_option(&parser, "-s", "--size", ARGTYPE_INT, &payload_size);
    add_option(&parser, "-n", "--numeric", ARGTYPE_FLAG, &numeric);

    parse_arguments(&parser, argc, argv);

    if (show_help) {
        printf("Usage: sudo ./ft_ping [-v] [-?] <hostname>\n");
        printf("  -v, --verbose        Show extra error information (e.g., unreachable hosts)\n");
        printf("  -c, --count COUNT    Stop after sending <count> packets\n");
        printf("  -i, --interval SECS  Interval between packets\n");
        printf("  -s, --size SIZE      Payload size in bytes\n");
        printf("  -n, --numeric        Do not resolve hostnames\n");
        printf("  -?, --help           Show this help message\n");
        printf("  -h, --help           Show this help message\n");
        exit(EXIT_SUCCESS);
    }

    // Require 1 positional argument: hostname
    if (parser.positional_count < 1) {
        fprintf(stderr, "ft_ping: missing host operand.\n");
        fprintf(stderr, "Usage: sudo ./ft_ping [-h] [-?] <hostname>\n");
        exit(EXIT_FAILURE);
    }

    ping.ping_hostname = parser.positional_args[0];

    int sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
    if (sockfd < 0)
    {
        printf("ping: error: %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }

    signal(SIGINT, handle_sigint);

    // Resolve FQDN to IP
    struct addrinfo hints, *res;
    char ip_str[INET_ADDRSTRLEN];
    ft_memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_RAW;

    if (getaddrinfo(ping.ping_hostname ,NULL, &hints, &res) != 0) {
        printf("ping: unknown host\n");
        exit(EXIT_FAILURE);
    }

    struct sockaddr_in *addr = (struct sockaddr_in *)res->ai_addr;
    inet_ntop(AF_INET, &(addr->sin_addr), ip_str, sizeof(ip_str));
    int sequence_nb = 0;
    if (verbose) {
        printf("PING %s (%s): %d data bytes, id 0x%x = %d\n",
            ping.ping_hostname,
            ip_str,
            payload_size,
            getpid(),
            getpid());
    } else {
        printf("PING %s (%s): %d bytes of data\n",
            ping.ping_hostname,
            ip_str,
            payload_size);
    }

    gettimeofday(&ping.start_time, NULL); // Record the start time

    while (count == 0 || ping.ping_num_xmit < (size_t)count) {
        struct timeval start_time, end_time;
        gettimeofday(&start_time, NULL);

        struct icmphdr icmp_hdr;
        icmp_hdr.type = ICMP_ECHO;
        icmp_hdr.code = 0;
        icmp_hdr.checksum = 0;
        icmp_hdr.un.echo.id = getpid();
        icmp_hdr.un.echo.sequence = htons(sequence_nb++);
        
        size_t adjusted_payload_size = payload_size;
        if (payload_size < sizeof(struct timeval)) {
            adjusted_payload_size = sizeof(struct timeval);
        }

        size_t packet_size = sizeof(struct icmphdr) + payload_size;
        char *packet = malloc(packet_size);
        if (!packet) {
            perror("malloc");
            exit(EXIT_FAILURE);
        }
        
        memset(packet + sizeof(struct icmphdr), 'x', payload_size);
        memcpy(packet, &icmp_hdr, sizeof(icmp_hdr));
        
        ((struct icmphdr *)packet)->checksum = calculate_checksum(packet, packet_size);

        // Send ICMP packet
        struct sockaddr_in dest_addr;
        dest_addr.sin_family = AF_INET;
        dest_addr.sin_addr = addr->sin_addr;

        struct timeval timeout;
        timeout.tv_sec = 2;
        timeout.tv_usec = 0;
        setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));

        if (sendto(sockfd, packet, packet_size, 0, (struct sockaddr *)&dest_addr, sizeof(dest_addr)) < 0) {
            perror("sendto");
            free(packet);
            continue;
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
            if (errno == EAGAIN || errno == EWOULDBLOCK) {
                continue;
            } else {
                printf("ping: error: %s\n", strerror(errno));
                exit(EXIT_FAILURE);
            }
        }

        gettimeofday(&end_time, NULL);
        ping.ping_num_recv++;

        struct iphdr *ip_hdr = (struct iphdr *)buffer;
        struct icmphdr *icmp_reply = (struct icmphdr *)(buffer + ip_hdr->ihl * 4);

        if (icmp_reply->type == ICMP_DEST_UNREACH) {
            struct iphdr *original_ip = (struct iphdr *)(buffer + ip_hdr->ihl * 4 + sizeof(struct icmphdr));
            struct icmphdr *original_icmp = (struct icmphdr *)((char *)original_ip + (original_ip->ihl * 4));

            printf("From %s icmp_seq=%d Destination Host Unreachable\n",
                inet_ntop(AF_INET, &reply_addr.sin_addr, ip_str, sizeof(ip_str)),
                ntohs(original_icmp->un.echo.sequence));
        } else {
            double rtt = calculate_rtt(start_time, end_time);
            printf("%d bytes from %s: icmp_seq=%d ttl=%d time=%.3f ms\n",
                ntohs(ip_hdr->tot_len) - (ip_hdr->ihl * 4),
                inet_ntop(AF_INET, &reply_addr.sin_addr, ip_str, sizeof(ip_str)),
                ntohs(icmp_reply->un.echo.sequence),
                ip_hdr->ttl,
                rtt);
        }

        free(packet);
        sleep(interval);
    }

    ping_finish(&ping);


    return 0;
}


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


static int ping_finish(ping_t *ping) {
    gettimeofday(&ping->end_time, NULL);
    // double total_time = (ping->end_time.tv_sec - ping->start_time.tv_sec) * 1000.0 + (ping->end_time.tv_usec - ping->start_time.tv_usec) / 1000.0;

    printf("\n--- %s ping statistics ---\n", ping->ping_hostname);
    printf("%ld packets transmitted, %ld received, %ld%% packet loss\n", \
        ping->ping_num_xmit, \
        ping->ping_num_recv, \
        ((ping->ping_num_xmit - ping->ping_num_recv) / ping->ping_num_xmit) * 100
    );
    return 0;
}


static void handle_sigint(int sig)
{
    sig_int = true;
    ping_finish(&ping);
    exit(EXIT_SUCCESS);
}


double calculate_rtt(struct timeval start, struct timeval end) {
    return (end.tv_sec - start.tv_sec) * 1000.0 + (end.tv_usec - start.tv_usec) / 1000.0;
}


