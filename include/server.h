/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jbeall <jbeall@student.42.us.org>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/07/15 16:51:32 by jbeall            #+#    #+#             */
/*   Updated: 2019/07/18 21:14:46 by jbeall           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_H
# define SERVER_H

#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <netdb.h>
#include <stdio.h>
#include <pthread.h>
#include "libft.h"
#include "get_next_line.h"
#include "com_string.h"

# define MAX_LOG_RX_MSG 2048
# define SERVER_BUFF_SIZE 1024
# define FILE_BUFF_SIZE 10000

# define NUM_CMD_CODE 3

char* g_cmd_code[] = {
	"LIST",
	"CWD",
	"CD",
	"GET"
};

typedef struct s_thread_args
{
	int afd;
	char env[1024];
}				t_thread_args;

char *g_server_root;

#endif