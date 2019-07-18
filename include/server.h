/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jbeall <jbeall@student.42.us.org>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/07/15 16:51:32 by jbeall            #+#    #+#             */
/*   Updated: 2019/07/17 20:52:35 by jbeall           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_H
# define SERVER_H

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/time.h>
#include <netdb.h>
#include <stdio.h>
#include <pthread.h>
#include "libft.h"
#include "get_next_line.h"
#include "com_string.h"

# define MAX_LOG_RX_MSG 2048

# define NUM_CMD_CODE 1

char* g_cmd_code[] = {
	"LIST"
};

typedef struct s_thread_args
{
	int afd;
	char env[1024];
}				t_thread_args;

char *g_server_root;

#endif