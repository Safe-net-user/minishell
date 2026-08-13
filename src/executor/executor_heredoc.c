/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   executor_heredoc.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fiaudfiz <fiaudfiz@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/23 16:55:56 by fiaudfiz          #+#    #+#             */
/*   Updated: 2026/08/13 11:16:31 by fiaudfiz         ###   ########.fr       */
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

static void	restore_canonical_tty(int fd)
{
	struct termios	term;

	if (tcgetattr(fd, &term) == -1)
		return ;
	term.c_iflag |= ICRNL;
	term.c_lflag |= ICANON | ECHO;
	tcsetattr(fd, TCSANOW, &term);
}

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
	tk_ptr = &tmp;
	if (expand_one(mms, &tk_ptr) != EXP_SUCCESS)
		return (NULL);
	if (!tk_ptr)
		return (ft_strdup(""));
	result = tk_ptr->value;
	if (!result)
		return (ft_strdup(""));
	return (result);
}

/**
 * @brief Reads a line from standard input and writes it to the
 *        here-document pipe.
 *
 * Prompts the user for input, reads a line from standard input, and compares it
 * with the here-document delimiter.If the delimiter is reached or an end-of-file
 * is encountered, the function stops reading.
 *
 * @param[in]  fd_here_doc File descriptors of the here-document pipe.
 * @param[in]  lim_nl      Delimiter marking the end of the here-document.
 *
 * @return 1 if the delimiter is reached or EOF is encountered,
 *         0 otherwise, -1 if the write to the pipe fails.
 */

int	execute_here_doc(t_mms *mms, int *fd_here_doc, char *lim_nl, bool expand)
{
	char	*line;
	char	*out;
	int		interrupted;

	write(STDOUT_FILENO, "> ", 2);
	line = heredoc_gnl(mms->tty_fd, &interrupted);
	if (interrupted)
	{
		free(line);
		return (-2);
	}
	if (!line)
	{
		ft_putstr_fd("minishell: warning: here-document delimited "
			"by end-of-file\n", STDERR_FILENO);
		return (1);
	}
	if (line_matches_delim(line, lim_nl))
	{
		free(line);
		return (1);
	}
	if (expand)
	{
		out = expand_hd_line(mms, line);
		if (!out || write(fd_here_doc[1], out, ft_strlen(out)) == -1)
		{
			free(out);
			return (-1);
		}
		free(out);
		return (0);
	}
	if (write(fd_here_doc[1], line, ft_strlen(line)) == -1)
	{
		free(line);
		return (-1);
	}
	free(line);
	return (0);
}

static int	init_here_doc(t_tk *redir, int *fd_here_doc, char **lim_nl)
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

static int	fill_here_doc(t_mms *mms, int *fd_here_doc,
		char *lim_nl, bool expand)
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

static void	here_doc_child(t_mms *mms, int *fd_here_doc,
		char *lim_nl, bool expand)
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

static int	check_hd_status(t_mms *mms, int status, int fd_read)
{
	if (WIFEXITED(status) && WEXITSTATUS(status) == 130)
	{
		mms->last_status = 130;
		close(fd_read);
		return (-2);
	}
	if (WIFSIGNALED(status) && WTERMSIG(status) == SIGINT)
	{
		mms->last_status = 130;
		close(fd_read);
		return (-2);
	}
	return (0);
}

int	here_doc(t_mms *mms, t_tk *redir)
{
	int		fd_here_doc[2];
	char	*lim_nl;
	pid_t	pid;
	int		status;
	bool	expand;

	if (init_here_doc(redir, fd_here_doc, &lim_nl) == -1)
		return (-1);
	expand = !is_delim_quoted(redir);
	pid = fork();
	if (pid == -1)
	{
		perror("minishell");
		free(lim_nl);
		close(fd_here_doc[0]);
		close(fd_here_doc[1]);
		return (-1);
	}
	if (pid == 0)
		here_doc_child(mms, fd_here_doc, lim_nl, expand);
	close(fd_here_doc[1]);
	free(lim_nl);
	signal(SIGINT, SIG_IGN);
	signal(SIGQUIT, SIG_IGN);
	waitpid(pid, &status, 0);
	set_signaux_interactif();
	if (check_hd_status(mms, status, fd_here_doc[0]) == -2)
		return (-2);
	return (fd_here_doc[0]);
}
