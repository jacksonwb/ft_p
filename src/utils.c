/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jbeall <jbeall@student.42.us.org>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/07/15 21:00:15 by jbeall            #+#    #+#             */
/*   Updated: 2019/07/18 21:04:43 by jbeall           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <sys/socket.h>
#include "ftp.h"
#include "com_string.h"
#include "libft.h"

char *g_com_str[] = {
	"LIST",
	"CWD",
	"ACK",
	"CD",
	"GET",
	"IS_FILE",
	"BAD",
	"FILE"
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
	ft_memset(buf, '\0', sizeof(buf));
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
	if (send(sfd, out, ft_strlen(g_com_str[ACK]), 0) <= 0)
		err_exit("send");
}

void send_bad(int sfd)
{
	char out[MAX_TN_LEN];

	snprintf(out, MAX_TN_LEN, "%s", g_com_str[BAD]);
	if (send(sfd, out, ft_strlen(g_com_str[BAD]), 0) <= 0)
		err_exit("send");
}