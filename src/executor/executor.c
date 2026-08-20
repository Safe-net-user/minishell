/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   executor.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: miouali <miouali@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/23 16:51:49 by fiaudfiz          #+#    #+#             */
/*   Updated: 2026/08/20 16:03:28 by miouali          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/**
 * @file executor.c
 * @brief Root of the executor — recursive AST dispatch.
 *
 * `executor()` walks the AST recursively, dispatching on node type:
 * - NODE_CMD  → `execute_cmd()` (fork + exec, or run in-process for
 *               builtins that must affect shell state, e.g. `cd`)
 * - NODE_AND  → right side runs only if the left side succeeded
 * - NODE_OR   → right side runs only if the left side failed
 * - NODE_PIPE → `pipeline()`, which forks one process per stage and
 *               connects them via pipe()/dup2()
 *
 * The exit status of whichever branch actually ran becomes both the
 * return value and the new `mms->last_status`, which is what `$?`
 * reflects in the next command.
 */

#include "executor.h"

int	executor(t_mms *mms, t_ast *head)
{
	t_ast	*tmp;
	int		exit_status;

	if (!head)
		return (0);
	tmp = head;
	exit_status = 0;
	if (tmp->type == NODE_CMD)
		exit_status = execute_cmd(mms, tmp);
	else if (tmp->type == NODE_AND)
	{
		exit_status = executor(mms, tmp->left);
		if (exit_status == 0)
			exit_status = executor(mms, tmp->right);
	}
	else if (tmp->type == NODE_OR)
	{
		exit_status = executor(mms, tmp->left);
		if (exit_status != 0)
			exit_status = executor(mms, tmp->right);
	}
	else if (tmp->type == NODE_PIPE)
		exit_status = pipeline(mms, tmp);
	mms->last_status = exit_status;
	return (exit_status);
}
