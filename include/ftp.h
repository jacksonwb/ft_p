/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ftp.h                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jbeall <jbeall@student.42.us.org>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/07/15 19:38:50 by jbeall            #+#    #+#             */
/*   Updated: 2019/07/17 19:19:37 by jbeall           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef FTP_H
# define FTP_H

#include <stdio.h>
#include <stdlib.h>

void		err_exit(char *str);
int			send_code(int sfd, int code, char *msg);
void		send_ack(int sfd);

#endif