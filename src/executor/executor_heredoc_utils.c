/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   executor_heredoc_utils.c                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: miouali <miouali@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/20 14:19:45 by miouali           #+#    #+#             */
/*   Updated: 2026/08/20 14:36:52 by miouali          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "executor.h"
#include <unistd.h>
#include "ft_strings.h"
#include "gnl.h"

int	write_heredoc_content(int fd[2], char *content)
{
	ssize_t	len;

	len = write(fd[1], content, ft_strlen(content));
	if (len == -1)
	{
		perror("miniMishell");
		close(fd[0]);
		close(fd[1]);
		return (1);
	}
	close(fd[1]);
	return (0);
}

void	hd_cleanup(char *content, char *lim_nl)
{
	free(content);
	free(lim_nl);
	gnl_reset();
	set_signaux_interactif();
}

char	*hd_handle_line(t_mms *mms, char *line, bool expand)
{
	if (!expand)
		return (line);
	return (expand_hd_line(mms, line));
}

int	hd_process_line(t_mms *mms, char **content, char *line,
		bool expand)
{
	char	*tmp;

	tmp = hd_handle_line(mms, line, expand);
	if (!tmp)
		return (1);
	line = tmp;
	tmp = ft_strjoin_free(*content, line);
	free(line);
	if (!tmp)
		return (1);
	*content = tmp;
	return (0);
}

char	*hd_interrupted(t_mms *mms, char *line, char *content,
		char *lim_nl)
{
	free(line);
	mms->last_status = 130;
	hd_cleanup(content, lim_nl);
	return (NULL);
}
