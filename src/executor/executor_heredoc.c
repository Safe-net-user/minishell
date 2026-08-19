/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   executor_heredoc.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gd-hallu <gd-hallu@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/23 16:55:56 by fiaudfiz          #+#    #+#             */
/*   Updated: 2026/08/19 16:15:55 by gd-hallu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "executor.h"
#include <unistd.h>
#include "ft_strings.h"
#include "signal.h"
#include "sys/wait.h"
#include "heredoc.h"
#include <termios.h>
#include "ft_io.h"
#include "expander.h"

/*
static void	restore_canonical_tty(int fd)
{
	struct termios	term;

	if (tcgetattr(fd, &term) == -1)
		return ;
	term.c_iflag |= ICRNL;
	term.c_lflag |= ICANON | ECHO;
	tcsetattr(fd, TCSANOW, &term);
}
*/
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
	result = ft_strdup(tk_ptr->value);
	return (result);
}

/**
 * @brief Reads and stores here-document input into a pipe.
 *
 * Creates a pipe and continuously reads lines from standard input
 * until the here-document delimiter is encountered or EOF is reached.
 * The input is written to the pipe, whose read end is returned for 
 * later use as standard input.
 *
 * @param[in]  mms  Pointer to the minishell main structure.
 * @param[in]  redir Here-document redirection containing the delimiter.
 *
 * @return The read end of the here-document pipe on success, -1 on error.
 */

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

		line = heredoc_gnl(mms->tty_fd, &interrupted);

		if (interrupted)
		{
			free(line);
			free(content);
			free(lim_nl);
			heredoc_gnl_reset();
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
			free(line);
			line = tmp;
			if (!line)
			{
				free(content);
				free(lim_nl);
				heredoc_gnl_reset();
				set_signaux_interactif();
				return (NULL);
			}
		}

		tmp = hd_strjoin_free(content, line);
		free(line);
		if (!tmp)
		{
			free(content);
			free(lim_nl);
			heredoc_gnl_reset();
			set_signaux_interactif();
			return (NULL);
		}
		content = tmp;
	}

	free(lim_nl);
	heredoc_gnl_reset();
	set_signaux_interactif();
	return (content);
}
