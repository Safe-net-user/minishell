/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   executor.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: miouali <miouali@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/23 16:51:49 by fiaudfiz          #+#    #+#             */
/*   Updated: 2026/08/20 10:51:13 by miouali          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

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
