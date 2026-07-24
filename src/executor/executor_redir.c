/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   executor_redir.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fiaudfiz <fiaudfiz@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/23 16:55:06 by fiaudfiz          #+#    #+#             */
/*   Updated: 2026/07/24 16:19:00 by fiaudfiz         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "executor.h"
#include <fcntl.h>
#include <unistd.h>
#include "expander.h"

static int	expand_redir_file(t_mms *mms, t_redir *redir)
{
	t_tk	tk;
	t_tk	*arr[2];
	t_tk	**arr_ptr;

	tk.value = redir->file;
	tk.flags = 0;
	tk.type_tk = TOK_WORD;
	arr[0] = &tk;
	arr[1] = NULL;
	arr_ptr = arr;
	if (!expand(mms, &arr_ptr))
		return (1);
	redir->file = arr[0]->value;
	return (0);
}

/**
 * @brief Redirects standard input from a file.
 *
 * Opens the input file specified by the redirection and replaces the current
 * standard input file descriptor with the opened file.
 *
 * @param[in] mms   Pointer to the minishell main structure.
 * @param[in] redir Input redirection containing the file path.
 *
 * @return 0 on success, 1 if the file cannot be opened or standard input
 *         cannot be redirected.
 */

int	redirection_in(t_mms *mms, t_redir *redir)
{
	int	fd_in;

	(void)mms;
	fd_in = open(redir->file, O_RDONLY);
	if (fd_in == -1)
	{
		perror("minishell");
		return (1);
	}
	if (dup2(fd_in, STDIN_FILENO) == -1)
	{
		perror("minishell");
		close(fd_in);
		return (1);
	}
	close(fd_in);
	return (0);
}

/**
 * @brief Redirects standard output to a file.
 *
 * Opens the output file in append mode for the `>>` operator or truncation mode
 * for the `>` operator, then replaces the current standard output file
 * descriptor with the opened file.
 *
 * @param[in] mms   Pointer to the minishell main structure.
 * @param[in] redir Output redirection containing 
 * the file path and redirection type.
 *
 * @return 0 on success, 1 if the file cannot be opened or standard output
 *         cannot be redirected.
 */

int	redirection_out(t_mms *mms, t_redir *redir)
{
	int	fd_out;

	(void)mms;
	if (redir->type_tk == TOK_DGREAT)
		fd_out = open(redir->file,
				O_WRONLY | O_CREAT | O_APPEND, 0644);
	else
		fd_out = open(redir->file,
				O_WRONLY | O_CREAT | O_TRUNC, 0644);
	if (fd_out == -1)
	{
		perror("minishell");
		return (1);
	}
	if (dup2(fd_out, STDOUT_FILENO) == -1)
	{
		perror("minishell");
		close(fd_out);
		return (1);
	}
	close(fd_out);
	return (0);
}

static int	redirection_heredoc(t_mms *mms, t_redir *redir)
{
	int	fd_heredoc;

	fd_heredoc = here_doc(mms, redir);
	if (fd_heredoc == -1)
		return (1);
	if (dup2(fd_heredoc, STDIN_FILENO) == -1)
	{
		perror("minishell");
		close(fd_heredoc);
		return (1);
	}
	close(fd_heredoc);
	return (0);
}

/**
 * @brief Applies all redirections associated with an AST node.
 *
 * Iterates through the list of redirections attached to the node and applies
 * each one according to its type: here-document, input redirection, or output
 * redirection. Stops immediately if a redirection fails.
 *
 * @param[in] mms  Pointer to the minishell main structure.
 * @param[in] node AST node containing the redirections to apply.
 *
 * @return 0 if all redirections are successfully applied, otherwise the error
 *         status returned by the failed redirection.
 */

int	redirection(t_mms *mms, t_ast *node)
{
	t_redir	*redir;
	int		status;

	redir = node->redirect;
	while (redir != NULL)
	{
		if (redir->type_tk != TOK_DLESS)
		{
			if (expand_redir_file(mms, redir))
				return (1);
		}
		if (redir->type_tk == TOK_DLESS)
			status = redirection_heredoc(mms, redir);
		else if (redir->type_tk == TOK_LESS)
			status = redirection_in(mms, redir);
		else
			status = redirection_out(mms, redir);
		if (status != 0)
			return (status);
		redir = redir->next;
	}
	return (0);
}
