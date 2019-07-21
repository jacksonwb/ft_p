/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   client_handler1.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jbeall <jbeall@student.42.us.org>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/07/20 17:56:01 by jbeall            #+#    #+#             */
/*   Updated: 2019/07/20 18:34:46 by jbeall           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "client.h"
#include "ftp.h"

void	handle_ls(int sfd, char **av)
{
	(void)av;
	read_data_from_request(sfd, LIST, NULL, STDOUT_FILENO, 0);
}

void	handle_pwd(int sfd, char **av)
{
	(void)av;
	read_data_from_request(sfd, CWD, NULL, STDOUT_FILENO, 0);
	write(1, "\n", 1);
}

void	handle_cd(int sfd, char **av)
{
	if (!av || !av[0])
	{
		printf("cd : need argument\n");
		return ;
	}
	read_data_from_request(sfd, CD, av[0], STDERR_FILENO, 0);
}

void	handle_get(int sfd, char **av)
{
	int		newfd;
	char	buf[MAX_TN_LEN];
	size_t	total;
	size_t	file_size;

	if (!av || !av[0] || !*av[0])
		return ;
	if (ft_strchr(av[0], '/') &&
		printf("get : filename cannot contain '/' character\n"))
		return ;
	if (send_code(sfd, GET, av[0]) == -1 && printf("Error: expected ACK\n"))
		return ;
	ft_memset(buf, 0, sizeof(buf));
	if (recv(sfd, buf, ft_strlen(g_com_str[ACK]), 0) == 0)
		die("Error: socket was closed");
	if (ft_strncmp(buf, g_com_str[ACK], ft_strlen(g_com_str[ACK])) &&
		printf("Error: file does not exist or is invalid\n"))
		return ;
	newfd = open(av[0], O_WRONLY | O_CREAT | O_TRUNC, 0666);
	file_size = 0;
	if (recv(sfd, &file_size, sizeof(file_size), 0) == 0)
		die("Error: socket was closed");
	total = read_data_from_request(sfd, SFILE, av[0], newfd, file_size);
	printf("Received %zu bytes in file: %s\n", total, av[0]);
	close(newfd);
}
