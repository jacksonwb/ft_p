/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server_handler2.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jbeall <jbeall@student.42.us.org>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/07/20 16:43:01 by jbeall            #+#    #+#             */
/*   Updated: 2019/07/30 10:35:47 by jbeall           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "server.h"
#include "ftp.h"

void	get_helper(int afd, char *path, char *cwd)
{
	int dfd;
	int status;

	dfd = establish_data_sock(afd);
	status = send_file_to_client(cwd, path, dfd);
	close(dfd);
	if (!status)
		server_log(GREEN "Successfully sent file" RESET);
	else
		server_log(RED "File transfer unsuccessful!" RESET);
}

void	handle_get(int afd, char **av, char *cwd)
{
	char	buf[MAX_TN_LEN];
	char	fbuf[SERVER_BUFF_SIZE];
	char	**msg;
	size_t	file_size;

	if (!is_gettable_file(cwd, av[0], fbuf))
	{
		send_bad(afd);
		return ;
	}
	send_ack(afd);
	file_size = get_file_size_from_filename(av[0], cwd);
	send(afd, &file_size, sizeof(file_size), 0);
	recv(afd, buf, MAX_TN_LEN, 0);
	msg = ft_strsplit(buf, ':');
	if (msg && *msg && ft_strcmp(msg[0], g_com_str[SFILE]))
	{
		server_log("Invalid get code\n");
		send_bad(afd);
		return ;
	}
	send_ack(afd);
	get_helper(afd, av[0], cwd);
	free_str_split(msg);
}

void	handle_put(int afd, char **av, char *cwd)
{
	int		dfd;
	char	buf[MAX_TN_LEN];
	char	**msg;

	recv(afd, buf, MAX_TN_LEN, 0);
	msg = ft_strsplit(buf, ':');
	if (msg && *msg && ft_strcmp(msg[0], g_com_str[TFILE]))
	{
		server_log("Invalid get code\n");
		send_bad(afd);
		return ;
	}
	send_ack(afd);
	dfd = establish_data_sock(afd);
	rec_file_from_client(cwd, av[0], dfd);
	server_log(GREEN "Successfully received file" RESET);
	free_str_split(msg);
	close(dfd);
}

int		send_file_to_client(char *cwd, char *path, int dfd)
{
	uint8_t		buf[FILE_BUFF_SIZE];
	char		namebuf[SERVER_BUFF_SIZE];
	t_rtsize	size;
	ssize_t		total;
	int			ffd;

	ft_strcpy(namebuf, g_server_root);
	ft_strlcat(namebuf, cwd, SERVER_BUFF_SIZE);
	if (ft_strcmp(cwd, "/"))
		ft_strlcat(namebuf, "/", SERVER_BUFF_SIZE);
	ft_strlcat(namebuf, path, SERVER_BUFF_SIZE);
	total = 0;
	if ((ffd = open(namebuf, O_RDONLY)) == -1)
		return (-1);
	while ((size.rx = read(ffd, buf, FILE_BUFF_SIZE)) > 0)
	{
		size.tx = send(dfd, buf, size.rx, 0);
		total += size.tx;
	}
	printf("Sent %zu bytes in file: %s\n", total, path);
	close(ffd);
	return (size.tx < 0 ? -1 : 0);
}

void	rec_file_from_client(char *cwd, char *path, int dfd)
{
	uint8_t	buf[FILE_BUFF_SIZE];
	char	namebuf[SERVER_BUFF_SIZE];
	size_t	size;
	size_t	total;
	int		ffd;

	ft_strcpy(namebuf, g_server_root);
	ft_strlcat(namebuf, cwd, SERVER_BUFF_SIZE);
	if (ft_strcmp(cwd, "/"))
		ft_strlcat(namebuf, "/", SERVER_BUFF_SIZE);
	ft_strlcat(namebuf, path, SERVER_BUFF_SIZE);
	total = 0;
	if ((ffd = open(namebuf, O_WRONLY | O_CREAT | O_TRUNC, 0666)) == -1)
		return ;
	while ((size = recv(dfd, buf, FILE_BUFF_SIZE, 0)))
		total += write(ffd, buf, size);
	printf("Recieved %zu bytes in file: %s\n", total, path);
	close(ffd);
}
