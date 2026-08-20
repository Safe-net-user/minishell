/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   executor_heredoc.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: miouali <miouali@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/23 16:55:56 by fiaudfiz          #+#    #+#             */
/*   Updated: 2026/08/20 13:40:47 by miouali          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "executor.h"
#include <unistd.h>
#include "ft_strings.h"
#include "signal.h"
#include "sys/wait.h"
#include <termios.h>
#include "ft_io.h"
#include "expander.h"
#include "gnl.h"

static char	*expand_hd_line(t_mms *mms, char *line)
{
	t_tk	tmp;
	t_tk	*tk_ptr;
	char	*result;

	tmp.value = line;
	tmp.type_tk = TOK_WORD;
	tmp.flags = 0;
	tmp.next = NULL;
	tmp.prev = NULL;
	tmp.heredoc_content = NULL;
	tk_ptr = &tmp;
	if (expand_one(mms, &tk_ptr) != EXP_SUCCESS)
		return (NULL);
	if (!tk_ptr || !tk_ptr->value)
		return (ft_strdup(""));
	result = tk_ptr->value;
	return (result);
}

 static bool	is_delim_quoted(t_tk *redir)
{
	if (redir->flags & TOKF_SQUOTE)
		return (true);
	if (redir->flags & TOKF_DQUOTE)
		return (true);
	return (false);
}

char	*here_doc(t_mms *mms, t_tk *redir)
{
	char	*content;
	char	*lim_nl;
	char	*line;
	char	*tmp;
	bool	expand;
	int		interrupted;

	set_signaux_heredoc();
	lim_nl = ft_strdup(redir->value);
	if (!lim_nl)
	{
		set_signaux_interactif();
		return (NULL);
	}
	content = ft_strdup("");
	if (!content)
	{
		free(lim_nl);
		set_signaux_interactif();
		return (NULL);
	}
	expand = !is_delim_quoted(redir);
	while (1)
	{
		write(STDOUT_FILENO, "> ", 2);

		line = gnl(mms->tty_fd, &interrupted);
		if (interrupted)
		{
			free(line);
			free(content);
			free(lim_nl);
			gnl_reset();
			mms->last_status = 130;
			set_signaux_interactif();
			return (NULL);
		}
		if (!line)
		{
			ft_putstr_fd(
				"miniMishell: warning: here-document delimited "
				"by end-of-file\n", STDERR_FILENO);
			break ;
		}
		if (line_matches_delim(line, lim_nl))
		{
			free(line);
			break ;
		}
		if (expand)
		{
			tmp = expand_hd_line(mms, line);
			line = tmp;
			if (!line)
			{
				free(content);
				free(lim_nl);
				gnl_reset();
				set_signaux_interactif();
				return (NULL);
			}
		}
		tmp = ft_strjoin_free(content, line);
		free(line);
		if (!tmp)
		{
			free(content);
			free(lim_nl);
			gnl_reset();
			set_signaux_interactif();
			return (NULL);
		}
		content = tmp;
	}
	free(lim_nl);
	gnl_reset();
	set_signaux_interactif();
	return (content);
}
