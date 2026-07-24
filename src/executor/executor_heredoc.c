/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   executor_heredoc.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fiaudfiz <fiaudfiz@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/23 16:55:56 by fiaudfiz          #+#    #+#             */
/*   Updated: 2026/07/23 23:43:40 by fiaudfiz         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "executor.h"
#include "gnl.h"
#include <unistd.h>
#include "ft_strings.h"

/**
 * @brief Reads a line from standard input and writes it to the here-document pipe.
 *
 * Prompts the user for input, reads a line from standard input, and compares it
 * with the here-document delimiter. If the delimiter is reached or an end-of-file
 * is encountered, the function stops reading.
 *
 * @param[in]  fd_here_doc File descriptors of the here-document pipe.
 * @param[in]  lim_nl      Delimiter marking the end of the here-document.
 *
 * @return 1 if the delimiter is reached or EOF is encountered, 0 otherwise,
 *         -1 if the write to the pipe fails.
 */

int	execute_here_doc(int *fd_here_doc, char *lim_nl)
{
	char	*line;

	fprintf(stdout, "> ");
	line = get_next_line(0);
	if (!line || ft_strcmp(line, lim_nl) == 0)
	{
		free(line);
		return (1);
	}
	if (write(fd_here_doc[1], line, ft_strlen(line)) == -1)
	{
		perror("minishell");
		free(line);
		return (-1);
	}
	free(line);
	return (0);
}

static int	init_here_doc(t_redir *redir, int *fd_here_doc, char **lim_nl)
{
	if (pipe(fd_here_doc) == -1)
	{
		perror("minishell");
		return (-1);
	}
	*lim_nl = ft_strjoin(redir->file, "\n");
	if (!*lim_nl)
	{
		print_error("memory allocation failed");
		close(fd_here_doc[0]);
		close(fd_here_doc[1]);
		return (-1);
	}
	return (0);
}

static int	here_doc_error(int *fd_here_doc, char *lim_nl)
{
	get_next_line(-1);
	free(lim_nl);
	close(fd_here_doc[0]);
	close(fd_here_doc[1]);
	return (-1);
}

static int	fill_here_doc(int *fd_here_doc, char *lim_nl)
{
	int	res;

	while (1)
	{
		res = execute_here_doc(fd_here_doc, lim_nl);
		if (res == -1)
			return (1);
		if (res == 1)
			break ;
	}
	return (0);
}

/**
 * @brief Reads and stores here-document input into a pipe.
 *
 * Creates a pipe and continuously reads lines from standard input until the
 * here-document delimiter is encountered or EOF is reached. The input is
 * written to the pipe, whose read end is returned for later use as standard input.
 *
 * @param[in]  mms  Pointer to the minishell main structure.
 * @param[in]  redir Here-document redirection containing the delimiter.
 *
 * @return The read end of the here-document pipe on success, -1 on error.
 */

int	here_doc(t_mms *mms, t_redir *redir)
{
	int		fd_here_doc[2];
	char	*lim_nl;

	(void)mms;
	if (init_here_doc(redir, fd_here_doc, &lim_nl) == -1)
		return (-1);
	if (fill_here_doc(fd_here_doc, lim_nl) == 1)
		return (here_doc_error(fd_here_doc, lim_nl));
	get_next_line(-1);
	free(lim_nl);
	close(fd_here_doc[1]);
	return (fd_here_doc[0]);
}
