/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   client.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jbeall <jbeall@student.42.us.org>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/07/15 16:53:48 by jbeall            #+#    #+#             */
/*   Updated: 2019/07/16 20:14:45 by jbeall           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CLIENT_H
# define CLIENT_H

#include <sys/socket.h>
#include <netdb.h>
#include <stdbool.h>
#include "libft.h"
#include "get_next_line.h"
#include "com_string.h"


#define PROMPT "(client)> "

#define NUM_CMDS 6

char* g_cmd_str[] = {
	"ls",
	"cd",
	"get",
	"put",
	"pwd",
	"quit"
};

#endif