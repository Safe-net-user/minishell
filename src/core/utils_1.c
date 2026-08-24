/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils_1.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fiaudfiz <fiaudfiz@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/17 11:37:49 by fiaudfiz          #+#    #+#             */
/*   Updated: 2026/08/24 14:02:22 by fiaudfiz         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include "executor.h"
#include <readline/history.h>
#include <unistd.h>

int	get_exit_status(t_mms *mms)
{
	if (mms->should_exit)
		return (mms->exit_status);
	return (mms->last_status);
}

void	run_parser_and_exec(t_mms *mms)
{
	t_ast	*head;

	head = parser(mms);
	if (head)
	{
		mms->current_ast = head;
		mms->last_status = executor(mms, head);
		free_ast_values(head);
		mms->current_ast = NULL;
	}
}

void	commit_pending_history(t_mms *mms)
{
	if (mms->history_buffer)
	{
		add_history(mms->history_buffer);
		free(mms->history_buffer);
		mms->history_buffer = NULL;
	}
}

void	init_og_fds(t_mms *mms)
{
	mms->tty_fd = -1;
	mms->st = malloc(sizeof(t_st));
}

int	init_og_check(t_mms *mms)
{
	if (!mms->env || !mms->cmd_path || !mms->sa || !mms->name || !mms->st)
	{
		free_og_struct(mms);
		return (0);
	}
	if (tcgetattr(STDIN_FILENO, mms->st) == -1)
	{
		free_og_struct(mms);
		return (0);
	}
	return (1);
}
