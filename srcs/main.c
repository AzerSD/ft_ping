/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: asioud <asioud@42heilbronn.de>             +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/20 04:15:25 by asioud            #+#    #+#             */
/*   Updated: 2023/10/20 16:04:06 by asioud           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_ping.h"

int ping_loop = 1;

void intHandler(int dummy)
{
	ping_loop=0;
}

int send_ping(int sockfd, char *domain)
{
	
	return 0;
}

int ft_ping(char *domain)
{
	char *sockfd;
	char *domain;

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
	send_ping(sockfd, domain);
	
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