/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heredoc.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fiaudfiz <fiaudfiz@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/23 16:55:56 by fiaudfiz          #+#    #+#             */
/*   Updated: 2026/08/24 16:39:26 by fiaudfiz         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "heredoc.h"
#include <unistd.h>
#include "ft_strings.h"
#include "ft_io.h"
#include "expander.h"
#include "gnl.h"
#include "ft_memory.h"

static bool	is_delim_quoted(t_tk *redir)
{
	if (redir->flags & TOKF_SQUOTE)
		return (true);
	if (redir->flags & TOKF_DQUOTE)
		return (true);
	return (false);
}

static int	hd_got_delim_or_eof(char *line, char *lim_nl)
{
	if (!line)
	{
		ft_putstr_fd("miniMishell: warning: here-document delimited "
			"by end-of-file\n", STDERR_FILENO);
		return (1);
	}
	if (line_matches_delim(line, lim_nl))
	{
		free(line);
		return (1);
	}
	return (0);
}

static void	hd_append_history(t_mms *mms, char *line)
{
	char	*tmp;

	if (!mms->history_buffer)
		return ;
	tmp = ft_strjoin_free(mms->history_buffer, "\n");
	if (!tmp)
	{
		mms->history_buffer = NULL;
		return ;
	}
	mms->history_buffer = ft_strjoin_free(tmp, line);
}

static char	*hd_read_loop(t_mms *mms, char *lim_nl, bool expand)
{
	char	*content;
	char	*line;
	int		interrupted;

	content = ft_strdup("");
	if (!content)
		return (NULL);
	while (1)
	{
		write(STDOUT_FILENO, "> ", 2);
		line = gnl(mms->tty_fd, &interrupted);
		if (interrupted)
			return (hd_interrupted(mms, line, content, lim_nl));
		if (line)
			hd_append_history(mms, line);
		if (hd_got_delim_or_eof(line, lim_nl))
			break ;
		if (hd_process_line(mms, &content, line, expand))
		{
			hd_cleanup(content, lim_nl);
			return (NULL);
		}
	}
	return (content);
}

char	*here_doc(t_mms *mms, t_tk *redir)
{
	char	*lim_nl;
	char	*content;
	bool	expand;

	set_signaux_heredoc();
	lim_nl = ft_strdup(redir->value);
	if (!lim_nl)
	{
		set_signaux_interactif();
		return (NULL);
	}
	strip_quotes(lim_nl);
	expand = !is_delim_quoted(redir);
	content = hd_read_loop(mms, lim_nl, expand);
	if (!content)
		return (NULL);
	free(lim_nl);
	gnl_reset();
	set_signaux_interactif();
	return (content);
}
