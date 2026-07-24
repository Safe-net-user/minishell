/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   executor.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fiaudfiz <fiaudfiz@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/23 16:51:49 by fiaudfiz          #+#    #+#             */
/*   Updated: 2026/07/24 21:18:57 by fiaudfiz         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "executor.h"

/**
 * @brief Executes an AST node according to its type.
 *
 * Dispatches the execution of a command, pipeline, logical AND, or logical
 * OR node. Logical operators are evaluated conditionally based on the exit
 * status of the left-hand side expression.
 *
 * @param mms  Pointer to the main minishell structure.
 * @param head Root node of the AST subtree to execute.
 *
 * @return The exit status of the executed command or expression.
 */

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
