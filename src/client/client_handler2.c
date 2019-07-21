/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   client_handler2.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jbeall <jbeall@student.42.us.org>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/07/20 17:57:01 by jbeall            #+#    #+#             */
/*   Updated: 2019/07/20 18:35:05 by jbeall           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "client.h"
#include "ftp.h"

void	handle_put(int sfd, char **av)
{
	int		filefd;
	size_t	total;
	size_t	file_size;

	if (!av || !av[0] || !*av[0])
		return ;
	if (ft_strchr(av[0], '/') &&
		printf("put : filename cannot contain '/' character\n"))
		return ;
	if ((filefd = open(av[0], O_RDONLY)) == -1)
	{
		perror("put");
		return ;
	}
	if (send_code(sfd, PUT, av[0]) == -1)
	{
		printf("Error: expected ACK\n");
		return ;
	}
	total = 0;
	file_size = get_file_size(av[0]);
	total = send_data_for_request(sfd, TFILE, av[0], filefd, file_size);
	printf("Sent %zu bytes in file: %s\n", total, av[0]);
	close(filefd);
}

void	handle_quit(int sfd, char **av)
{
	(void)av;
	(void)sfd;
	exit(EXIT_SUCCESS);
}

void	handle_lls(int sfd, char **av)
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

void	handle_lcd(int sfd, char **av)
{
	char buf[MAXPATHLEN];

	(void)sfd;
	if (chdir(av[0]) == -1)
		perror("lcd");
	setenv("OLDPWD", getenv("PWD"), 1);
	setenv("PWD", getcwd(buf, MAXPATHLEN), 1);
}

void	handle_lpwd(int sfd, char **av)
{
	char buf[MAXPATHLEN];

	(void)sfd;
	(void)av;
	printf("%s\n", getcwd(buf, MAXPATHLEN));
}
