/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   client.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jbeall <jbeall@student.42.us.org>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/07/15 16:53:48 by jbeall            #+#    #+#             */
/*   Updated: 2019/07/19 17:26:12 by jbeall           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CLIENT_H
# define CLIENT_H

#include <sys/socket.h>
#include <sys/param.h>
#include <netdb.h>
#include <stdbool.h>
#include <fcntl.h>
#include "libft.h"
#include "get_next_line.h"
#include "com_string.h"


# define PROMPT "(client)"
# define PURPLE "\x1b[38;5;135m"
# define RESET "\x1b[0m"

#define NUM_CMDS 9
char* g_cmd_str[] = {
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

#endif