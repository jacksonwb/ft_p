/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server_handler1.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jbeall <jbeall@student.42.us.org>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/07/20 16:40:16 by jbeall            #+#    #+#             */
/*   Updated: 2019/07/20 17:31:35 by jbeall           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <server.h>

void	handle_ls(int afd, char **av, char *cwd)
{
	int		dfd;
	char	buf[SERVER_BUFF_SIZE];

	(void)av;
	(void)cwd;
	dfd = establish_data_sock(afd);
	if (!ft_strcmp(cwd, "/"))
		exec_to_socket(dfd, "ls", (char*[]){"ls", "-l", NULL});
	else
	{
		ft_strcpy(buf, g_server_root);
		ft_strlcat(buf, cwd, SERVER_BUFF_SIZE);
		exec_to_socket(dfd, "ls", (char*[]){"ls", "-l", buf, NULL});
	}
	close(dfd);
}

void	handle_pwd(int afd, char **av, char *cwd)
{
	int dfd;

	(void)av;
	dfd = establish_data_sock(afd);
	write(dfd, cwd, ft_strlen(cwd));
	close(dfd);
}

int		is_valid_directory(char *cwd, char *path)
{
	struct stat	file;
	char		buf[2048];

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

void	cd_helper(char *cwd)
{
	*ft_strrchr(cwd, '/') = 0;
	if (!*cwd)
		ft_strlcat(cwd, "/", SERVER_BUFF_SIZE);
}

void	handle_cd(int afd, char **av, char *cwd)
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
			cd_helper(cwd);
	}
	else if (is_valid_directory(cwd, av[0]))
	{
		if (ft_strcmp(cwd, "/"))
			ft_strlcat(cwd, "/", SERVER_BUFF_SIZE);
		ft_strlcat(cwd, av[0], SERVER_BUFF_SIZE);
	}
	else
		PRINTFD("Error: not a valid directory\n", dfd);
	close(dfd);
}
