/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jbeall <jbeall@student.42.us.org>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/07/15 21:00:15 by jbeall            #+#    #+#             */
/*   Updated: 2019/07/17 13:38:08 by jbeall           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <sys/socket.h>
#include "ftp.h"
#include "com_string.h"
#include "libft.h"

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

void send_code(int sfd, int code, char *msg)
{
	char out[MAX_TN_LEN];
	char buf[MAX_TN_LEN];

	snprintf(out, MAX_TN_LEN, "%s:%s\n", g_com_str[code], msg);
	if (send(sfd, out, ft_strlen(out), 0) <= 0)
		err_exit("send");
	if (recv(sfd, buf, MAX_TN_LEN, 0) == 0)
		die("Error: socket was closed");
}

void send_ack(int sfd)
{
	char out[MAX_TN_LEN];

	snprintf(out, MAX_TN_LEN, "%s\n", g_com_str[ACK]);
	if (send(sfd, out, ft_strlen(out), 0) <= 0)
		err_exit("send");
}
