/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   client.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jbeall <jbeall@student.42.us.org>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/07/15 16:47:04 by jbeall            #+#    #+#             */
/*   Updated: 2019/07/29 20:16:31 by jbeall           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "client.h"
#include "ftp.h"

char *g_cmd_code[] = {
	"LIST",
	"CWD",
	"CD",
	"GET",
	"PUT"
};

char *g_cmd_str[] = {
	"ls",
	"cd",
	"get",
	"put",
	"pwd",
	"quit",
	"lls",
	"lcd",
	"lpwd"
};

static void	(*g_jump[])(int, char **) = {
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

int		connect_to_data_sock(int sfd)
{
	socklen_t			addr_len;
	struct sockaddr_in	addr;
	struct sockaddr_in	peer;
	socklen_t			peer_len;
	int					dfd;

	addr_len = sizeof(addr);
	peer_len = sizeof(peer);
	ft_memset(&addr, 0, sizeof(addr));
	addr.sin_family = PF_INET;
	recv(sfd, &(addr.sin_port), sizeof(addr.sin_port), 0);
	if (getpeername(sfd, (struct sockaddr*)(&peer), &peer_len) == -1)
		return (-1);
	addr.sin_addr.s_addr = peer.sin_addr.s_addr;
	if ((dfd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
		err_exit("socket");
	if (connect(dfd, (struct sockaddr*)(&addr), addr_len) == -1)
		err_exit("could not connect");
	return (dfd);
}

size_t	read_data_from_request(int sfd, t_client_cmd cmd, int outfd,
	size_t file_size)
{
	int		dfd;
	uint8_t	buf[1000];
	size_t	len;
	size_t	total;

	if (send_code(sfd, cmd.cmd, cmd.msg) == -1)
	{
		printf("Error: expected ACK\n");
		return (0);
	}
	dfd = connect_to_data_sock(sfd);
	total = 0;
	while ((len = recv(dfd, buf, sizeof(buf), 0)))
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

size_t	send_data_for_request(int sfd, t_client_cmd cmd, int infd,
	size_t file_size)
{
	int		dfd;
	uint8_t	buf[1000];
	size_t	len;
	size_t	total;

	if (send_code(sfd, cmd.cmd, cmd.msg) == -1)
	{
		printf("Error: expected ACK\n");
		return (0);
	}
	dfd = connect_to_data_sock(sfd);
	total = 0;
	while ((len = read(infd, buf, sizeof(buf))))
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

void	handle_cmd(int sfd, char *str)
{
	char		**cmd;
	int			i;

	i = 0;
	cmd = ft_strsplit(str, ' ');
	while (i < NUM_CMDS)
	{
		if (!ft_strcmp(cmd[0], g_cmd_str[i]))
		{
			g_jump[i](sfd, cmd + 1);
			break ;
		}
		i++;
	}
	if (i >= NUM_CMDS)
		printf("command not found : %s\n", cmd[0]);
	free_str_split(cmd);
}

int		main(int ac, char **av)
{
	int		sfd;
	char	*buf;

	if (ac != 3)
		usage();
	sfd = client_connect(av[1], av[2]);
	while (prompt() && get_next_line(STDIN_FILENO, &buf))
	{
		if (!buf || !*buf)
			continue;
		handle_cmd(sfd, buf);
		free(buf);
	}
	return (0);
}
