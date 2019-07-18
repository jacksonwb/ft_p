/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jbeall <jbeall@student.42.us.org>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/07/15 21:00:15 by jbeall            #+#    #+#             */
/*   Updated: 2019/07/17 19:21:29 by jbeall           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <sys/socket.h>
#include "ftp.h"
#include "com_string.h"
#include "libft.h"

# define LIST 0
# define CWD 1
# define ACK 2

char *g_com_str[] = {
	"LIST",
	"CWD",
	"ACK"
};

void		err_exit(char *str)
{
	perror(str);
	exit(EXIT_FAILURE);
}

int send_code(int sfd, int code, char *msg)
{
	char out[MAX_TN_LEN];
	char buf[MAX_TN_LEN];

	snprintf(out, MAX_TN_LEN, "%s%s%s\n", g_com_str[code], msg ? ":" : "", msg ? msg : "");
	if (send(sfd, out, ft_strlen(out), 0) <= 0)
		err_exit("send");
	if (recv(sfd, buf, ft_strlen(g_com_str[ACK]), 0) == 0)
		die("Error: socket was closed");
	if (ft_strcmp(buf, g_com_str[ACK]))
		return (-1);
	return (0);
}

void send_ack(int sfd)
{
	char out[MAX_TN_LEN];

	snprintf(out, MAX_TN_LEN, "%s", g_com_str[ACK]);
	if (send(sfd, out, ft_strlen(out), 0) <= 0)
		err_exit("send");
}
