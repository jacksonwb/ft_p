/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jbeall <jbeall@student.42.us.org>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/07/15 16:47:57 by jbeall            #+#    #+#             */
/*   Updated: 2019/07/17 14:14:34 by jbeall           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "server.h"
#include "ftp.h"

void	usage(void)
{
	printf("./server port\n");
	exit(EXIT_SUCCESS);
}

int init_server(char *port_str)
{
	struct sockaddr_in addr;
	int sfd;

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

void server_log(char *msg)
{
	time_t now;
	char *t_str;
	char *ptr;

	now = time(NULL);
	t_str = ctime(&now);
	if ((ptr =  ft_strchr(t_str, '\n')))
		*ptr = '\0';
	printf("[%s] %s\n", t_str, msg);
}

void log_receive(int afd, char *msg)
{
	char buf[MAX_LOG_RX_MSG];
	struct sockaddr peer;
	socklen_t peer_len;
	char host[1024];
	char service[20];

	peer_len = sizeof(peer);
	if(getpeername(afd, &peer, &peer_len) == -1)
		err_exit("getpeername");
	getnameinfo(&peer, peer_len, host, sizeof(host), service, sizeof(service), 0);
	snprintf(buf, MAX_LOG_RX_MSG, "[RX] from [%s:%s] %s", host, service, msg);
	server_log(buf);
}

void *handle_connection(void *in)
{
	char buf[MAX_TN_LEN];
	char *end;
	t_thread_args args;

	ft_memcpy(&args, in, sizeof(t_thread_args));
	while (recv(args.afd, buf, MAX_TN_LEN, 0))
	{
		if ((end = ft_strchr(buf, '\n')))
			*end = '\0';
		log_receive(args.afd, buf);
		send_ack(args.afd);
	}
	close(args.afd);
	return (NULL);
}

void launch_thread(int afd)
{
	t_thread_args args;
	pthread_t new;

	args.afd = afd;
	ft_strlcat(args.env, "/", 1024);
	pthread_create(&new, NULL, &handle_connection, (void*)&args);
	pthread_detach(new);
}

int main(int ac, char **av)
{
	int sfd;
	int afd;
	struct sockaddr faddr;
	socklen_t fadd_len;


	if (ac != 2)
		usage();
	sfd = init_server(av[1]);
	while ((afd = accept(sfd, &faddr, &fadd_len)))
	{
		server_log("Connection created!");
		launch_thread(afd);
	}
	return (0);
}