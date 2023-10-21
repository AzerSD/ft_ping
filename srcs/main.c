/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: asioud <asioud@42heilbronn.de>             +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/20 04:15:25 by asioud            #+#    #+#             */
/*   Updated: 2023/10/21 16:20:05 by asioud           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_ping.h"

     #include <sys/socket.h>
     #include <netinet/in.h>

	 
#define PACKET_SIZE 64
#define ICMP_HEADER_SIZE 8

int ping_loop = 1;

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

int send_ping(int sockfd, char *domainName)
{
    (void) domainName;
	// struct sockaddr targetAddress = resolve_fqdn(domainName);
	struct sockaddr_in targetAddress;
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
    // icmpHeader->icmp_cksum = calculateChecksum((unsigned short *)icmpHeader, ICMP_HEADER_SIZE + PACKET_SIZE);

	int sent_bytes = sendto(sockfd, packet, PACKET_SIZE, 0, (struct sockaddr *)&targetAddress, sizeof(targetAddress));
    if (sent_bytes == -1) {
        perror("Failed to send ICMP packet");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

	    printf("Ping sent to %s\n", "8.8.8.8");

    // Listen for ICMP Echo Replies
	char responseBuffer[PACKET_SIZE];
    memset(responseBuffer, 0, PACKET_SIZE);
    int received_bytes = recv(sockfd, responseBuffer, PACKET_SIZE, 0);
    if (received_bytes == -1) {
        perror("Failed to receive ICMP response");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    handlePingResponse(responseBuffer);

	return 0;
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
	send_ping(sockfd, domainName);
	
	return 0;
}

int main(int argc, char **argv)
{
	/* Check for root access */
    if (getuid() != 0)
    {
        fprintf(stderr, "%s: raw socket operations require superuser privileges!\n", argv[0]);
        exit(EXIT_FAILURE);
    }
	if (argc == 2)
		ft_ping(argv[1]);
	else
		printf(RED "wrong arguments" RESET);
	
	return 0;
}