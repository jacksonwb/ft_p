/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ftp.h                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jbeall <jbeall@student.42.us.org>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/07/15 19:38:50 by jbeall            #+#    #+#             */
/*   Updated: 2019/07/20 17:51:13 by jbeall           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef FTP_H
# define FTP_H

# include <stdio.h>
# include <stdlib.h>
# include <sys/stat.h>

void		err_exit(char *str);
int			send_code(int sfd, int code, char *msg);
void		send_ack(int sfd);
void		send_bad(int sfd);
size_t		get_file_size(char *file);

#endif
