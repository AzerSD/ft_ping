/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: asioud <asioud@42heilbronn.de>             +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/20 04:15:25 by asioud            #+#    #+#             */
/*   Updated: 2023/10/22 03:01:51 by asioud           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_ping.h"


	 
#define PACKET_SIZE 20 /* ping of death attack */
#define ICMP_HEADER_SIZE 8

int ping_loop = 1;

unsigned short checksum(void *b, int len)
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

void intHandler(int dummy)
{
    (void) dummy;
	ping_loop=0;
}

void handlePingResponse(char *responseBuffer) {
    struct ip *ipHeader = (struct ip *)responseBuffer;
    int ipHeaderLength = ipHeader->ip_hl * 4;
    struct icmp *icmpHeader = (struct icmp *)(responseBuffer + ipHeaderLength);

    if (icmpHeader->icmp_type == ICMP_ECHOREPLY) {
        printf("Received Ping Reply from %s\n", inet_ntoa(ipHeader->ip_src));
    } else {
        printf("Received ICMP packet of type %d\n", icmpHeader->icmp_type);
    }
}

void echo_request(int sockfd, char *domainName)
{
    struct sockaddr_in targetAddress;
    // struct sockaddr targetAddress = resolve_fqdn(domainName);
    targetAddress.sin_family = AF_INET;
    targetAddress.sin_addr.s_addr = inet_addr("8.8.8.8");

    char packet[PACKET_SIZE];
    memset(packet, 0, PACKET_SIZE);
	
	struct icmp *icmpHeader = (struct icmp *)packet;
    icmpHeader->icmp_type = ICMP_ECHO;
    icmpHeader->icmp_code = 0;
    icmpHeader->icmp_id = getpid();
    icmpHeader->icmp_seq = 1;
    icmpHeader->icmp_cksum = 0;
    icmpHeader->icmp_cksum = checksum((unsigned short *)icmpHeader, ICMP_HEADER_SIZE + PACKET_SIZE);

    int sent_bytes = sendto(sockfd, packet, PACKET_SIZE, 0, (struct sockaddr *)&targetAddress, sizeof(targetAddress));
    if (sent_bytes == -1) {
        perror("Failed to send ICMP packet");
        close(sockfd);
        exit(EXIT_FAILURE);
    }
    printf("Echo Request sent to %s\n", "8.8.8.8");
}

void echo_reply(int sockfd, char *domainName)
{
	char responseBuffer[PACKET_SIZE];
    memset(responseBuffer, 0, PACKET_SIZE);
    int received_bytes = recv(sockfd, responseBuffer, PACKET_SIZE, 0);
    if (received_bytes == -1) {
        perror("Failed to receive ICMP response");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    struct ip *ipHeader = (struct ip *)responseBuffer;
    if (ipHeader->ip_p == IPPROTO_ICMP) {
        handlePingResponse(responseBuffer);
    } else {
        printf("Received non-ICMP packet\n");
    }
}


struct addrinfo resolve_fqdn(char *domainName)
{
	struct addrinfo *server;
	struct addrinfo hints;

	memset(&hints, 0, sizeof hints); /* libft */
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	
	int status = getaddrinfo(domainName, NULL, &hints, &server);
	if (status != 0) {
		fprintf(stderr, "getaddrinfo error: %s\n", gai_strerror(status));
        exit(1);
	}
	return *server;
}

int ft_ping(char *domainName)
{
	int		sockfd;

    (void) domainName;
	sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
	if (sockfd < 0)
	{
		printf("\nSocket file descriptor not received!\n");
		return 0;
	}
	else
	{
		printf("\nSocket file descriptor %d received\n", sockfd);
	}
	signal(SIGINT, intHandler);
    while(ping_loop)
    {
        echo_request(sockfd, domainName);
        echo_reply(sockfd, domainName);
    }
	return 0;
}

int main(int argc, char **argv)
{
    if (getuid() != 0)
    {
        fprintf(stderr, "%s: raw socket operations require superuser privileges!\n", argv[0]);
        exit(EXIT_FAILURE);
    }

	if (argc == 2)
    {
		ft_ping(argv[1]);
    }
	else
		printf(RED "wrong arguments" RESET);
	
	return 0;
}