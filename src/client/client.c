/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   client.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jbeall <jbeall@student.42.us.org>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/07/15 16:47:04 by jbeall            #+#    #+#             */
/*   Updated: 2019/07/19 21:59:16 by jbeall           ###   ########.fr       */
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
	freeaddrinfo(addr);
	return (sfd);
}

bool prompt(void)
{
	printf(PURPLE PROMPT RESET " > ");
	fflush(stdout);
	return (true);
}

int connect_to_data_sock(int sfd)
{
	socklen_t addr_len;
	struct sockaddr_in addr;
	struct sockaddr_in peer;
	socklen_t peer_len;
	int dfd;

	addr_len = sizeof(addr);
	ft_memset(&addr, 0, sizeof(addr));
	addr.sin_family = PF_INET;
	recv(sfd, &(addr.sin_addr.s_addr), sizeof(addr.sin_addr.s_addr), 0);
	recv(sfd, &(addr.sin_port), sizeof(addr.sin_port), 0);
	if (getpeername(sfd, (struct sockaddr*)(&peer), &peer_len) == -1)
		return (-1);
	addr.sin_addr.s_addr = peer.sin_addr.s_addr;
	//getpeeraddress
	printf("family: %u, port: %u, address: %u\n", addr.sin_family, addr.sin_port, addr.sin_addr.s_addr);
	if((dfd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
		err_exit("socket");
	if (connect(dfd, (struct sockaddr*)(&addr), addr_len) == -1)
		err_exit("could not connect");
	return (dfd);
}

void read_data_from_request(int sfd, int cmd, char *msg, int outfd)
{
	int dfd;
	uint8_t buf[1000];
	size_t len;

	if (send_code(sfd, cmd, msg) == -1)
	{
		printf("Error: expected ACK\n");
		return;
	}
	dfd = connect_to_data_sock(sfd);
	while((len = recv(dfd, buf, sizeof(buf), 0)))
	{
		printf("Received %zu bytes\n", len);
		write(outfd, buf, len);

	}
	close(dfd);
}

void send_data_for_request(int sfd, int cmd, char *msg, int infd)
{
	int dfd;
	uint8_t buf[1000];
	size_t len;

	if (send_code(sfd, cmd, msg) == -1)
	{
		printf("Error: expected ACK\n");
		return;
	}
	dfd = connect_to_data_sock(sfd);
	while((len = read(infd, buf, sizeof(buf))))
	{
		printf("Sent %zu bytes\n", len);
		send(dfd, buf, len, 0);
	}
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
	char buf[MAX_TN_LEN];

	if (!av || !av[0] || !*av[0])
		return;
	if (ft_strchr(av[0], '/'))
	{
		printf("get : filename cannot contain '/' character\n");
		return;
	}
	if (send_code(sfd, GET, av[0]) == -1)
	{
		printf("Error: expected ACK\n");
		return;
	}

	ft_memset(buf, 0, sizeof(buf));
	if (recv(sfd, buf, ft_strlen(g_com_str[ACK]), 0) == 0)
		die("Error: socket was closed");
	if (ft_strncmp(buf, g_com_str[ACK], ft_strlen(g_com_str[ACK])))
	{
		printf("Error: file does not exist or is invalid\n");
		return;
	}

	newfd = open(av[0], O_WRONLY | O_CREAT | O_TRUNC, 0666);
	read_data_from_request(sfd, SFILE, av[0], newfd);
	close (newfd);
}

void handle_put(int sfd, char **av)
{
	int filefd;

	if (!av || !av[0] || !*av[0])
		return;
	if (ft_strchr(av[0], '/'))
	{
		printf("put : filename cannot contain '/' character\n");
		return;
	}
	if ((filefd = open(av[0], O_RDONLY)) == -1)
	{
		perror("put");
		return;
	}
	if (send_code(sfd, PUT, av[0]) == -1)
	{
		printf("Error: expected ACK\n");
		return;
	}
	send_data_for_request(sfd, TFILE, av[0], filefd);
	close(filefd);
}

void handle_quit(int sfd, char **av)
{
	(void)av;
	(void)sfd;
	exit(EXIT_SUCCESS);
}

void handle_lls(int sfd, char **av)
{
	int pid;

	(void)sfd;
	pid = fork();
	if (!pid)
	{
		if (execvp("ls", (char*[]){"ls", "-l", av[0], NULL}) == -1)
			err_exit("lls");
	}
	wait4(pid, NULL, 0, NULL);
}

void handle_lcd(int sfd, char **av)
{
	(void)sfd;
	char buf[MAXPATHLEN];

	if (chdir(av[0]) == -1)
		perror("lcd");
	setenv("OLDPWD", getenv("PWD"), 1);
	setenv("PWD", getcwd(buf, MAXPATHLEN), 1);
}

void handle_lpwd(int sfd, char **av)
{
	(void)sfd;
	(void)av;
	char buf[MAXPATHLEN];

	printf("%s\n", getcwd(buf, MAXPATHLEN));
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
		&handle_quit,
		&handle_lls,
		&handle_lcd,
		&handle_lpwd
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
