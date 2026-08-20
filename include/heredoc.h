/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heredoc.h                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: miouali <miouali@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/20 14:57:10 by miouali           #+#    #+#             */
/*   Updated: 2026/08/20 16:34:34 by miouali          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef HEREDOC_H
# define HEREDOC_H

# include "minishell.h"

/* Heredoc */

char	*here_doc(t_mms *mms, t_tk *redir);
char	*expand_hd_line(t_mms *mms, char *line);
int		write_heredoc_content(int fd[2], char *content);
void	hd_cleanup(char *content, char *lim_nl);
char	*hd_handle_line(t_mms *mms, char *line, bool expand);
int		hd_process_line(t_mms *mms, char **content, char *line,
			bool expand);
char	*hd_interrupted(t_mms *mms, char *line, char *content,
			char *lim_nl);

#endif
