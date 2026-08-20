/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   executor_redir.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: miouali <miouali@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/23 16:55:06 by fiaudfiz          #+#    #+#             */
/*   Updated: 2026/08/20 14:22:59 by miouali          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "executor.h"
#include <fcntl.h>
#include <unistd.h>
#include "expander.h"
#include "ft_strings.h"

static int	expand_redir_file(t_mms *mms, t_tk *op)
{
	t_tk	*file;

	file = op->next;
	if (!file)
		return (1);
	if (expand_one(mms, &file) != EXP_SUCCESS)
		return (1);
	if (!op->next || (op->next->type_tk != TOK_WORD
			&& op->next->type_tk != TOK_DELIMITER))
	{
		print_error("ambiguous redirect");
		return (1);
	}
	return (0);
}

int	redirection_in(t_mms *mms, t_tk *redir)
{
	int	fd_in;

	(void)mms;
	fd_in = open(redir->value, O_RDONLY);
	if (fd_in == -1)
	{
		perror("miniMishell");
		return (1);
	}
	if (dup2(fd_in, STDIN_FILENO) == -1)
	{
		perror("miniMishell");
		close(fd_in);
		return (1);
	}
	close(fd_in);
	return (0);
}

int	redirection_out(t_mms *mms, t_tk *op)
{
	int		fd_out;
	char	*file;

	(void)mms;
	file = op->next->value;
	if (op->type_tk == TOK_DGREAT)
		fd_out = open(file, O_WRONLY | O_CREAT | O_APPEND, 0644);
	else
		fd_out = open(file, O_WRONLY | O_CREAT | O_TRUNC, 0644);
	if (fd_out == -1)
	{
		perror("miniMishell");
		return (1);
	}
	if (dup2(fd_out, STDOUT_FILENO) == -1)
	{
		perror("miniMishell");
		close(fd_out);
		return (1);
	}
	close(fd_out);
	return (0);
}

static int	redirection_heredoc(t_tk *op)
{
	int	fd[2];

	if (pipe(fd) == -1)
	{
		perror("miniMishell");
		return (1);
	}
	if (write_heredoc_content(fd, op->heredoc_content))
		return (1);
	if (dup2(fd[0], STDIN_FILENO) == -1)
	{
		perror("miniMishell");
		close(fd[0]);
		return (1);
	}
	close(fd[0]);
	return (0);
}

int	redirection(t_mms *mms, t_ast *node)
{
	t_tk	*op;
	int		status;

	op = node->redirect;
	while (op != NULL)
	{
		if (!op->next)
			return (1);
		if (op->type_tk != TOK_DLESS)
		{
			if (expand_redir_file(mms, op))
				return (1);
		}
		if (op->type_tk == TOK_DLESS)
			status = redirection_heredoc(op);
		else if (op->type_tk == TOK_LESS)
			status = redirection_in(mms, op->next);
		else
			status = redirection_out(mms, op);
		if (status != 0)
			return (status);
		op = op->next->next;
	}
	return (0);
}
