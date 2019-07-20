/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   client.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jbeall <jbeall@student.42.us.org>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/07/15 16:47:04 by jbeall            #+#    #+#             */
/*   Updated: 2019/07/20 14:38:12 by jbeall           ###   ########.fr       */
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
	peer_len = sizeof(peer);
	ft_memset(&addr, 0, sizeof(addr));
	addr.sin_family = PF_INET;
	recv(sfd, &(addr.sin_port), sizeof(addr.sin_port), 0);
	if (getpeername(sfd, (struct sockaddr*)(&peer), &peer_len) == -1)
		return (-1);
	addr.sin_addr.s_addr = peer.sin_addr.s_addr;
	if((dfd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
		err_exit("socket");
	if (connect(dfd, (struct sockaddr*)(&addr), addr_len) == -1)
		err_exit("could not connect");
	return (dfd);
}

size_t read_data_from_request(int sfd, int cmd, char *msg, int outfd, size_t file_size)
{
	int dfd;
	uint8_t buf[1000];
	size_t len;
	size_t total;

	if (send_code(sfd, cmd, msg) == -1)
	{
		printf("Error: expected ACK\n");
		return (0);
	}
	dfd = connect_to_data_sock(sfd);
	total = 0;
	while((len = recv(dfd, buf, sizeof(buf), 0)))
	{
		total += len;
		write(outfd, buf, len);
		if (file_size)
			write_progress(total, file_size);
	}
	if (file_size)
		clear_progress();
	close(dfd);
	return (total);
}

void write_progress(size_t sent, size_t total)
{
	float ratio;
	float percent;
	char buf[51];
	int i;

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

void clear_progress(void)
{
	printf("\n");
}

size_t send_data_for_request(int sfd, int cmd, char *msg, int infd, size_t file_size)
{
	int dfd;
	uint8_t buf[1000];
	size_t len;
	size_t total;

	if (send_code(sfd, cmd, msg) == -1)
	{
		printf("Error: expected ACK\n");
		return (0);
	}
	dfd = connect_to_data_sock(sfd);
	total = 0;
	while((len = read(infd, buf, sizeof(buf))))
	{
		total += send(dfd, buf, len, 0);
		if (file_size)
			write_progress(total, file_size);
	}
	if (file_size)
		clear_progress();
	close(dfd);
	return (total);
}

void handle_ls(int sfd, char **av)
{
	(void)av;
	read_data_from_request(sfd, LIST, NULL, STDOUT_FILENO, 0);
}

void handle_pwd(int sfd, char **av)
{
	(void)av;
	read_data_from_request(sfd, CWD, NULL, STDOUT_FILENO, 0);
	write(1, "\n", 1);
}

void handle_cd(int sfd, char **av)
{
	if (!av ||  !av[0])
	{
		printf("cd : need argument\n");
		return;
	}
	read_data_from_request(sfd, CD, av[0], STDERR_FILENO, 0);
}

void handle_get(int sfd, char **av)
{
	int newfd;
	char buf[MAX_TN_LEN];
	size_t total;
	size_t file_size;

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
	file_size = 0;
	if (recv(sfd, &file_size, sizeof(file_size), 0) == 0)
		die("Error: socket was closed");
	total = read_data_from_request(sfd, SFILE, av[0], newfd, file_size);
	printf("Received %zu bytes in file: %s\n", total, av[0]);
	close (newfd);
}

void handle_put(int sfd, char **av)
{
	int filefd;
	size_t total;
	size_t file_size;

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
	total = 0;
	file_size = get_file_size(av[0]);
	total = send_data_for_request(sfd, TFILE, av[0], filefd, file_size);
	printf("Sent %zu bytes in file: %s\n", total, av[0]);
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
