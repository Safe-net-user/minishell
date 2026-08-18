/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   executor_heredoc_utils_3.c                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fiaudfiz <fiaudfiz@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/17 12:21:48 by fiaudfiz          #+#    #+#             */
/*   Updated: 2026/08/17 12:21:51 by fiaudfiz         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "executor.h"
#include <unistd.h>
#include "ft_strings.h"
#include "signal.h"
#include "heredoc.h"
#include <termios.h>

static void	restore_canonical_tty(int fd)
{
	struct termios	term;

	if (tcgetattr(fd, &term) == -1)
		return ;
	term.c_iflag |= ICRNL;
	term.c_lflag |= ICANON | ECHO;
	tcsetattr(fd, TCSANOW, &term);
}

int	init_here_doc(t_tk *redir, int *fd_here_doc, char **lim_nl)
{
	if (pipe(fd_here_doc) == -1)
	{
		perror("minishell");
		return (-1);
	}
	*lim_nl = ft_strdup(redir->value);
	if (!*lim_nl)
	{
		print_error("memory allocation failed");
		close(fd_here_doc[0]);
		close(fd_here_doc[1]);
		return (-1);
	}
	return (0);
}

int	fill_here_doc(t_mms *mms, int *fd_here_doc, char *lim_nl, bool expand)
{
	int	res;

	while (1)
	{
		res = execute_here_doc(mms, fd_here_doc, lim_nl, expand);
		if (res == -2)
			return (130);
		if (res == -1)
			return (1);
		if (res == 1)
			break ;
	}
	return (0);
}

void	here_doc_child(t_mms *mms, int *fd_here_doc, char *lim_nl, bool expand)
{
	int	status;

	close(fd_here_doc[0]);
	signal(SIGINT, SIG_DFL);
	restore_canonical_tty(mms->tty_fd);
	status = fill_here_doc(mms, fd_here_doc, lim_nl, expand);
	heredoc_gnl_reset();
	free(lim_nl);
	close(fd_here_doc[1]);
	exit(status);
}

bool	is_delim_quoted(t_tk *redir)
{
	if (redir->flags & TOKF_SQUOTE)
		return (true);
	if (redir->flags & TOKF_DQUOTE)
		return (true);
	return (false);
}
