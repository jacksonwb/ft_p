/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   client.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jbeall <jbeall@student.42.us.org>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/07/15 16:47:04 by jbeall            #+#    #+#             */
/*   Updated: 2019/07/18 21:20:48 by jbeall           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "client.h"
#include "ftp.h"

void	usage(void)
{
	printf("./client hostname port\n");
	exit(EXIT_SUCCESS);
}

int client_connect(char *addr_str, char *port_str)
{
	struct addrinfo hints;
	struct addrinfo *addr;
	int sfd;
	if ((sfd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
		err_exit("socket");
	ft_memset(&hints, 0, sizeof(hints));
	addr = NULL;
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	if(getaddrinfo(addr_str, port_str, &hints, &addr) == -1)
		err_exit("getaddrinfo");
	if(!addr)
		err_exit("Bad Address");
	if (connect(sfd, addr->ai_addr, addr->ai_addrlen) == -1)
		err_exit("Connect");
	return (sfd);
}

bool prompt(void)
{
	PRINT(PROMPT);
	return (true);
}

int connect_to_data_sock(int sfd)
{
	socklen_t addr_len;
	struct sockaddr addr;
	int dfd;

	ft_memset(&addr, 0, sizeof(addr));
	recv(sfd, &addr_len, sizeof(addr_len), 0);
	recv(sfd, &addr, addr_len, 0);
	if((dfd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
		err_exit("socket");
	if (connect(dfd, &addr, addr_len) == -1)
		err_exit("could not connect");
	return (dfd);
}

void read_data_from_request(int sfd, int cmd, char *msg, int outfd)
{
	int dfd;
	char buf[1000];
	size_t len;

	if (send_code(sfd, cmd, msg) == -1)
	{
		printf("Error: expected ACK\n");
		return;
	}
	dfd = connect_to_data_sock(sfd);
	while((len = recv(dfd, buf, sizeof(buf), 0)))
		write(outfd, buf, len);
	close(dfd);
}

void handle_ls(int sfd, char **av)
{
	(void)av;
	read_data_from_request(sfd, LIST, NULL, STDOUT_FILENO);
}

void handle_pwd(int sfd, char **av)
{
	(void)av;
	read_data_from_request(sfd, CWD, NULL, STDOUT_FILENO);
	write(1, "\n", 1);
}

void handle_cd(int sfd, char **av)
{
	if (!av ||  !av[0])
	{
		printf("cd : need argument\n");
		return;
	}
	read_data_from_request(sfd, CD, av[0], STDERR_FILENO);
}

void handle_get(int sfd, char **av)
{
	int newfd;

	if (ft_strchr(av[0], '/'))
	{
		printf("get : filename cannot contain '/' character\n");
		return;
	}
	if (send_code(sfd, IS_FILE, av[0]) == -1)
	{
		printf("Error: file does not exist or is invalid\n");
		return;
	}
	newfd = open(av[0], O_RDWR | O_CREAT | O_TRUNC);
	read_data_from_request(sfd, FILE, av[0], newfd);
	close (newfd);
}

void handle_put(int sfd, char **av)
{
	printf("put: %s\n", av[0]);

	(void)sfd;
}

void handle_quit(int sfd, char **av)
{
	(void)av;
	(void)sfd;
	exit(EXIT_SUCCESS);
}

void handle_cmd(int sfd, char *str)
{
	char **cmd;
	int i;
	static void(*jump[])(int, char **) = {
		&handle_ls,
		&handle_cd,
		&handle_get,
		&handle_put,
		&handle_pwd,
		&handle_quit
	};

	i = 0;
	cmd = ft_strsplit(str, ' ');
	while (i < NUM_CMDS)
	{
		if (!ft_strcmp(cmd[0], g_cmd_str[i]))
		{
			jump[i](sfd, cmd + 1);
			break;
		}
		i++;
	}
	if (i >= NUM_CMDS)
		printf("command not found : %s\n", cmd[0]);
	free_str_split(cmd);
}

int main(int ac, char **av)
{
	int sfd;
	char *buf;

	if (ac != 3)
		usage();
	sfd = client_connect(av[1], av[2]);
	while (prompt() && get_next_line(STDIN_FILENO, &buf))
	{
		handle_cmd(sfd, buf);
		free(buf);
	}
	return (0);
}
