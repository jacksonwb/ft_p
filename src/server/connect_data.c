/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   connect_data.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jbeall <jbeall@student.42.us.org>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/07/20 16:37:18 by jbeall            #+#    #+#             */
/*   Updated: 2019/07/20 17:46:21 by jbeall           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "server.h"
#include "ftp.h"

int		server_listen_eph(void)
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

int		transmit_addr(int lfd, int afd)
{
	struct sockaddr_in	addr;
	socklen_t			addr_len;

	addr_len = sizeof(addr);
	ft_memset(&addr, 0, sizeof(addr));
	if (getsockname(lfd, (struct sockaddr*)(&addr), &addr_len) == -1)
		return (-1);
	send(afd, &(addr.sin_port), sizeof(addr.sin_port), 0);
	server_log("Sent sock_addr info");
	return (0);
}

int		establish_data_sock(int afd)
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

void	exec_to_socket(int dfd, char *cmd, char **av)
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

size_t	get_file_size_from_filename(char *path, char *cwd)
{
	char namebuf[SERVER_BUFF_SIZE];

	ft_strcpy(namebuf, g_server_root);
	ft_strlcat(namebuf, cwd, SERVER_BUFF_SIZE);
	if (ft_strcmp(cwd, "/"))
		ft_strlcat(namebuf, "/", SERVER_BUFF_SIZE);
	ft_strlcat(namebuf, path, SERVER_BUFF_SIZE);
	return (get_file_size(namebuf));
}
