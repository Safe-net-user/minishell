/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heredoc.h                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fiaudfiz <fiaudfiz@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/25 00:56:41 by fiaudfiz          #+#    #+#             */
/*   Updated: 2026/07/25 01:07:37 by fiaudfiz         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef HEREDOC_GNL_H
# define HEREDOC_GNL_H

# include <stdlib.h>
# include <unistd.h>

# ifndef HD_BUFFER_SIZE
#  define HD_BUFFER_SIZE 1024
# endif

char	*heredoc_gnl(int fd);
void	heredoc_gnl_reset(void);
size_t	hd_strlen(const char *s);
char	*hd_strchr(const char *s, int c);
char	*hd_strjoin_free(char *s1, char *s2);
int	line_matches_delim(char *line, char *lim);

#endif