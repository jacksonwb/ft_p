/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   client_util.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jbeall <jbeall@student.42.us.org>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/07/20 17:54:22 by jbeall            #+#    #+#             */
/*   Updated: 2019/07/20 18:26:18 by jbeall           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "client.h"
#include "ftp.h"

void	usage(void)
{
	printf("./client hostname port\n");
	exit(EXIT_SUCCESS);
}

bool	prompt(void)
{
	printf(PURPLE);
	printf(PROMPT);
	printf(RESET);
	printf(" > ");
	fflush(stdout);
	return (true);
}

void	write_progress(size_t sent, size_t total)
{
	float	ratio;
	float	percent;
	char	buf[51];
	int		i;

	ratio = (float)sent / (float)total;
	percent = ratio * 100;
	ft_memset(buf, ' ', sizeof(buf));
	buf[sizeof(buf) - 1] = '\0';
	i = 0;
	while (i < ratio * sizeof(buf) - 2)
	{
		buf[i] = '=';
		i++;
	}
	printf("\rprogress [%s] %.0f%% ", buf, percent);
	fflush(stdout);
}

void	clear_progress(void)
{
	printf("\n");
}

int		client_connect(char *addr_str, char *port_str)
{
	struct addrinfo	hints;
	struct addrinfo	*addr;
	int				sfd;

	if ((sfd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
		err_exit("socket");
	ft_memset(&hints, 0, sizeof(hints));
	addr = NULL;
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	if (getaddrinfo(addr_str, port_str, &hints, &addr) == -1)
		err_exit("getaddrinfo");
	if (!addr)
		err_exit("Bad Address");
	if (connect(sfd, addr->ai_addr, addr->ai_addrlen) == -1)
		err_exit("Connect");
	freeaddrinfo(addr);
	return (sfd);
}
