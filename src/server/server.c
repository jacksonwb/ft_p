/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jbeall <jbeall@student.42.us.org>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/07/15 16:47:57 by jbeall            #+#    #+#             */
/*   Updated: 2019/07/29 21:36:13 by jbeall           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "server.h"
#include "ftp.h"

char	*g_cmd_code[] = {
	"LIST",
	"CWD",
	"CD",
	"GET",
	"PUT"
};

int		is_gettable_file(char *cwd, char *path, char *fbuf)
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

void	handle_cmd(int afd, char *str, char *cwd)
{
	char		**cmd;
	int			i;
	static void	(*jump[])(int, char **, char *) = {
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

void	*handle_connection(void *in)
{
	char			buf[MAX_TN_LEN];
	char			*end;
	t_thread_args	args;
	char			cwd[SERVER_BUFF_SIZE];

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

void	launch_thread(int afd)
{
	t_thread_args	args;
	pthread_t		new;

	args.afd = afd;
	ft_strlcat(args.env, "/", 1024);
	pthread_create(&new, NULL, &handle_connection, (void*)&args);
	pthread_detach(new);
}

int		main(int ac, char **av)
{
	int				sfd;
	int				afd;
	struct sockaddr	faddr;
	socklen_t		fadd_len;

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
