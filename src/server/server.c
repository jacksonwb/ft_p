/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jbeall <jbeall@student.42.us.org>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/07/15 16:47:57 by jbeall            #+#    #+#             */
/*   Updated: 2019/07/20 12:24:29 by jbeall           ###   ########.fr       */
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

int server_listen_eph(void)
{
	int sfd;
	if ((sfd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
	{
		server_log("Error: Create Socket\n");
		return (-1);
	}
	if (listen(sfd, 0) == -1)
	{
		server_log("Error: Listen");
		return (-1);
	}
	return (sfd);
}

int transmit_addr(int lfd, int afd)
{
	struct sockaddr_in addr;
	socklen_t addr_len;

	addr_len = sizeof(addr);
	ft_memset(&addr, 0, sizeof(addr));
	if (getsockname(lfd, (struct sockaddr*)(&addr), &addr_len) == -1)
		return (-1);
	printf("family: %u, port: %u, address: %u\n", addr.sin_family, addr.sin_port, addr.sin_addr.s_addr);
	send(afd, &(addr.sin_port), sizeof(addr.sin_port), 0);
	server_log("Sent sock_addr info");
	return (0);
}

void exec_to_socket(int dfd, char *cmd, char **av)
{
	int pid;

	pid = fork();
	if (!pid)
	{
		dup2(dfd, STDOUT_FILENO);
		dup2(dfd, STDERR_FILENO);
		if (execvp(cmd, av) == -1)
			err_exit("exec");
	}
	wait4(pid, NULL, 0, NULL);
}

int establish_data_sock(int afd)
{
	int lfd;
	int dfd;

	if ((lfd = server_listen_eph()) == -1)
		return (-1);
	transmit_addr(lfd, afd);
	dfd = accept(lfd, NULL, 0);
	close(lfd);
	return (dfd);
}

void handle_ls(int afd, char **av, char *cwd)
{
	int dfd;
	char buf[SERVER_BUFF_SIZE];

	(void)av;
	(void)cwd;
	dfd = establish_data_sock(afd);
	server_log("Opened data connection");
	if (!ft_strcmp(cwd, "/"))
		exec_to_socket(dfd, "ls", (char*[]){"ls", "-l", NULL});
	else
	{
		ft_strcpy(buf, g_server_root);
		ft_strlcat(buf, cwd, SERVER_BUFF_SIZE);
		printf("buf: %s\n", buf);
		exec_to_socket(dfd, "ls", (char*[]){"ls", "-l", buf, NULL});
	}
	close(dfd);
	server_log("Success, closed data connection");
}

void handle_pwd(int afd, char **av, char *cwd)
{
	int dfd;

	(void)av;
	dfd = establish_data_sock(afd);
	server_log("Opened data connection");
	write(dfd, cwd, ft_strlen(cwd));
	close(dfd);
	server_log("Success, closed data connection");
}

int is_valid_directory(char *cwd, char *path)
{
	struct stat file;
	char buf[2048];

	ft_strncpy(buf, g_server_root, sizeof(buf));
	ft_strlcat(buf, cwd, sizeof(buf));
	ft_strlcat(buf, "/", sizeof(buf));
	ft_strlcat(buf, path, sizeof(buf));
	if (lstat(buf, &file) == -1)
		return (0);
	if (S_ISDIR(file.st_mode))
		return (1);
	return (0);
}

void handle_cd(int afd, char **av, char *cwd)
{
	int dfd;

	dfd = establish_data_sock(afd);
	if (!av || !av[0])
		PRINTFD("Error: no argument\n", dfd);
	else if (ft_strchr(av[0], '/'))
		PRINTFD("Error: filename cannot contain a '/' character\n", dfd);
	else if (!ft_strcmp(av[0], ".."))
	{
		if (!ft_strcmp(cwd, "/"))
			PRINTFD("Error: already in root!\n", dfd);
		else
		{
			*ft_strrchr(cwd, '/') = 0;
			if (!*cwd)
				ft_strlcat(cwd, "/", SERVER_BUFF_SIZE);
		}
	}
	else if (is_valid_directory(cwd, av[0]))
	{
		if (ft_strcmp(cwd, "/"))
			ft_strlcat(cwd, "/", SERVER_BUFF_SIZE);
		ft_strlcat(cwd, av[0], SERVER_BUFF_SIZE);
	}
	else
		PRINTFD("Error: not a valid directory\n", dfd);
	close (dfd);
}

int is_gettable_file(char *cwd, char *path, char *fbuf)
{
	struct stat file;

	if (!path)
		return (0);
	ft_strcpy(fbuf, g_server_root);
	ft_strlcat(fbuf, cwd, SERVER_BUFF_SIZE);
	if (ft_strcmp(cwd, "/"))
		ft_strlcat(fbuf, "/", SERVER_BUFF_SIZE);
	ft_strlcat(fbuf, path, SERVER_BUFF_SIZE);
	if (lstat(fbuf, &file) == -1)
		return (0);
	if (S_ISREG(file.st_mode) && !access(fbuf, R_OK))
		return (1);
	printf("lstat succesful, check is bad\n");
	return (0);
}

void send_file_to_client(char *cwd, char *path, int dfd)
{
	uint8_t buf[FILE_BUFF_SIZE];
	char namebuf[SERVER_BUFF_SIZE];
	size_t size;
	size_t total;
	int ffd;

	ft_strcpy(namebuf, g_server_root);
	ft_strlcat(namebuf, cwd, SERVER_BUFF_SIZE);
	if (ft_strcmp(cwd, "/"))
		ft_strlcat(namebuf, "/", SERVER_BUFF_SIZE);
	ft_strlcat(namebuf, path, SERVER_BUFF_SIZE);
	total = 0;
	if((ffd = open(namebuf, O_RDONLY)) == -1)
		return;
	while ((size = read(ffd, buf, FILE_BUFF_SIZE)))
		total += send(dfd, buf, size, 0);
	printf("Sent %zu bytes in file: %s\n", total, path);
	close(ffd);
}

void rec_file_from_client(char *cwd, char *path, int dfd)
{
	uint8_t buf[FILE_BUFF_SIZE];
	char namebuf[SERVER_BUFF_SIZE];
	size_t size;
	size_t total;
	int ffd;

	ft_strcpy(namebuf, g_server_root);
	ft_strlcat(namebuf, cwd, SERVER_BUFF_SIZE);
	if (ft_strcmp(cwd, "/"))
		ft_strlcat(namebuf, "/", SERVER_BUFF_SIZE);
	ft_strlcat(namebuf, path, SERVER_BUFF_SIZE);
	total = 0;
	if ((ffd = open(namebuf, O_WRONLY | O_CREAT | O_TRUNC, 0666)) == -1)
		return;
	while ((size = recv(dfd, buf, FILE_BUFF_SIZE, 0)))
		total += write(ffd, buf, size);
	printf("Recieved %zu bytes in file: %s\n", total, path);
	close(ffd);
}

void handle_get(int afd, char **av, char *cwd)
{
	int dfd;
	char buf[MAX_TN_LEN];
	char fbuf[SERVER_BUFF_SIZE];
	char  **msg;

	if (!is_gettable_file(cwd, av[0], fbuf))
	{
		send_bad(afd);
		return;
	}
	send_ack(afd);
	recv(afd, buf, MAX_TN_LEN, 0);
	msg = ft_strsplit(buf, ':');
	if (msg && *msg && ft_strcmp(msg[0], g_com_str[SFILE]))
	{
		server_log("Invalid get code\n");
		send_bad(afd);
		return;
	}
	send_ack(afd);
	dfd = establish_data_sock(afd);
	send_file_to_client(cwd, av[0], dfd);
	close(dfd);
	server_log("Succesfully sent file");
	free_str_split(msg);
}

void handle_put(int afd, char **av, char  *cwd)
{
	int dfd;
	char buf[MAX_TN_LEN];
	char **msg;

	recv(afd, buf, MAX_TN_LEN, 0);
	msg = ft_strsplit(buf, ':');
	if (msg && *msg && ft_strcmp(msg[0], g_com_str[TFILE]))
	{
		server_log("Invalid get code\n");
		send_bad(afd);
		return;
	}
	send_ack(afd);
	dfd = establish_data_sock(afd);
	rec_file_from_client(cwd, av[0], dfd);
	server_log("Successfully received file");
	free_str_split(msg);
	close(dfd);
}

void handle_cmd(int afd, char *str, char *cwd)
{
	char **cmd;
	int i;
	static void(*jump[])(int, char **, char *) = {
		&handle_ls,
		&handle_pwd,
		&handle_cd,
		&handle_get,
		&handle_put
	};

	i = 0;
	cmd = ft_strsplit(str, ':');
	while (i < NUM_CMD_CODE)
	{
		if (!ft_strcmp(cmd[0], g_cmd_code[i]))
			jump[i](afd, cmd + 1, cwd);
		i++;
	}
	free_str_split(cmd);
}

void *handle_connection(void *in)
{
	char buf[MAX_TN_LEN];
	char *end;
	t_thread_args args;
	char cwd[SERVER_BUFF_SIZE];

	ft_memcpy(&args, in, sizeof(t_thread_args));
	ft_memset(cwd, '\0', sizeof(cwd));
	ft_strcpy(cwd, "/");
	while (recv(args.afd, buf, MAX_TN_LEN, 0))
	{
		if ((end = ft_strchr(buf, '\n')))
			*end = '\0';
		log_receive(args.afd, buf);
		send_ack(args.afd);
		handle_cmd(args.afd, buf, cwd);
	}
	close(args.afd);
	server_log("Connection closed!");
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
	g_server_root = ft_strdup(getenv("PWD"));
	while ((afd = accept(sfd, &faddr, &fadd_len)))
	{
		server_log("Connection opened!");
		launch_thread(afd);
	}
	return (0);
}