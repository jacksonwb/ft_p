/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   com_string.h                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jbeall <jbeall@student.42.us.org>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/07/16 15:51:04 by jbeall            #+#    #+#             */
/*   Updated: 2019/07/20 17:50:45 by jbeall           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef COM_STRING_H
# define COM_STRING_H

# define MAX_TN_LEN 200

# define LIST 0
# define CWD 1
# define ACK 2
# define CD 3
# define GET 4
# define IS_FILE 5
# define BAD 6
# define SFILE 7
# define PUT 8
# define TFILE 9

extern char *g_com_str[];

#endif
