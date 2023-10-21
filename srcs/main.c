/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: asioud <asioud@42heilbronn.de>             +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/20 04:15:25 by asioud            #+#    #+#             */
/*   Updated: 2023/10/21 02:02:07 by asioud           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_ping.h"

int ping_loop = 1;

void intHandler(int dummy)
{
	ping_loop=0;
}

int send_ping(int sockfd, char *domainName)
{

	resolve_fqdn(domainName);
	printf("asdfasdf\n");
	return 0;
}

struct addrinfo resolve_fqdn(char *domainName)
{
	struct addrinfo server;
	struct addrinfo hints;

	memset(&hints, 0, sizeof hints); /* libft */
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	
	int status = getaddrinfo(domainName, NULL, &hints, &server);
	if (status != 0) {
		fprintf(stderr, "getaddrinfo error: %s\n", gai_strerror(status));
        exit(1);
	}
	return server;
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
	if (argc == 2)
		ft_ping(argv[1]);
	else
		printf(RED "wrong arguments" RESET);
	
	return 0;
}