/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server_util.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jbeall <jbeall@student.42.us.org>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/07/20 16:34:30 by jbeall            #+#    #+#             */
/*   Updated: 2019/07/20 17:16:49 by jbeall           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "server.h"
#include "ftp.h"

void	usage(void)
{
	printf("./server port\n");
	exit(EXIT_SUCCESS);
}

void	check_port(char *port_str)
{
	int i;

	i = 0;
	while (port_str[i])
	{
		if (!ft_isdigit(port_str[i]))
			die("Port must be a number");
		i++;
	}
}

int		init_server(char *port_str)
{
	struct sockaddr_in	addr;
	int					sfd;

	check_port(port_str);
	printf("initializing server...\n");
	if ((sfd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
		err_exit("Socket Error");
	ft_memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_port = htons((uint32_t)(ft_atoi(port_str)));
	addr.sin_addr.s_addr = INADDR_ANY;
	if (bind(sfd, (struct sockaddr*)&addr, sizeof(addr)) == -1)
		err_exit("Bind Error");
	if (listen(sfd, 0) == -1)
		err_exit("Listen Error");
	printf("server listening on port %s\n", port_str);
	return (sfd);
}

void	server_log(char *msg)
{
	time_t	now;
	char	*t_str;
	char	*ptr;

	now = time(NULL);
	t_str = ctime(&now);
	if ((ptr = ft_strchr(t_str, '\n')))
		*ptr = '\0';
	printf("[%s] %s\n", t_str, msg);
}

void	log_receive(int afd, char *msg)
{
	char			buf[MAX_LOG_RX_MSG];
	struct sockaddr	peer;
	socklen_t		peer_len;
	char			host[1024];
	char			service[20];

	peer_len = sizeof(peer);
	if (getpeername(afd, &peer, &peer_len) == -1)
		err_exit("getpeername");
	getnameinfo(&peer, peer_len, host, sizeof(host),
		service, sizeof(service), 0);
	snprintf(buf, MAX_LOG_RX_MSG, "[RX] from [%s:%s] %s", host, service, msg);
	server_log(buf);
}
