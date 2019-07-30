/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   client.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jbeall <jbeall@student.42.us.org>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/07/15 16:53:48 by jbeall            #+#    #+#             */
/*   Updated: 2019/07/29 20:17:21 by jbeall           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CLIENT_H
# define CLIENT_H

# include <sys/socket.h>
# include <sys/param.h>
# include <sys/wait.h>
# include <arpa/inet.h>
# include <sys/stat.h>
# include <netdb.h>
# include <stdbool.h>
# include <fcntl.h>
# include "libft.h"
# include "get_next_line.h"
# include "com_string.h"

# define PROMPT "(client)"
# define PURPLE "\x1b[38;5;135m"
# define RESET "\x1b[0m"

# define NUM_CMDS 9

# define TCC t_client_cmd

extern char* g_cmd_str[];

typedef struct	s_client_cmd
{
	int			cmd;
	char		*msg;
}				t_client_cmd;

/*
** client_handler
*/
void	handle_ls(int sfd, char **av);
void	handle_pwd(int sfd, char **av);
void	handle_cd(int sfd, char **av);
void	handle_get(int sfd, char **av);
void	handle_put(int sfd, char **av);
void	handle_quit(int sfd, char **av);
void	handle_lls(int sfd, char **av);
void	handle_lcd(int sfd, char **av);
void	handle_lpwd(int sfd, char **av);

/*
** utility
*/
void	write_progress(size_t sent, size_t total);
void	clear_progress(void);
void	usage(void);
bool	prompt(void);
void	write_progress(size_t sent, size_t total);
void	clear_progress(void);
int		client_connect(char *addr_str, char *port_str);

size_t	read_data_from_request(int sfd, t_client_cmd cmd,
	int outfd, size_t file_size);
size_t	send_data_for_request(int sfd, t_client_cmd cmd,
	int infd, size_t file_size);

#endif
