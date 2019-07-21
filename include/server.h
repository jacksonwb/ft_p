/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jbeall <jbeall@student.42.us.org>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/07/15 16:51:32 by jbeall            #+#    #+#             */
/*   Updated: 2019/07/20 17:50:18 by jbeall           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_H
# define SERVER_H

# include <netinet/in.h>
# include <arpa/inet.h>
# include <sys/socket.h>
# include <sys/time.h>
# include <sys/stat.h>
# include <sys/wait.h>
# include <fcntl.h>
# include <netdb.h>
# include <stdio.h>
# include <pthread.h>
# include "libft.h"
# include "get_next_line.h"
# include "com_string.h"

# define MAX_LOG_RX_MSG 2048
# define SERVER_BUFF_SIZE 1024
# define FILE_BUFF_SIZE 10000

# define NUM_CMD_CODE 5

extern char*	g_cmd_code[];

typedef struct	s_thread_args
{
	int			afd;
	char		env[1024];
}				t_thread_args;

char			*g_server_root;

/*
** server_util
*/
void			usage(void);
void			check_port(char *port_str);
int				init_server(char *port_str);
void			server_log(char *msg);
void			log_receive(int afd, char *msg);

/*
** connect_data
*/
int				server_listen_eph(void);
int				transmit_addr(int lfd, int afd);
int				establish_data_sock(int afd);
void			exec_to_socket(int dfd, char *cmd, char **av);

/*
**server_handlers
*/
void			handle_ls(int afd, char **av, char *cwd);
void			handle_pwd(int afd, char **av, char *cwd);
int				is_valid_directory(char *cwd, char *path);
void			handle_cd(int afd, char **av, char *cwd);
void			handle_get(int afd, char **av, char *cwd);
void			handle_put(int afd, char **av, char *cwd);
void			send_file_to_client(char *cwd, char *path, int dfd);
void			rec_file_from_client(char *cwd, char *path, int dfd);
size_t			get_file_size_from_filename(char *path, char *cwd);

int				is_gettable_file(char *cwd, char *path, char *fbuf);

#endif
